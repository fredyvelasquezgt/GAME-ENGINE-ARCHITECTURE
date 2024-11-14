// Render.cpp
#include "Render.h"
#include <SDL2/SDL_image.h>

SDL_Texture* TextureLoader::loadTexture(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}