#pragma once
#include "SkeletalMeshActor.hpp"

//-----------------------------------------------------------------------------------------------
class AnimationClip;
class Clock;
class CharacterAnimationController;
class AbilitySystemComponent;

//-----------------------------------------------------------------------------------------------
class Character : public SkeletalMeshActor
{
public:
    Character( Game* game, std::string const& name );
    ~Character();

    void                          Update() override;
    void                          Render() const override;

    CharacterAnimationController* m_animationController = nullptr;
    AbilitySystemComponent*       m_asc                 = nullptr;
};