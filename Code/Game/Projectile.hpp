#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/ProjectileDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "Engine/Core/Timer.hpp"

//-----------------------------------------------------------------------------------------------
class MovementComponent;
class SpriteSheet;
class SpriteAnimDefinition;
class Game;
class StaticMeshDefinition;

//-----------------------------------------------------------------------------------------------
class Projectile : public Actor
{
public:
    Projectile( ProjectileDefinition const& projectileDef, Game* game );
    ~Projectile();

    void                        Update() override;
    void                        Render() const override;
    Mat44                       GetModelToWorldTransform() const;

    MovementComponent*          m_movementComponent    = nullptr;
    SpriteSheet*                m_spriteSheet          = nullptr;
    SpriteAnimDefinition*       m_spriteAnimDefinition = nullptr;
    Game*                       m_game                 = nullptr;

    StaticMeshDefinition const* m_staticMeshDef = nullptr;
    ProjectileDefinition const& m_projectileDef;
    Mat44                       m_toEngineMatrix;

    Timer                       m_deathTimer;
};