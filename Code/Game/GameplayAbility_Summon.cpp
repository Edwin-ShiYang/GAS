#include "Game/GameplayAbility_Summon.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"

#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/GameplayTask_PlayAnimationAndWait.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "CharacterDefinition.hpp"

//-----------------------------------------------------------------------------------------------
bool GameplayAbility_Summon::ActivateAbility()
{
    if ( !GameplayAbility::ActivateAbility() )
    {
        return false;
    }

    SkeletalMeshComponent* skeletalMeshComponent = m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >();
    GUARANTEE_OR_DIE( skeletalMeshComponent, "Summoner is missing SkeletalMeshComponent" );

    GameplayTask_PlayAnimationAndWait& animationTask = m_ownerASC->CreateGameplayTask< GameplayTask_PlayAnimationAndWait >( *this, *skeletalMeshComponent->m_animator, m_definition->m_animationTrigger, "AnimEnd" );

    animationTask.OnFinished( [ this ]( GameplayTaskEndReason reason ) {
        if ( reason == GameplayTaskEndReason::Succeeded )
        {
            SpawnArcher();
            EndAbility();
        }
    } );
    animationTask.Activate();
    return true;
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Summon::UpdateAbility()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayAbility_Summon::SpawnArcher()
{
    CharacterDefinition const& skeletonDef = CharacterDefinition::GetDefinitionById( "SkeletonArcher" );
    g_app->m_game->SpawnEnemy( skeletonDef );
}