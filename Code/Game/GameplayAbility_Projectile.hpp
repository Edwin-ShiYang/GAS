#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayAbility_Projectile : public GameplayAbility
{
public:
    GameplayAbility_Projectile()  = default;
    ~GameplayAbility_Projectile() = default;

    bool ActivateAbility() override;
    void EndAbility() override;
    void UpdateAbility() override;

private:
    void SpawnProjectile();
};