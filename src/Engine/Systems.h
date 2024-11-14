#pragma once
#include "Scene.h"
#include <SDL2/SDL.h>
#include "Components.h"

class System {
protected:
  Scene* scene;

public:
  void setScene(Scene* s) { scene = s; };
};

class SetupSystem : public System {
public:
  virtual void run() = 0;
};

class UpdateSystem : public System {
public:
  virtual void run(float dt) = 0;
};

class EventSystem : public System {
public:
  virtual void run(SDL_Event e) = 0;
};

class RenderSystem : public System {
public:
  virtual void run(SDL_Renderer* renderer) = 0;
};



class RenderSystem {
public:
    static void render(entt::registry& registry, SDL_Renderer* renderer) {
        auto view = registry.view<Position, SpriteComponent>();
        for (auto entity : view) {
            auto& pos = view.get<Position>(entity);
            auto& sprite = view.get<SpriteComponent>(entity);

            SDL_Rect dstRect = { pos.x, pos.y, sprite.width, sprite.height };
            SDL_RenderCopy(renderer, sprite.texture, nullptr, &dstRect);
        }
    }
};