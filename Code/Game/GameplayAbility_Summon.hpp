#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class GameplayAbility_Summon : public GameplayAbility
{
public:
    GameplayAbility_Summon()  = default;
    ~GameplayAbility_Summon() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;

private:
    void SpawnArcher();
};