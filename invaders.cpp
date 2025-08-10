#include "invaders.hpp"
#include "assets.hpp"

using namespace blit;

extern const uint8_t font_space_inv[];
static Font space_font{ font_space_inv };

static Surface *invaders_sheet = Surface::load(sprites_invaders);

//Spritesheet config
static const int SHEET_W = invaders_sheet->bounds.w;
static const int SHEET_H = invaders_sheet->bounds.h;
static const int COLS = 3;
static const int ROWS = 3;
static const int CELL_W = SHEET_W / COLS;
static const int CELL_H = SHEET_H / ROWS;

Rect cell_rect(int col, int row) {
    return Rect(
        col * CELL_W,
        row * CELL_H,
        CELL_W,
        CELL_H
    );
}

static const Rect SRC_PLAYER  = cell_rect(0, 0);

static const Rect SRC_A0_F0   = cell_rect(1, 0);
static const Rect SRC_A0_F1   = cell_rect(2, 0);

static const Rect SRC_A1_F0   = cell_rect(1, 1);
static const Rect SRC_A1_F1   = cell_rect(2, 1);

static const Rect SRC_A2_F0   = cell_rect(1, 2);
static const Rect SRC_A2_F1   = cell_rect(2, 2);

static const Rect EXP_ALIEN  = cell_rect(0, 1);
static const Rect EXP_PLAYER = cell_rect(0, 2);

static const uint16_t SCORE_TABLE[4] {
    10,
    20,
    30,
    150
};

void reset_player_pos() {
    game.player.pos = { 160 - CELL_W/2, 210 };
}

void setup_level() {
    game.game_over = false;
    game.wave_cleared = false;
    game.lives = PLAYER_LIVES;

    reset_player_pos();
    game.player.alive = true;

    for (int y=0; y<5; y++) {
        uint8_t row_type;
        if (y == 0)      row_type = 2;
        else if (y <= 2) row_type = 1;
        else             row_type = 0;

        for (int x=0; x<11; x++) {
            auto &inv = game.invaders[y * 11 + x];
            inv.pos = { 20 + x * 24, 20 + y * 16 };
            inv.type = row_type;
            inv.col = x;
            inv.alive = true;
        }
    }
}

//INIT//
void init() {
    set_screen_mode(ScreenMode::hires);
    setup_level();
}

// RENDERING FUNCTIONS // 

void draw_player(uint32_t time) {
    if(game.player.exploding) {
        screen.blit(invaders_sheet, EXP_PLAYER, game.player.pos);
        if(time - game.player.explode_start > P_EXPLOSION_TIME) {
            game.player.exploding = false;
        }
    }
    else if(game.player.alive)
        screen.blit(invaders_sheet, SRC_PLAYER, game.player.pos);

}

void draw_invaders(int frame, uint32_t time) {
    for(auto &inv : game.invaders) {
        if (!inv.alive) continue;
    
        const Rect *src = nullptr;

        switch(inv.type) {
            case 0: src = (frame==0 ? &SRC_A0_F0 : &SRC_A0_F1); break;
            case 1: src = (frame==0 ? &SRC_A1_F0 : &SRC_A1_F1); break;
            case 2: src = (frame==0 ? &SRC_A2_F0 : &SRC_A2_F1); break;
        }

        if (inv.exploding) {
            src = &EXP_ALIEN;
            if (time - inv.explode_start > EXPLOSION_TIME) {
                inv.exploding = false;
                inv.alive = false;
            }
        }

    
        screen.blit(invaders_sheet, *src, inv.pos);
    }
}

void draw_score() {
    screen.pen = Pen(255, 255, 255);
    screen.text("SCORE", space_font, Point(5,4), false, TextAlign::top_left);
    char buf[12];
    snprintf(buf, sizeof(buf), "%04lu", static_cast<unsigned long>(game.score));

    int text_height = space_font.char_h;
    Point num_pos{5, 4 + text_height + 2};

    screen.text(buf, space_font, num_pos, false, TextAlign::top_left);
}

