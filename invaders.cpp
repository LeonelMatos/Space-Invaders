#include "invaders.hpp"
#include "assets.hpp"

using namespace blit;

static Surface *invaders_sheet = Surface::load(sprites_invaders);

static const Rect SRC_SHIP   { 0,   0, 11, 8 };
static const Rect SRC_ALIEN { 11,  0, 11, 8 };


void init() {
    set_screen_mode(ScreenMode::hires);

    game.player.pos = { 160 - 8, 210 };

    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 11; x++)
            game.invaders[y * 11 + x].pos = { 20 + x *24, 20 + y * 16 };
}

void render(uint32_t time) {
    screen.pen = Pen(0, 0, 0);
    screen.clear();


    screen.blit(invaders_sheet, SRC_SHIP, game.player.pos);

    for(auto &inv : game.invaders) {
        if (!inv.alive) continue;
        
        screen.blit(invaders_sheet, SRC_ALIEN, inv.pos);
    }

    screen.pen = Pen(255, 255, 0);
    for(auto &b : game.bullets)
        if(b.active)
            screen.rectangle(Rect(b.pos.x, b.pos.y, 2, 4));
    
}

void update(uint32_t time) {

    //player movement
    if (buttons & Button::DPAD_LEFT)
        game.player.pos.x = std::max(0, game.player.pos.x -2);
    if (buttons & Button::DPAD_RIGHT)
        game.player.pos.x = std::min(320-16, game.player.pos.x + 2);

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
                break;
            }
        }
    }



}