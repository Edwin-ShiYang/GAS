#pragma once

#include "Engine/AbilitySystem/GameplayCueNotify.hpp"

class ParticleEmitter;

class HitImpactGameplayCue : public GameplayCueNotify
{
public:
    HitImpactGameplayCue()  = default;
    ~HitImpactGameplayCue() = default;

    void             HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
    ParticleEmitter* m_particleEmitter = nullptr;
};