void draw_enemy_bullets(uint32_t time) {
    int phase = (time / 200) & 1;
    screen.pen = Pen(255, 255, 255);
    for(auto &eb : game.enemy_bullets) {
        if(!eb.active) continue;
        int x = eb.pos.x, y = eb.pos.y;
        
        screen.pixel(Point(x, y));
        screen.pixel(Point(x + (phase ? 1 : -1), y + 2));
        screen.pixel(Point(x, y + 4));
    }
}

void draw_lives() {
    screen.pen = Pen(255,255,255);
    int x0 = (screen.bounds.w / 2), y0 = 4;

    screen.text("LIVES", space_font, Point(x0, y0), false, TextAlign::top_left);

    Point p{ x0, y0 + space_font.char_h + 2 };
    for (int i = 0; i < game.lives; i++) {
        screen.blit(invaders_sheet, SRC_PLAYER, p);
        p.x += CELL_W + 4;
    }
}

void draw_game_over() {
    std::string msg;

    if (game.wave_cleared)
        msg = "WAVE " + std::to_string(game.wave);
    else
        msg = "GAME OVER";
    screen.pen = Pen(255,255,255);

    int text_h = space_font.char_h;
    int y = (screen.bounds.h - text_h) / 2;
    Rect r{ 0, y, screen.bounds.w, text_h };
    screen.text(msg, space_font, r, false, TextAlign::center_h);
}

//RENDER//
void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    if(game.game_over || game.wave_cleared) {
        draw_game_over();
        draw_score();
        return;
    }

    draw_player(time);
    
    //Invaders
    int frame = (time / 300) % 2;
    draw_invaders(frame, time);

    //PLAYER Bullets
    screen.pen = Pen(255, 255, 255);
    for(auto &b : game.bullets)
        if(b.active)
            screen.rectangle(Rect(b.pos.x, b.pos.y, 2, 4));

    draw_enemy_bullets(time);
    
    //Score
    draw_score();
    draw_lives();
    
}

// GAME LOGIC FUNCTIONS //

bool all_invaders_destroyed() {
    for(const auto &inv : game.invaders) {
        if(inv.alive) return false;
    }
    return true;
}

//checks the gives invader, adds a buffer to surpass a little
bool invaders_reached_player(const Invader &inv, int buffer = 15) {
    return inv.alive && (inv.pos.y + CELL_H >= game.player.pos.y + buffer);
}

void finish_current_wave() {
    game.wave_cleared = true;
    game.wave++;
}


void handle_player() {
    if(game.player.exploding) return;
    //player movement
    if (buttons & Button::DPAD_LEFT)
        game.player.pos.x = std::clamp(game.player.pos.x - 2L, 0L, (long)screen.bounds.w - CELL_W); //left
    if (buttons & Button::DPAD_RIGHT)
        game.player.pos.x = std::clamp(game.player.pos.x + 2L, 0L, (long)screen.bounds.w - CELL_W); //right

    //fire
    if ((buttons.pressed & Button::A) && !game.bullets[0].active) {
        for (auto &b : game.bullets) {
            if (!b.active) {
                b.active = true;
                b.pos = game.player.pos + Point(5, -4);
                break;
            }
        }
    }
}

