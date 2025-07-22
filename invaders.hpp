#include "32blit.hpp"
#include <array>

using namespace blit;

struct Player {
  Point   pos;
  bool    alive{ true };
};

struct Invader {
  Point   pos;
  bool    alive{ true };
  uint8_t type; //0,1,2
};

struct Bullet {
  Point   pos;
  bool    active{ false };
};

struct Game {
    Player player;
    std::array<Invader, 55> invaders;
    std::array<Bullet, 4> bullets;
    uint32_t last_move_time{ 0 };
    int direction{ 1 }; //1=right -1=left
} game;