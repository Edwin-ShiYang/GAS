#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class InstantAbility : public GameplayAbility
{
public:
    InstantAbility()  = default;
    ~InstantAbility() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;

private:
};