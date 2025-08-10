#include "32blit.hpp"
#include <array>
#include <cstdio>

using namespace blit;

constexpr bool DEBUG = false;

constexpr int PLAYER_LIVES = 3;
constexpr int INVADERS_NUM = 55;

constexpr uint32_t EXPLOSION_TIME =   250; //ms
constexpr uint32_t P_EXPLOSION_TIME = 1500;

struct Player {
  Point     pos;
  bool      alive{ true };
  bool      exploding{ false };
  uint32_t  explode_start{ 0 };
};

struct Invader {
  Point     pos;
  bool      alive{ true };
  uint8_t   type; //0,1,2
  uint8_t   col; //0..10 column pos
  bool      exploding{ false };
  uint32_t  explode_start{ 0 };

};

struct Bullet {
  Point   pos;
  bool    active{ false };
};

struct EnemyBullet {
  Point pos;
  bool active = false;
};

struct Barrier {
  static const int width = 5;
  static const int height = 3;

  blit::Vec2 position;
  bool chunks[height][width];
};

struct Game {
    Player player;
    std::array<Invader, INVADERS_NUM> invaders;
    std::array<Bullet, 4> bullets;
    std::array<EnemyBullet, 5> enemy_bullets;
    uint32_t last_move_time{ 0 };
    uint32_t score{ 0 };
    int direction{ 1 }; //1=right -1=left
    int lives{PLAYER_LIVES};
    bool game_over{false};
    int wave{ 1 }; //current wave
    bool wave_cleared{ false };
} game;