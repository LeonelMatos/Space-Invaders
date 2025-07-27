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
  uint8_t col; //0..10 column pos
};

struct Bullet {
  Point   pos;
  bool    active{ false };
};

struct EnemyBullet {
  Point pos;
  bool active = false;
};

struct Game {
    Player player;
    std::array<Invader, 55> invaders;
    std::array<Bullet, 4> bullets;
    std::array<EnemyBullet, 5> enemy_bullets;
    uint32_t last_move_time{ 0 };
    uint32_t score{ 0 };
    int direction{ 1 }; //1=right -1=left
    int lives{3};
    bool game_over{false};
} game;