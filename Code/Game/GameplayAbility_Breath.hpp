#pragma once

#include "Engine/AbilitySystem/GameplayAbility.hpp"

class ParticleEmitter;

//-----------------------------------------------------------------------------------------------
class GameplayAbility_Breath : public GameplayAbility
{
public:
    GameplayAbility_Breath()  = default;
    ~GameplayAbility_Breath() = default;

    bool ActivateAbility() override;
    void UpdateAbility() override;
    void EndAbility() override;

private:
    ParticleEmitter* m_fireBreathEmitter = nullptr;

    float            m_fireBreathElapsed  = 0.f;
    float            m_fireBreathDuration = 5.f;
    float            m_sweepStartYaw      = 45.f;
    float            m_sweepEndYaw        = -45.f;
};