#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayCueNotify_Static.hpp"

//-----------------------------------------------------------------------------------------------
class ParticleEmitter;

//-----------------------------------------------------------------------------------------------
class GameplayCue_PoisonAOE : public GameplayCueNotify_Static
{
public:
    GameplayCue_PoisonAOE();
    ~GameplayCue_PoisonAOE() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};