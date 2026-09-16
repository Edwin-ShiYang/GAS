#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayAbility_Death : public GameplayAbility
{
public:
    GameplayAbility_Death()  = default;
    ~GameplayAbility_Death() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
    void EndAbility() override;
};