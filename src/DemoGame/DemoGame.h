#ifndef GAME_H
#define GAME_H

#include "Engine/Game.h"
#include "Engine/Scene.h"
#include "Engine/Entity.h"
#include "Engine/Components.h"
#include "Engine/Systems.h"
#include "Colliders.h"
#include "Player.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "Render.h"
#include <entt/entt.hpp>



const int WIDTH = 1024;
const int HEIGHT = 768;

class MovementSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent>();
        for (auto e : view) {
            auto& pos = view.get<PositionComponent>(e);
            auto vel = view.get<VelocityComponent>(e);
            pos.x += (vel.x * dT);
            pos.y += (vel.y * dT);
        }
    }
};

class WallHitSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();
        for (auto e : view) {
            auto pos = view.get<PositionComponent>(e);
            auto spr = view.get<SpriteComponent>(e);
            auto& vel = view.get<VelocityComponent>(e);
            int newPosX = pos.x + vel.x * dT;
            int newPosY = pos.y + vel.y * dT;
            if (newPosX < 0 || newPosX + spr.width > 1024) {
                vel.x *= -1;
            }
            if (newPosY < 0 || newPosY + spr.height > 768) {
                vel.y *= -1;
            }
        }
    }
};

class LoseSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, SpriteComponent>();
        for (auto e : view) {
            auto pos = view.get<PositionComponent>(e);
            auto spr = view.get<SpriteComponent>(e);
            auto& vel = view.get<VelocityComponent>(e);
            int newPosX = pos.x + vel.x * dT;
            int newPosY = pos.y + vel.y * dT;
            if (newPosY + spr.height > 768) {
                std::cout << "YOU LOST" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }
};

class EnemyCheckSystem : public UpdateSystem {
public:
    void run(float dT) override {
        auto view = scene->r.view<EnemyCollider>();

        bool allEnemiesEliminated = true;
        for (auto e : view) {
            auto& enemy = view.get<EnemyCollider>(e);
            if (!enemy.isDestroyed) {
                allEnemiesEliminated = false;
                break;
            }
        }

        if (allEnemiesEliminated) {
            std::cout << "YOU WIN" << std::endl;
            std::exit(EXIT_SUCCESS); // Terminar el programa con un código de éxito
        }
    }
};







class DemoGame : public Game {
public:
    Scene* sampleScene;
    entt::registry r;

public:
    DemoGame()
        : Game("SAMPLE", WIDTH, HEIGHT) { }

    void setup() {
        sampleScene = new Scene("", r);
        addUpdateSystem<ColliderResetSystem>(sampleScene);
        addUpdateSystem<PlayerColliderSystem>(sampleScene);
        addUpdateSystem<LoseSystem>(sampleScene);
        addUpdateSystem<PlayerWallHitSystem>(sampleScene);
        addUpdateSystem<EnemyCollisionSystem>(sampleScene);
        addUpdateSystem<WallHitSystem>(sampleScene);
        addEventSystem<PlayerMovementSystem>(sampleScene);
        addUpdateSystem<MovementSystem>(sampleScene);
        setScene(sampleScene);
    }
};

#endif // GAME_H
