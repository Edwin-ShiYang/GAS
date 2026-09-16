#include "GameplayAbility_AOE.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/AbilitySystem/GameplayTask_PlayAnimationAndWait.hpp"
#include "Character.hpp"
#include <Engine/Core/Engine.hpp>
#include <Engine/Math/Mat44.hpp>
#include <Engine/Core/VertexUtils.hpp>
#include "App.hpp"
#include "Game.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "Engine/VFX/SpriteSheetEffect.hpp"
#include "Engine/VFX/VFXSystem.hpp"
#include <Engine/AbilitySystem/GameplayTask_WaitGameplayEvent.hpp>
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/ParticleSystem/ParticleSystem.hpp"

//-----------------------------------------------------------------------------------------------
bool GameplayAbility_AOE::ActivateAbility()
{
    if ( !GameplayAbility::ActivateAbility() )
    {
        return false;
    }

    m_hitTargets.clear();

    Actor*                 owneringActor         = m_ownerASC->m_owner;
    SkeletalMeshComponent* skeletalMeshComponent = owneringActor->GetComponentByClass< SkeletalMeshComponent >();
    if ( !skeletalMeshComponent || !skeletalMeshComponent->m_animator )
    {
        EndAbility();
        return false;
    }

    GameplayTag                     spawnEventTag    = GameplayTagManager::Get().RequestTag( "Event.AOE.Spawn" );
    GameplayTask_WaitGameplayEvent& waitGameplayTask = m_ownerASC->CreateGameplayTask< GameplayTask_WaitGameplayEvent >( *this, spawnEventTag );
    waitGameplayTask.OnEvent( [ this ]( [[maybe_unused]] GameplayEventData const& data ) {
        GameplayCueParameters parameters;
        parameters.m_instigator = m_ownerASC->m_owner;
        parameters.m_target     = g_app->m_game->m_playerCharacer;

        if ( g_app->m_game->m_waveType == 1 )
        {
            parameters.m_target->GetComponentByClass< AbilitySystemComponent >()->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.VFX.FireAOE" ), parameters );
        }
        else if ( g_app->m_game->m_waveType == 2 )
        {
            parameters.m_target->GetComponentByClass< AbilitySystemComponent >()->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.VFX.PoisonAOE" ), parameters );
        }
        else if ( g_app->m_game->m_waveType == 3 )
        {
            parameters.m_target->GetComponentByClass< AbilitySystemComponent >()->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.VFX.WindAOE" ), parameters );
        }
        else if ( g_app->m_game->m_waveType == 4 )
        {
            parameters.m_target->GetComponentByClass< AbilitySystemComponent >()->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.VFX.FrostAOE" ), parameters );
        }
        else if ( g_app->m_game->m_waveType == 5 )
        {
            parameters.m_target->GetComponentByClass< AbilitySystemComponent >()->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.VFX.LightningAOE" ), parameters );
        }
    } );
    waitGameplayTask.Activate();

    skeletalMeshComponent->m_animator->BindNotify( "AOESpawn", [ asc = m_ownerASC, spawnEventTag ]() {
        GameplayEventData data;
        data.m_instigator = asc->m_owner;  // AbilitySystemComponent* asc = this->m_ownerASC;
        asc->SendGameplayEvent( spawnEventTag, data );
    } );

    GameplayTask_PlayAnimationAndWait& animationTask = m_ownerASC->CreateGameplayTask< GameplayTask_PlayAnimationAndWait >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );
    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
        }
    } );
    animationTask.Activate();

    return true;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_AOE::UpdateAbility()
{
    if ( !m_isActive ) return;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_AOE::DebugRender() const
{
    /*
    if ( !m_isAreaDetectionEnabled )
    {
        return;
    }

    Character* owningCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );

    g_engine->m_render->BindTexture( g_defaultWhiteTexture );
    g_engine->m_render->BindShader( ShaderType::Default );

    Mat44 discTransform;
    discTransform.AppendTranslation3D( owningCharacter->m_position + Vec3( 0.f, 0.f, 0.01f ) );

    g_engine->m_render->SetModelConstants( discTransform );

    std::vector< Vertex > verts;
    AddVertsForDisc3D( verts, Vec3::ZERO, 5.0f, Rgba8::GREEN );

    g_engine->m_render->DrawVertexArray( verts );
    */
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_AOE::SpawnAOE()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_AOE::EndAbility()
{
    GameplayAbility::EndAbility();
    Animator* animator = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >()->m_animator;
    animator->UnbindNotify( "AOESpawn" );
}