void bullets_physics(uint32_t time) {
    int player_bullet_dist = 4;
    int enemy_bullet_dist = 4;
    //Moving PLAYER bullets
    for(auto &b : game.bullets) {
        if(b.active) {
            b.pos.y -= player_bullet_dist;
            if(b.pos.y < 0) b.active = false;
        }
    }
    //ENEMY bullets
    if(DEBUG) return;
    static uint32_t last_enemy_shot = 0;
    constexpr int shoot_freq_ms = 100;
    if(time - last_enemy_shot > shoot_freq_ms) {
        last_enemy_shot = time;
        //find the lowest invader in each column (the ones that can shoot)
        Invader *lowest[11] = { nullptr };
    
        for (auto &inv : game.invaders) {
            if(!inv.alive) continue;
            int c = inv.col;
            lowest[c] = &inv;
        }
        
        int cols[11], cols_count = 0; 
        for(int c = 0; c < 11; c++)
            if(lowest[c]) cols[cols_count++] = c;
        
        if(cols_count > 0) { //pick random column
            int rnd = cols[ blit::random() % cols_count ];
            Invader &shooter = *lowest[rnd];
            
            for(auto &eb : game.enemy_bullets) {
                if(!eb.active) {
                    eb.active = true;
                    eb.pos = shooter.pos + Point(CELL_W/2, CELL_H);
                    break;
                }
            }
        }
        for (auto &eb : game.enemy_bullets) {
            if(!eb.active) continue;
            eb.pos.y += enemy_bullet_dist;
            if(eb.pos.y > screen.bounds.h)
                eb.active = false;
        }
    }
}

void handle_collisions(uint32_t time) {
    for(auto &b : game.bullets) {
        if(!b.active) continue;

        //collision: bullets at bullets
        bool bullet_destroyed = false;
        for(auto &eb : game.enemy_bullets) {
            if(!eb.active) continue;

            Rect b_rect{ b.pos, Size(2,4) };
            Rect eb_rect{ eb.pos, Size(2,4) };

            if(b_rect.intersects(eb_rect)) {
                b.active = false;
                eb.active = false;
                bullet_destroyed = true;
                break;
            }
        }

        if(bullet_destroyed) continue;

        //collision: bullets at aliens
        for(auto &inv : game.invaders) {
            if(inv.alive && Rect(inv.pos, Size(CELL_W, CELL_H)).contains(b.pos)) {
                b.active = false;
                //Still alive to catch inv on next draw render
                //alive = false after finishing last draw aliens loop
                //inv.alive = false;
                inv.exploding = true;
                inv.explode_start = time;
            
                if (inv.type < 3) {
                    game.score += SCORE_TABLE[inv.type];
                }
                break;
            }
        }
    }
    Rect player_rect{ game.player.pos, Size(16,8) };
    for(auto &eb : game.enemy_bullets) {
        if(!eb.active) continue;
        if(player_rect.contains(eb.pos)) {
            eb.active = false;
            game.lives--;
            game.player.exploding = true;
            game.player.explode_start = time;
            if(game.lives > 0) {
                game.player.alive = true;
            }
            else {
                game.game_over = true;
                game.player.alive = false;
            }
            break;
        }
    }
    //Wave completion check
    if(all_invaders_destroyed())
        finish_current_wave();
}

void reset_game() {
    game = Game{};
    setup_level();
    for(auto &b : game.bullets) b.active = false;
    for(auto &eb : game.enemy_bullets) eb.active = false;
}

void start_new_wave() {
    setup_level();
    for(auto &b : game.bullets) b.active = false;
    for(auto &eb : game.enemy_bullets) eb.active = false;
}

//UPDATE//
void update(uint32_t time) {
    if(game.game_over || game.wave_cleared) {
        if(buttons.pressed & Button::A) {
            game.game_over ? reset_game() : start_new_wave();
        }
        return;
    }
    handle_player();

    if(time - game.last_move_time > 500) {
    // check if any edge of the fleet would hit the screen edge
    bool at_edge = false;
    for(auto &inv : game.invaders)
      if(inv.alive && 
         (
          (inv.pos.x + 8 >= 320 && game.direction == 1) 
           ||
          (inv.pos.x <= 0   && game.direction == -1)
         )) {
        at_edge = true;
        break;
      }

    // shift fleet down + reverse direction if at edge
    //+ check end contition invaders reached player
    for(auto &inv : game.invaders) {
      if(inv.alive) {
        inv.pos.x += game.direction * 8;
        if(at_edge) inv.pos.y += 8;

        if(invaders_reached_player(inv)) {
            game.game_over = true;
        }
      }
    }
    if(at_edge) game.direction *= -1;
    game.last_move_time = time;
    }

    bullets_physics(time);

    handle_collisions(time);


}