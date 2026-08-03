#include "Game/PlayerController.hpp"
#include "Game/Game.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Character.hpp"

//-----------------------------------------------------------------------------------------------
PlayerController::PlayerController()

{
    m_worldCamera = new Camera();
    m_worldCamera->SetCameraToRenderTransform( Mat44::MakeCameraToRenderTransform() );
}

PlayerController::~PlayerController()
{
    delete m_worldCamera;
    m_worldCamera = nullptr;
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Update()
{
    float speed        = 2.0f;
    float rotateSpeed  = 90.f;
    Vec3  direction    = Vec3::ZERO;
    float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );

    if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
    {
        speed *= 10.f;
    }

    if ( g_engine->m_input->IsKeyDown( 'Z' ) )
    {
        direction -= Vec3::WORLD_UP;
    }

    if ( g_engine->m_input->IsKeyDown( 'C' ) )
    {
        direction += Vec3::WORLD_UP;
    }

    if ( g_engine->m_input->IsKeyDown( 'H' ) )
    {
        m_position    = Vec3::ZERO;
        m_orientation = EulerAngles();
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
    {
        Character* character = dynamic_cast< Character* >( m_possessedActor );
        character->GetAbilitySystemComponent()->TryActivateAbility( "BasicAttack" );
    }

    UpdateFromKeyboard( direction, rotateSpeed, deltaSeconds );

    if ( direction.GetLengthSquared() > 1.0f )
    {
        direction = direction.GetNormalized();
    }

    m_velocity = direction * speed;
    m_position += m_velocity * deltaSeconds;

    UpdatePlayerCamera();
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateFromKeyboard( Vec3& direction, float rotateSpeed, float deltaSeconds )
{
    Vec2  cursorDelta      = g_engine->m_input->GetCursorClientDelta();
    float mouseSensitivity = 0.125f;

    if ( g_engine->m_input->IsKeyDown( 'Q' ) )
    {
        m_orientation.m_rollDegrees -= deltaSeconds * rotateSpeed;
    }

    if ( g_engine->m_input->IsKeyDown( 'E' ) )
    {
        m_orientation.m_rollDegrees += deltaSeconds * rotateSpeed;
    }

    float newYaw   = m_orientation.m_yawDegrees - cursorDelta.x * mouseSensitivity;
    float newPitch = GetClamped( m_orientation.m_pitchDegrees + cursorDelta.y * mouseSensitivity, -85.f, 85.f );

    float newRoll = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );

    m_orientation = EulerAngles( newYaw, newPitch, newRoll );

    Mat44 orientationMatrix = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();

    Vec3  forwardVector = orientationMatrix.GetIBasis3D();
    Vec3  leftVector    = orientationMatrix.GetJBasis3D();

    if ( g_engine->m_input->IsKeyDown( 'S' ) )
    {
        direction -= forwardVector;
    }

    if ( g_engine->m_input->IsKeyDown( 'W' ) )
    {
        direction += forwardVector;
    }

    if ( g_engine->m_input->IsKeyDown( 'A' ) )
    {
        direction += leftVector;
    }

    if ( g_engine->m_input->IsKeyDown( 'D' ) )
    {
        direction -= leftVector;
    }
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdatePlayerCamera()
{
    m_worldCamera->SetPosition( m_position );
    m_worldCamera->SetOrientation( m_orientation );
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Render() const
{
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Possess( Actor* actor )
{
    m_possessedActor = actor;
}

//-----------------------------------------------------------------------------------------------
Actor* PlayerController::GetPossessedActor() const
{
    return m_possessedActor;
}