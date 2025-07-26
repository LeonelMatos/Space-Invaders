#include "invaders.hpp"
#include "assets.hpp"

using namespace blit;

static  Surface *digits_surf = Surface::load(sprite_digits);
static const int DIGIT_W = digits_surf->bounds.w / 10;
static const int DIGIT_H = digits_surf->bounds.h;

static Rect digit_rect(int d) {
    return Rect{ d * DIGIT_W, 0, DIGIT_W, DIGIT_H};
}

static Surface *invaders_sheet = Surface::load(sprites_invaders);

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

void draw_number(uint32_t value, Point pos) {
    std::string s = std::to_string(value);
    for (char c : s) {
        int d = c - '0';
        screen.blit(digits_surf, digit_rect(d), pos);
        pos.x += DIGIT_W + 1;
    }
}

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
    screen.pen = Pen(255,255,255);
    screen.text("SCORE", minimal_font, Point(5,4));
    
    draw_number(game.score, Point(5+6*5, 4));

}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    screen.blit(invaders_sheet, SRC_PLAYER, game.player.pos);
    
    //Invaders
    int frame = (time / 300) % 2;
    draw_invaders(frame);

    //Bullets
    screen.pen = Pen(255, 255, 0);
    for(auto &b : game.bullets)
        if(b.active)
            screen.rectangle(Rect(b.pos.x, b.pos.y, 2, 4));

    //Score
    draw_score();
    
}

void update(uint32_t time) {

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
                b.pos = game.player.pos + Point(8, -4);
                break;
            }
        }
    }

    //move bullets
    for(auto &b : game.bullets) {
        if(b.active) {
            b.pos.y -= 4;
            if(b.pos.y < 0) b.active = false;
        }
    }

    if(time - game.last_move_time > 500) {
    // check if any edge of the fleet would hit the screen edge
    bool at_edge = false;
    for(auto &inv : game.invaders)
      if(inv.alive && 
         (inv.pos.x + 8 >= 320 && game.direction == 1 ||
          inv.pos.x <= 0   && game.direction == -1)) {
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



}