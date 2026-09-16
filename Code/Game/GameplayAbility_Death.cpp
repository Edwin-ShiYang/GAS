#include "GameplayAbility_Death.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/GameplayTask_PlayAnimationAndWait.hpp"

//-----------------------------------------------------------------------------------------------
bool GameplayAbility_Death::ActivateAbility()
{
    GameplayAbility::ActivateAbility();

    SkeletalMeshComponent* skeletalMeshComponent = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >();
    GUARANTEE_OR_DIE( skeletalMeshComponent, Stringf( "skeletalMeshComponent is not found!" ) );

    // Animation
    GameplayTask_PlayAnimationAndWait& animationTask = m_ownerASC->CreateGameplayTask< GameplayTask_PlayAnimationAndWait >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );
    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            EndAbility();
            m_ownerASC->m_owner->m_isDead = true;
        }
    } );

    animationTask.Activate();

    return false;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Death::UpdateAbility()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Death::EndAbility()
{
}