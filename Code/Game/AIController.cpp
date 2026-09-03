#include "Game/AIController.hpp"
#include "Game/Character.hpp"
#include "Game/CharacterDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Animation/Animator.hpp"
#include <Engine/Core/Engine.hpp>
#include <Engine/Core/VertexUtils.hpp>

//-----------------------------------------------------------------------------------------------
AIController::AIController( Game* game, ActorHandle const& actorHandle )
    : Controller( game, actorHandle )
{
}

void AIController::ChasePlayer( Character& character )
{
    if ( !m_game->m_enableAI )
    {
        character.m_velocity = Vec3::ZERO;
        return;
    }

    float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );
    if ( m_elapsedSeconds > 0.f )
    {
        m_elapsedSeconds += deltaSeconds;
        if ( m_elapsedSeconds >= m_duration )
        {
            m_elapsedSeconds = 0.f;
        }
    }

    Vec2  targetPositionXY        = Vec2( m_game->m_playerCharacer->m_position.x, m_game->m_playerCharacer->m_position.y );
    Vec2  positionXY              = Vec2( character.m_position.x, character.m_position.y );
    float distanceToTargetSquared = GetDistanceSquared2D( positionXY, targetPositionXY );

    if ( IsPointInsideDisc2D( targetPositionXY, positionXY, character.m_characterDef.m_sightRadius ) )
    {
        m_targetActor = m_game->m_playerCharacer;
    }

    if ( m_targetActor )
    {
        Vec3 direction       = ( m_game->m_playerCharacer->m_position - character.m_position ).GetNormalized();
        character.m_velocity = direction * character.m_characterDef.m_runSpeed;
    }

    if ( m_targetActor && distanceToTargetSquared <= character.m_characterDef.m_attackRange * character.m_characterDef.m_attackRange )
    {
        character.m_velocity = Vec3::ZERO;
        if ( m_elapsedSeconds <= 0.f )
        {
            GameplayTag const&      abilityTag = GameplayTagManager::Get().RequestTag( "Ability.Melee.Basic" );
            AbilitySystemComponent* asc        = character.GetComponentByClass< AbilitySystemComponent >();
            if ( asc && asc->TryActivateAbility( abilityTag ) )
            {
                m_elapsedSeconds = deltaSeconds;
            }
        }
    }

    GetActor()->GetComponentByClass< SkeletalMeshComponent >()->m_animator->SetFloat( "Speed", character.m_velocity.GetLengthSquared() );
}

//-----------------------------------------------------------------------------------------------
void AIController::TurnTowardDirection( Character* character )
{
    float deltaSeconds     = static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );
    Vec2  positionXY       = Vec2( character->m_position.x, character->m_position.y );
    Vec2  targetPositionXY = Vec2( m_game->m_playerCharacer->m_position.x, m_game->m_playerCharacer->m_position.y );

    float turnSpeed       = character->m_characterDef.m_turnSpeed;
    float goalOrientation = ( targetPositionXY - positionXY ).GetOrientationDegrees();

    character->m_orientation.m_yawDegrees = GetTurnedTowardDegrees( character->m_orientation.m_yawDegrees, goalOrientation, turnSpeed * deltaSeconds );
}

//-----------------------------------------------------------------------------------------------
void AIController::Update()
{
    Character* character = dynamic_cast< Character* >( GetActor() );
    if ( !character )
    {
        return;
    }

    ChasePlayer( *character );
    TurnTowardDirection( character );
}

//-----------------------------------------------------------------------------------------------
void AIController::RenderDebug() const
{
    Character* character = dynamic_cast< Character* >( GetActor() );

    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );

    Mat44                 discTransform;
    std::vector< Vertex > verts;

    discTransform.SetTranslation3D( character->m_position + Vec3( 0.f, 0.f, 0.01f ) );
    g_engine->m_render->SetModelConstants( discTransform );
    AddVertsForDisc3D( verts, Vec3::ZERO, character->m_characterDef.m_sightRadius, Rgba8::GREEN );
    g_engine->m_render->DrawVertexArray( verts );

    verts.clear();
    discTransform.SetTranslation3D( character->m_position + Vec3( 0.f, 0.f, 0.02f ) );
    g_engine->m_render->SetModelConstants( discTransform );
    AddVertsForDisc3D( verts, Vec3::ZERO, character->m_characterDef.m_attackRange, Rgba8::RED );
    g_engine->m_render->DrawVertexArray( verts );
}