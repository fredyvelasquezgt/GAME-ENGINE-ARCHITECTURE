#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "Engine/Components.h"
#include "Engine/Systems.h"
#include <entt/entt.hpp>
#include <vector>
#include "Render.h"
#include <iostream>

struct ColliderComponent {
    int width;
    int height;
    bool isTriggered;
    bool isSolid;
};

struct BarCollider {
    int width;
    int height;
};

struct EnemyCollider
{
    bool isDestroyed;
};

class ColliderResetSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<ColliderComponent>();
        for (auto [e, col] : view.each()) {
            col.isTriggered = false;
        }
    }
};

class ColliderUpdateSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, ColliderComponent>();
        std::vector<entt::entity> entities(view.begin(), view.end());
        size_t entityCount = entities.size();

        for (size_t i = 0; i < entityCount; ++i) {
            auto e1 = entities[i];
            auto& pos1 = view.get<PositionComponent>(e1);
            auto& vel1 = view.get<VelocityComponent>(e1);
            auto& col1 = view.get<ColliderComponent>(e1);

            for (size_t j = i + 1; j < entityCount; ++j) {
                auto e2 = entities[j];
                auto& pos2 = view.get<PositionComponent>(e2);
                auto& vel2 = view.get<VelocityComponent>(e2);
                auto& col2 = view.get<ColliderComponent>(e2);

                if (checkCollision(pos1, col1, pos2, col2)) {
                    resolveCollision(vel1, vel2, col1, col2);
                    col1.isTriggered = true;
                    col2.isTriggered = true;
                }
            }
            pos1.x += vel1.x * dT;
            pos1.y += vel1.y * dT;
        }
    }

private:
    bool checkCollision(const PositionComponent& pos1, const ColliderComponent& col1,
                        const PositionComponent& pos2, const ColliderComponent& col2) {
        return (pos1.x < pos2.x + col2.width &&
                pos1.x + col1.width > pos2.x &&
                pos1.y < pos2.y + col2.height &&
                pos1.y + col1.height > pos2.y);
    }

    void resolveCollision(VelocityComponent& vel1, VelocityComponent& vel2,
                          const ColliderComponent& col1, const ColliderComponent& col2) {
        if (col1.isSolid && col2.isSolid) {
            std::swap(vel1.x, vel2.x);
            std::swap(vel1.y, vel2.y);
        } else if (col1.isSolid && !col2.isSolid) {
            vel1.x = -vel1.x;
            vel1.y = -vel1.y;
        } else if (!col1.isSolid && col2.isSolid) {
            vel2.x = -vel2.x;
            vel2.y = -vel2.y;
        }
    }
};

#endif // COLLIDERS_H

class EnemyCollisionSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, ColliderComponent>();
        auto enemyView = scene->r.view<PositionComponent, EnemyCollider, SpriteComponent>();

        std::vector<entt::entity> entities(view.begin(), view.end());
        size_t entityCount = entities.size();

        for (size_t i = 0; i < entityCount; ++i) {
            auto e1 = entities[i];
            auto& pos1 = view.get<PositionComponent>(e1);
            auto& vel1 = view.get<VelocityComponent>(e1);
            auto& col1 = view.get<ColliderComponent>(e1);

            // Check collision with enemies
            for (auto enemyEntity : enemyView) {
                auto& enemyPos = enemyView.get<PositionComponent>(enemyEntity);
                auto& enemyComp = enemyView.get<EnemyCollider>(enemyEntity);
                auto& enemySpr = enemyView.get<SpriteComponent>(enemyEntity);

                if (checkCollision(pos1, col1, enemyPos, enemyComp)) {

                    // Mark the enemy for destruction
                    enemyComp.isDestroyed = true;
                }
            }
        }

        // Destroy enemies marked for destruction
        auto entitiesToDestroy = scene->r.view<EnemyCollider>();
        for (auto e : entitiesToDestroy) {  
            auto& enemyComp = scene->r.get<EnemyCollider>(e);
            auto& enemySpr = scene->r.get<SpriteComponent>(e);
            if (enemyComp.isDestroyed) {
                enemySpr.width = 0;
                enemySpr.height = 0;
            }
        }
    }

private:
    bool checkCollision(const PositionComponent& pos1, const ColliderComponent& col1,
                        const PositionComponent& pos2, const EnemyCollider& enemyComp) {
        // Assuming EnemyCollider doesn’t have size, you might need to use a fixed size for collision detection
        int enemyWidth = 50; // Set an appropriate width for enemy
        int enemyHeight = 50; // Set an appropriate height for enemy
        
        return (pos1.x < pos2.x + enemyWidth &&
                pos1.x + col1.width > pos2.x &&
                pos1.y < pos2.y + enemyHeight &&
                pos1.y + col1.height > pos2.y);
    }
};

class PlayerColliderSystem : public UpdateSystem {
public:
    void run(float dT) {
        auto view = scene->r.view<PositionComponent, VelocityComponent, ColliderComponent>();
        auto barView = scene->r.view<PositionComponent, BarCollider>();
        
        std::vector<entt::entity> entities(view.begin(), view.end());
        size_t entityCount = entities.size();

        for (size_t i = 0; i < entityCount; ++i) {
            auto e1 = entities[i];
            auto& pos1 = view.get<PositionComponent>(e1);
            auto& vel1 = view.get<VelocityComponent>(e1);
            auto& col1 = view.get<ColliderComponent>(e1);

            for (auto e2 : barView) {
                auto& pos2 = barView.get<PositionComponent>(e2);
                auto& barCol2 = barView.get<BarCollider>(e2);

                if (checkCollision(pos1, col1, pos2, barCol2)) {

                    // Invert the velocity in the y direction for the object with the ColliderComponent
                    vel1.y = -vel1.y;
                    col1.isTriggered = true;
                }
            }
            pos1.x += vel1.x * dT;
            pos1.y += vel1.y * dT;
        }
    }

private:
    bool checkCollision(const PositionComponent& pos1, const ColliderComponent& col1,
                        const PositionComponent& pos2, const BarCollider& barCol2) {
        return (pos1.x < pos2.x + barCol2.width &&
                pos1.x + col1.width > pos2.x &&
                pos1.y < pos2.y + barCol2.height &&
                pos1.y + col1.height > pos2.y);
    }
};
