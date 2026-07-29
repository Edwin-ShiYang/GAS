#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Actor.hpp"

//-----------------------------------------------------------------------------------------------
class Camera;
class Character;

//-----------------------------------------------------------------------------------------------
class PlayerController
{
public:
    PlayerController();
    ~PlayerController();

    void       Update();
    void       UpdateFromKeyboard( Vec3& direction, float rotateSpeed, float deltaSeconds );
    void       UpdateFromController( Vec3& direction, float rotateSpeed, float deltaSeconds );
    void       UpdatePlayerCamera();
    void       Render() const;
    void       Possess( Character* actor );
    Character* GetPossessedActor() const;

public:
    Camera*                m_worldCamera;

    std::string            m_name;
    Vec3                   m_position;
    EulerAngles            m_orientation;
    Vec3                   m_scale = Vec3( 1.0f, 1.0f, 1.0f );
    Vec3                   m_velocity;
    StaticModel*           m_model    = nullptr;
    ActorDefinition const* m_actorDef = nullptr;
    Rgba8                  m_color    = Rgba8::WHITE;

private:
    Character* m_possessedActor = nullptr;
};