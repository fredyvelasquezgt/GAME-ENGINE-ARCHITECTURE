#ifndef PLAYER_H
#define PLAYER_H

#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Render.h"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

struct PlayerComponent {
    bool isPlayer;
    int speedx;
    int speedy;
};

class PlayerWallHitSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, PlayerComponent, SpriteComponent>();
        for (auto e : view) {
            auto pos = view.get<PositionComponent>(e);
            auto player = view.get<PlayerComponent>(e);
            auto spr = view.get<SpriteComponent>(e);
            auto& vel = view.get<VelocityComponent>(e);
            int newPosX = pos.x + vel.x * dT;
            int newPosY = pos.y + vel.y * dT;
            if (newPosX < 0 || newPosX + spr.width > 1024) {
                vel.x = 0;
            }
            if (newPosY < 0 || newPosY + spr.height > 768) {
                vel.y = 0;
            }
        }
    }
};

class PlayerMovementSystem : public EventSystem {
public:
    void run(SDL_Event event) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, PlayerComponent>();
        for (auto e : view) {
            auto& vel = view.get<VelocityComponent>(e);
            auto player = view.get<PlayerComponent>(e);

            if (event.type == SDL_KEYDOWN) {
                handleKeyDown(event, vel, player);
            } else if (event.type == SDL_KEYUP) {
                vel.x = 0;
                vel.y = 0;
            }
        }
    }

private:
    void handleKeyDown(SDL_Event event, VelocityComponent& vel, PlayerComponent& player) {
        switch (event.key.keysym.sym) {
            case SDLK_a:
                vel.x = -player.speedx;
                break;
            case SDLK_d:
                vel.x = player.speedx;
                break;
            // case SDLK_w:
            //     vel.y = -player.speedy;
            //     break;
            // case SDLK_s:
            //     vel.y = player.speedy;
            //     break;
        }
    }
};

#endif // PLAYER_H
