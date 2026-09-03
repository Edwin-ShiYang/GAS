#pragma once

#include "Engine/AbilitySystem/GameplayCueNotify.hpp"

class ParticleEmitter;

class GameplayCue_HitImpact : public GameplayCueNotify
{
public:
    GameplayCue_HitImpact()  = default;
    ~GameplayCue_HitImpact() = default;

    void             HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
    ParticleEmitter* m_particleEmitter = nullptr;
};