#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class HitReactAbility : public GameplayAbility
{
public:
    HitReactAbility()  = default;
    ~HitReactAbility() = default;

    bool ActivateAbility() override;
};