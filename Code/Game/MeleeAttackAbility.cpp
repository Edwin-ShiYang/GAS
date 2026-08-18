#include "Game/MeleeAttackAbility.hpp"
#include "Game/Character.hpp"

#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/Animation/Animator.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "App.hpp"
#include "Game.hpp"
#include "Engine/AbilitySystem/WaitDelayTask.hpp"
#include "Engine/AbilitySystem/PlayAnimationAndWaitTask.hpp"

//-----------------------------------------------------------------------------------------------
bool MeleeAttackAbility::ActivateAbility()
{
    if ( !GameplayAbility::ActivateAbility() )
    {
        return false;
    }

    m_hitTargets.clear();

    Actor* owneringActor = m_ownerASC->m_owner;
    if ( !owneringActor )
    {
        EndAbility();
        return false;
    }

    /*
    WaitDelayTask& delayTask = m_ownerASC->CreateGameplayTask< WaitDelayTask >( *this, 1.0f );
    delayTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    delayTask.Activate();
    */

    SkeletalMeshComponent* skeletalMeshComponent = owneringActor->GetComponentByClass< SkeletalMeshComponent >();
    if ( !skeletalMeshComponent || !skeletalMeshComponent->m_animator )
    {
        EndAbility();
        return false;
    }

    PlayAnimationAndWaitTask& animationTask = m_ownerASC->CreateGameplayTask< PlayAnimationAndWaitTask >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );
    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    animationTask.Activate();

    // skeletalMeshComponent->m_animator->BindNotify( "AnimEnd", [ this ]() { EndAbility(); } );
    skeletalMeshComponent->m_animator->BindNotify( "HitStart", [ this ]() { m_isHitDetectionEnabled = true; } );
    skeletalMeshComponent->m_animator->BindNotify( "HitEnd", [ this ]() { m_isHitDetectionEnabled = false; } );

    return true;
}

//-----------------------------------------------------------------------------------------------
void MeleeAttackAbility::UpdateAbility()
{
    if ( !m_isActive )
    {
        return;
    }

    if ( m_isHitDetectionEnabled )
    {
        Vec3 forward       = m_ownerASC->m_owner->m_orientation.GetForwardDir_IFwd_JLeft_KUp().GetNormalized();
        Vec2 sectorForward = Vec2( forward.x, forward.y ).GetNormalized();
        Vec2 sectorTip     = Vec2( m_ownerASC->m_owner->m_position.x, m_ownerASC->m_owner->m_position.y );

        for ( Actor* actor : g_app->m_game->m_characters )
        {
            if ( actor == nullptr )
            {
                continue;
            }

            Vec2 actorPosition = Vec2( actor->m_position.x, actor->m_position.y );
            if ( !IsPointInsideDirectedSector2D( actorPosition, sectorTip, sectorForward, 120.f, 1.5f ) )
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
            m_ownerASC->ApplyGameplayEffectToTarget( gameplayEffect, targetASC );

            GameplayEventData gameplayEventData;
            gameplayEventData.m_instigator = m_ownerASC->m_owner;
            gameplayEventData.m_target     = targetASC->m_owner;
            targetASC->SendGameplayEvent( GameplayTagManager::Get().RequestTag( "Event.HitReact" ), gameplayEventData );

            GameplayCueParameters parameters;

            parameters.m_instigator = m_ownerASC->m_owner;
            parameters.m_target     = targetASC->m_owner;

            targetASC->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.Combat.HitImpact" ), parameters );
        }
    }
}

void MeleeAttackAbility::DebugRender() const
{
    if ( !m_isHitDetectionEnabled )
    {
        return;
    }

    Character* owningCharacter =
        dynamic_cast< Character* >( m_ownerASC->m_owner );

    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );

    Mat44 sectorTransform;

    sectorTransform.AppendTranslation3D(
        owningCharacter->m_position + Vec3( 0.f, 0.f, 0.01f ) );

    EulerAngles yawOnlyOrientation;
    yawOnlyOrientation.m_yawDegrees =
        owningCharacter->m_orientation.m_yawDegrees;
    yawOnlyOrientation.m_pitchDegrees = 0.f;
    yawOnlyOrientation.m_rollDegrees  = 0.f;

    sectorTransform.Append(
        yawOnlyOrientation.GetAsMatrix_IFwd_JLeft_KUp() );

    g_engine->m_render->SetModelConstants( sectorTransform );

    std::vector< Vertex > verts;
    AddVertsForSector(
        verts,
        Vec3::ZERO,
        1.5f,
        0.f,
        120.f,
        Rgba8::GREEN );

    g_engine->m_render->DrawVertexArray( verts );
}

//-----------------------------------------------------------------------------------------------
void MeleeAttackAbility::EndAbility()
{
    GameplayAbility::EndAbility();
    Animator* animator = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >()->m_animator;
    animator->UnbindNotify( "AnimEnd" );
}