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

static const uint16_t SCORE_TABLE[4] {
    10,
    20,
    30,
    150
};

//INIT//
void init() {
    set_screen_mode(ScreenMode::hires);

    game.player.pos = { 160 - CELL_W/2, 210 };

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
        }
    }
}

void draw_invaders(int frame) {
    for(auto &inv : game.invaders) {
        if (!inv.alive) continue;
    
        const Rect *src = nullptr;

        switch(inv.type) {
            case 0: src = (frame==0 ? &SRC_A0_F0 : &SRC_A0_F1); break;
            case 1: src = (frame==0 ? &SRC_A1_F0 : &SRC_A1_F1); break;
            case 2: src = (frame==0 ? &SRC_A2_F0 : &SRC_A2_F1); break;
        }
    
        screen.blit(invaders_sheet, *src, inv.pos);
    }
}

void draw_score() {
    screen.pen = Pen(255, 255, 255);
    screen.text("SCORE", space_font, Point(5,4), false, TextAlign::top_left);
    char buf[5];
    snprintf(buf, sizeof(buf), "%04u", game.score);

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

//RENDER//
void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    if(game.player.alive)
        screen.blit(invaders_sheet, SRC_PLAYER, game.player.pos);
    
    //Invaders
    int frame = (time / 300) % 2;
    draw_invaders(frame);

    //PLAYER Bullets
    screen.pen = Pen(255, 255, 255);
    for(auto &b : game.bullets)
        if(b.active)
            screen.rectangle(Rect(b.pos.x, b.pos.y, 2, 4));

    draw_enemy_bullets(time);
    
    //Score
    draw_score();
    
}

void handle_player() {
    //player movement
    if (buttons & Button::DPAD_LEFT)
        game.player.pos.x = std::clamp(game.player.pos.x - 2L, 0L, 304L); //left
    if (buttons & Button::DPAD_RIGHT)
        game.player.pos.x = std::clamp(game.player.pos.x + 2L, 0L, 304L); //right

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

void handle_collisions() {
    //collision: bullets at aliens
    for(auto &b : game.bullets) {
        if(!b.active) continue;
        for(auto &inv : game.invaders) {
            if(inv.alive && Rect(inv.pos, Size(16,8)).contains(b.pos)) {
                inv.alive = false;
                b.active = false;

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
            game.player.alive = false;
        }
    }
}

//UPDATE//
void update(uint32_t time) {

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
    for(auto &inv : game.invaders) {
      if(inv.alive) {
        inv.pos.x += game.direction * 8;
        if(at_edge) inv.pos.y += 8;
      }
    }

    if(at_edge) game.direction *= -1;
    game.last_move_time = time;
    }

    bullets_physics(time);

    handle_collisions();


}