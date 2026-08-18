#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"
#include "vector"

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class MeleeAttackAbility : public GameplayAbility
{
public:
    MeleeAttackAbility()  = default;
    ~MeleeAttackAbility() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
    void DebugRender() const override;
    void EndAbility() override;

private:
    float                 m_elapsedSeconds        = 0.0f;
    bool                  m_isHitDetectionEnabled = false;
    std::vector< Actor* > m_hitTargets;
};