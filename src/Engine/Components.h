#pragma once
#include <string>
#include <SDL2/SDL.h>


struct NameComponent {
  const std::string name;
};

struct PositionComponent {
  int x;
  int y;
};

struct VelocityComponent {
  int x;
  int y;
};


struct Position {
    int x;
    int y;
};

struct SpriteComponent {
    SDL_Texture* texture;
    int width;
    int height;
};
