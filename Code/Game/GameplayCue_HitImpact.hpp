#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayCueNotify_Static.hpp"

//-----------------------------------------------------------------------------------------------
class ParticleEmitter;

//-----------------------------------------------------------------------------------------------
class GameplayCue_HitImpact : public GameplayCueNotify_Static
{
public:
    GameplayCue_HitImpact()  = default;
    ~GameplayCue_HitImpact() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};