#include "Game/PlayerController.hpp"
#include "Game/Character.hpp"

#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Animation/Animator.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "App.hpp"
#include "Game.hpp"
#include "GameCommon.hpp"

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
    if ( g_app->m_game->m_cameraMode == CameraMode::TopDown )
    {
        ImGuiIO& io = ImGui::GetIO();
        if ( !io.WantCaptureMouse )
        {
            UpdateFromMouse();
        }

        Character* character       = dynamic_cast< Character* >( m_possessedActor );
        float      distanceSquared = GetDistanceSquared3D( character->m_position, m_mouseTargetPos );
        if ( distanceSquared <= 0.1f )
        {
            character->m_velocity = Vec3::ZERO;
        }

        UpdateFromKeyboard();
        UpdatePlayerCamera();
    }
    else
    {
        UpdateFreeFlyCamera();
    }
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateFromKeyboard()
{
    float      rotateSpeed  = 720.f;
    float      deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );
    Character* character    = dynamic_cast< Character* >( m_possessedActor );

    m_possessedActor->GetComponentByClass< SkeletalMeshComponent >()->m_animator->SetFloat( "Speed", character->m_velocity.GetLengthSquared() );

    if ( character->m_velocity.GetLengthSquared() > 0.f )
    {
        m_possessedActor->m_orientation.m_yawDegrees = GetTurnedTowardDegrees( m_possessedActor->m_orientation.m_yawDegrees, character->m_velocity.GetOrientationAboutZDegrees(), rotateSpeed * deltaSeconds );
    }
    else
    {
        Vec3 direction                               = ( m_mousePos - character->m_position ).GetNormalized();
        m_possessedActor->m_orientation.m_yawDegrees = GetTurnedTowardDegrees( m_possessedActor->m_orientation.m_yawDegrees, direction.GetOrientationAboutZDegrees(), rotateSpeed * deltaSeconds );
    }

    if ( g_engine->m_input->WasKeyJustPressed( '1' ) )
    {
        GameplayTag const& abilityTag = GameplayTagManager().Get().RequestTag( "Ability.Melee.ComboAttack" );
        m_possessedActor->GetComponentByClass< AbilitySystemComponent >()->TryActivateAbility( abilityTag );
    }

    if ( g_engine->m_input->WasKeyJustPressed( '2' ) )
    {
        GameplayTag const& abilityTag = GameplayTagManager().Get().RequestTag( "Ability.Melee.SwordAndShieldAttack" );
        m_possessedActor->GetComponentByClass< AbilitySystemComponent >()->TryActivateAbility( abilityTag );
    }

    if ( g_engine->m_input->WasKeyJustPressed( '3' ) )
    {
        GameplayTag const& abilityTag = GameplayTagManager().Get().RequestTag( "Ability.Magic.Cast" );
        m_possessedActor->GetComponentByClass< AbilitySystemComponent >()->TryActivateAbility( abilityTag );
    }
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateFromMouse()
{
    GetMouseGroundPosition( m_mousePos );
    float speed = 5.0f;
    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_RIGHT_MOUSE ) )
    {
        Character* character = dynamic_cast< Character* >( m_possessedActor );
        m_mouseTargetPos     = m_mousePos;
        Vec3 direction       = ( m_mouseTargetPos - character->m_position ).GetNormalized();

        character->m_velocity = direction * speed;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_LEFT_MOUSE ) )
    {
        GameplayTag const& abilityTag = GameplayTagManager().Get().RequestTag( "Ability.Melee.Basic" );
        m_possessedActor->GetComponentByClass< AbilitySystemComponent >()->TryActivateAbility( abilityTag );
    }
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdatePlayerCamera()
{
    Vec3        targetPos = m_possessedActor->m_position + Vec3( 0.f, 0.f, 2.0f );

    EulerAngles cameraOrientation( 0.f, 45.f, 0.f );
    Mat44       cameraMatrix  = cameraOrientation.GetAsMatrix_IFwd_JLeft_KUp();
    Vec3        cameraForward = cameraMatrix.GetIBasis3D();

    float       cameraDistance = 10.f;
    Vec3        cameraPos      = targetPos - cameraForward * cameraDistance;

    m_worldCamera->SetPosition( cameraPos );
    m_worldCamera->SetOrientation( cameraOrientation );
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Render() const
{
    std::vector< Vertex > verts;
    AddVertsForAABB3D( verts, AABB3( Vec3( -0.5f, -0.5f, -0.5f ), Vec3( 0.5f, 0.5f, 0.5f ) ), Rgba8::WHITE );

    Mat44 m;
    m.AppendTranslation3D( m_mousePos );
    m.AppendScaleUniform3D( 0.1f );

    g_engine->m_render->SetModelConstants( m );

    g_engine->m_render->BindTextureWithSampler( { g_defaultWhiteTexture, SamplerMode::POINT_CLAMP } );
    g_engine->m_render->DrawVertexArray( verts );
}

//-----------------------------------------------------------------------------------------------
void PlayerController::Possess( Actor* character )
{
    m_possessedActor = character;
    dynamic_cast< Character* >( m_possessedActor )->PossessedBy( this );
}

//-----------------------------------------------------------------------------------------------
void PlayerController::UpdateFreeFlyCamera()
{
    float speed = 2.0f;
    // float rotateSpeed      = 360.f;
    Vec2  cursorDelta      = g_engine->m_input->GetCursorClientDelta();
    float mouseSensitivity = 0.125f;

    Vec3  direction;
    float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );

    Mat44 orientationMatrix = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
    Vec3  forwardVector     = orientationMatrix.GetIBasis3D();
    Vec3  leftVector        = orientationMatrix.GetJBasis3D();

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

    if ( g_engine->m_input->IsKeyDown( KEYCODE_SHIFT ) )
    {
        speed *= 10.f;
    }

    if ( direction.GetLengthSquared() > 0.f )
    {
        direction = direction.GetNormalized();
    }

    m_velocity = direction * speed;
    m_position += m_velocity * deltaSeconds;

    float newYaw   = m_orientation.m_yawDegrees - cursorDelta.x * mouseSensitivity;
    float newPitch = GetClamped( m_orientation.m_pitchDegrees + cursorDelta.y * mouseSensitivity, -85.f, 85.f );
    float newRoll  = GetClamped( m_orientation.m_rollDegrees, -45.f, 45.f );
    m_orientation  = EulerAngles( newYaw, newPitch, newRoll );

    m_worldCamera->SetPosition( m_position );
    m_worldCamera->SetOrientation( m_orientation );
}

