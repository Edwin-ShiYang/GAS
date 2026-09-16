#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Controller.hpp"

//-----------------------------------------------------------------------------------------------
class Camera;
class Character;
class MovementComponent;

//-----------------------------------------------------------------------------------------------
class PlayerController : Controller
{
public:
    PlayerController();
    ~PlayerController();

    void   Update();
    void   UpdateFromKeyboard();
    void   UpdateFromMouse();
    void   UpdatePlayerCamera();
    void   Render() const;
    void   Possess( Actor* character );
    void   UpdateFreeFlyCamera();

    Actor* GetPossessedActor() const;
    bool   GetMouseGroundPosition( Vec3& outGroundPosition ) const;

public:
    Camera*     m_worldCamera;
    std::string m_name;

    EulerAngles m_orientation;

    Vec3        m_position;
    Vec3        m_velocity;

    Vec3        m_scale = Vec3( 1.0f, 1.0f, 1.0f );

    Vec3        m_mousePos;

private:
    Actor*             m_possessedActor    = nullptr;
    float              m_currentMoveSpeed  = 2.0f;
    Vec3               m_mouseTargetPos    = Vec3::ZERO;
    MovementComponent* m_movementComponent = nullptr;
};