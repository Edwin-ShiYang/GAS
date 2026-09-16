#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class GameplayAbility_AOE : public GameplayAbility
{
public:
    GameplayAbility_AOE()  = default;
    ~GameplayAbility_AOE() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
    void EndAbility() override;
    void DebugRender() const override;
    void SpawnAOE();

private:
    bool                  m_isAreaDetectionEnabled = false;
    std::vector< Actor* > m_hitTargets;
};