//-----------------------------------------------------------------------------------------------
Actor* PlayerController::GetPossessedActor() const
{
    return m_possessedActor;
}

//-----------------------------------------------------------------------------------------------
bool PlayerController::GetMouseGroundPosition( Vec3& outGroundPosition ) const
{
    Vec2    uv = g_engine->m_window->GetNormalizedMouseUV();

    float   ndcX = uv.x * 2.f - 1.f;
    float   ndcY = uv.y * 2.f - 1.f;

    IntVec2 clientSize = g_engine->m_window->GetClientDimensions();
    float   aspect     = static_cast< float >( clientSize.x ) / static_cast< float >( clientSize.y );

    float   tanHalfFov = tanf( ConvertDegreesToRadians( 60.f * 0.5f ) );

    Mat44   camToWorld = m_worldCamera->GetCameraToWorldTransform();

    Vec3    forward = camToWorld.GetIBasis3D();
    Vec3    left    = camToWorld.GetJBasis3D();
    Vec3    up      = camToWorld.GetKBasis3D();

    Vec3    rayStart = camToWorld.GetTranslation3D();
    Vec3    rayDir   = ( forward + left * ( -ndcX * tanHalfFov * aspect ) + up * ( ndcY * tanHalfFov ) ).GetNormalized();

    if ( rayDir.z >= -0.001f )
    {
        return false;
    }

    float t = ( 0.f - rayStart.z ) / rayDir.z;
    if ( t < 0.f )
    {
        return false;
    }

    outGroundPosition = rayStart + rayDir * t;
    return true;
}