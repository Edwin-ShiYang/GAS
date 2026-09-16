#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class GameplayAbility_HitReact : public GameplayAbility
{
public:
    GameplayAbility_HitReact()  = default;
    ~GameplayAbility_HitReact() = default;

    bool ActivateAbility() override;
};