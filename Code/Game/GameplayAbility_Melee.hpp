#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "vector"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class GameplayAbility_Melee : public GameplayAbility
{
public:
    GameplayAbility_Melee()  = default;
    ~GameplayAbility_Melee() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
    void DebugRender() const override;
    void EndAbility() override;

private:
    bool                  m_isHitDetectionEnabled = false;
    std::vector< Actor* > m_hitTargets;
};