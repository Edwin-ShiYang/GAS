#include "GameplayAbility_Breath.hpp"
#include "Character.hpp"

#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/GameplayTask_PlayAnimationAndWait.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/AbilitySystem/GameplayTask_WaitGameplayEvent.hpp"

//-----------------------------------------------------------------------------------------------
bool GameplayAbility_Breath::ActivateAbility()
{
    if ( !GameplayAbility::ActivateAbility() )
    {
        return false;
    }

    Character* ownerCharacter = dynamic_cast< Character* >( m_ownerASC->m_owner );
    GUARANTEE_OR_DIE( ownerCharacter, "GameplayAbility_Breath owner is not a Character!" );

    SkeletalMeshComponent* skeletalMeshComponent = ownerCharacter->GetComponentByClass< SkeletalMeshComponent >();
    GUARANTEE_OR_DIE( skeletalMeshComponent, "SkeletalMeshComponent is not found!" );
    GUARANTEE_OR_DIE( skeletalMeshComponent->m_animator, "Animator is not found!" );

    GameplayTag                     breathEventTag   = GameplayTagManager::Get().RequestTag( "Event.Breath" );
    GameplayTask_WaitGameplayEvent& waitGameplayTask = m_ownerASC->CreateGameplayTask< GameplayTask_WaitGameplayEvent >( *this, breathEventTag );
    waitGameplayTask.OnEvent( [ this ]( [[maybe_unused]] GameplayEventData const& data ) {
        GameplayCueParameters parameters;
        parameters.m_instigator = m_ownerASC->m_owner;
        parameters.m_instigator->GetComponentByClass< AbilitySystemComponent >()->ExecuteGameplayCue( GameplayTagManager::Get().RequestTag( "GameplayCue.VFX.Breath" ), parameters );
    } );

    waitGameplayTask.Activate();

    skeletalMeshComponent->m_animator->BindNotify( "Breath", [ asc = m_ownerASC, breathEventTag ]() {
        GameplayEventData data;
        data.m_instigator = asc->m_owner;
        asc->SendGameplayEvent( breathEventTag, data );
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
void GameplayAbility_Breath::UpdateAbility()
{
    if ( !m_isActive )
    {
        return;
    }
}

void GameplayAbility_Breath::EndAbility()
{
    GameplayAbility::EndAbility();
}