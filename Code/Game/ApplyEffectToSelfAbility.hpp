#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class ApplyEffectToSelfAbility : public GameplayAbility
{
public:
    ApplyEffectToSelfAbility()  = default;
    ~ApplyEffectToSelfAbility() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
};