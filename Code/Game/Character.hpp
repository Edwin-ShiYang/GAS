#pragma once
#include "SkeletalMeshActor.hpp"
#include "Engine/AbilitySystem/GameplayEffect.hpp"

//-----------------------------------------------------------------------------------------------
class AnimationClip;
class Clock;
class CharacterAnimationController;
class AbilitySystemComponent;
class AttributeSet;

//-----------------------------------------------------------------------------------------------
class Character : public SkeletalMeshActor
{
public:
    Character( Game* game, std::string const& name );
    ~Character();

    void                          Update() override;
    void                          Render() const override;

    AbilitySystemComponent*       GetAbilitySystemComponent() const;
    AttributeSet*                 GetAttributeSet() const;

    CharacterAnimationController* m_animationController = nullptr;

private:
    AbilitySystemComponent* m_asc = nullptr;
};