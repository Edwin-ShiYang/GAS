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

    void ActivateAbility() override;
    void UpdateAbility() override;
    void DebugRender() const override;

private:
    float                 m_hitStartTime          = 0.5f;
    float                 m_hitEndTime            = 0.8f;
    float                 m_elapsedSeconds        = 0.0f;
    bool                  m_isHitDetectionEnabled = false;
    std::vector< Actor* > m_hitTargets;
};