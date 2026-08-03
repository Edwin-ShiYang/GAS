#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Prop.hpp"

//-----------------------------------------------------------------------------------------------
class Camera;
class Character;
class StaticModel;

//-----------------------------------------------------------------------------------------------
class PlayerController
{
public:
    PlayerController();
    ~PlayerController();

    void   Update();
    void   UpdateFromKeyboard( Vec3& direction, float rotateSpeed, float deltaSeconds );
    void   UpdatePlayerCamera();
    void   Render() const;
    void   Possess( Actor* actor );
    Actor* GetPossessedActor() const;

public:
    Camera*               m_worldCamera;
    std::string           m_name;
    Vec3                  m_position;
    EulerAngles           m_orientation;
    Vec3                  m_scale = Vec3( 1.0f, 1.0f, 1.0f );
    Vec3                  m_velocity;
    StaticModel*          m_model    = nullptr;
    PropDefinition const* m_actorDef = nullptr;

private:
    Actor* m_possessedActor   = nullptr;
    float  m_currentMoveSpeed = 2.0f;
};