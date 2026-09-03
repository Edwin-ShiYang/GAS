#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayAbility.hpp"

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
class Actor;

//-----------------------------------------------------------------------------------------------
class AreaAbility : public GameplayAbility
{
public:
    AreaAbility()  = default;
    ~AreaAbility() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
    void EndAbility() override;
    void DebugRender() const override;

private:
    bool                  m_isAreaDetectionEnabled = false;
    std::vector< Actor* > m_hitTargets;
};