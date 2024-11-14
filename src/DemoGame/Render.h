#pragma once
#include <SDL2/SDL.h>
#include <string>


struct SpriteComponent {
    int width;
    int height;
    SDL_Color color;
};


class TextureLoader {
public:
    static SDL_Texture* loadTexture(const std::string& path, SDL_Renderer* renderer);
};

