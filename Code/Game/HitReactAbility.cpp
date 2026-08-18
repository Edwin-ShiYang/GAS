#include "Game/HitReactAbility.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"
#include "Engine/Animation/Animator.hpp"

//-----------------------------------------------------------------------------------------------
bool HitReactAbility::ActivateAbility()
{
    m_ownerASC->m_owner->GetComponentByClass< SkeletalMeshComponent >()->m_animator->SetTrigger( "HitReactTrigger" );
    return true;
}