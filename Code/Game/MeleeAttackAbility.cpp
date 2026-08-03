#include "Game/MeleeAttackAbility.hpp"
#include "Game/Character.hpp"
#include "Game/Game.hpp"
#include "Prop.hpp"

#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include <Engine/Math/MathUtils.hpp>

#include "Engine/GameFramework/Component.hpp"

//-----------------------------------------------------------------------------------------------
void MeleeAttackAbility::ActivateAbility()
{
    GameplayAbility::ActivateAbility();
    if ( !m_ownerASC || !m_definition )
    {
        EndAbility();
        return;
    }

    Character* owningCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );
    if ( !owningCharacter )
    {
        EndAbility();
        return;
    }

    owningCharacter->PlayAbilityAnimation( m_definition->m_animationName );
}

//-----------------------------------------------------------------------------------------------
void MeleeAttackAbility::UpdateAbility()
{
    /*
    if ( !m_isActive )
    {
        return;
    }

    if ( !m_ownerASC || !m_definition )
    {
        EndAbility();
        return;
    }

    Character* owningCharacter = dynamic_cast< Character* >( m_ownerASC->m_owningActor );
    if ( !owningCharacter )
    {
        EndAbility();
        return;
    }

    m_elapsedSeconds += static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );

    if ( m_elapsedSeconds >= m_hitStartTime && m_elapsedSeconds < m_hitEndTime )
    {
        m_isHitDetectionEnabled = true;

        Vec3 forwardDirection = owningCharacter->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
        Vec2 sectorForward    = Vec2( forwardDirection.x, forwardDirection.y ).GetNormalized();
        Vec2 sectorTip        = Vec2( owningCharacter->m_position.x, owningCharacter->m_position.y );

        for ( GameActor* actor : owningCharacter->m_game->m_characters )
        {
            if ( actor == nullptr )
            {
                continue;
            }

            if ( actor == owningCharacter )
            {
                continue;
            }

            Vec2 actorPosition = Vec2( actor->m_position.x, actor->m_position.y );
            if ( !IsPointInsideDirectedSector2D( actorPosition, sectorTip, sectorForward, 90.f, 1.0f ) )
            {
                continue;
            }

            bool alreadyHit = false;
            for ( Actor* hitTarget : m_hitTargets )
            {
                if ( hitTarget == actor )
                {
                    alreadyHit = true;
                    break;
                }
            }

            if ( alreadyHit )
            {
                continue;
            }

            Character* targetCharacter = dynamic_cast< Character* >( actor );
            if ( targetCharacter == nullptr )
            {
                continue;
            }

            AbilitySystemComponent* targetASC = targetCharacter->GetAbilitySystemComponent();
            if ( targetASC == nullptr )
            {
                continue;
            }

            m_hitTargets.push_back( actor );

            GameplayEffect gameplayEffect;
            gameplayEffect.m_gameplayEffectDef = m_definition->m_gameplayEffectDef;

            owningCharacter->m_asc->ApplyGameplayEffectToTarget( gameplayEffect, targetASC );
        }
    }

    if ( m_elapsedSeconds >= m_hitEndTime )
    {
        m_hitTargets.clear();
        m_isHitDetectionEnabled = false;
        m_elapsedSeconds        = 0.0f;
        EndAbility();
    }
    */
}

//-----------------------------------------------------------------------------------------------
void MeleeAttackAbility::DebugRender() const
{
    if ( !m_isHitDetectionEnabled )
    {
        return;
    }

    Character* owningCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );
    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );
    g_engine->m_render->SetModelConstants( owningCharacter->GetModelToWorldTransform() );

    std::vector< Vertex > verts;
    AddVertsForSector( verts, Vec3( 0.f, 0.f, 0.01f ), 1.0f, owningCharacter->m_orientation.m_yawDegrees, 120.f, Rgba8::GREEN );
    g_engine->m_render->DrawVertexArray( verts );
}