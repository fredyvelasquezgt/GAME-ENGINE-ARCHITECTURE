#pragma once
#include <entt/entt.hpp>
#include <iostream> //print
#include <utility>
#include "Scene.h"
#include "Components.h"

class Entity {
public:
    static entt::entity createHeart(entt::registry& registry, SDL_Texture* texture, int x, int y) {
        auto entity = registry.create();
        registry.emplace<Position>(entity, x, y);
        registry.emplace<SpriteComponent>(entity, texture, 32, 32); // Ajusta el tamaño según tus sprites
        return entity;
    }

    static entt::entity createBomb(entt::registry& registry, SDL_Texture* texture, int x, int y) {
        auto entity = registry.create();
        registry.emplace<Position>(entity, x, y);
        registry.emplace<SpriteComponent>(entity, texture, 32, 32);
        return entity;
    }
};