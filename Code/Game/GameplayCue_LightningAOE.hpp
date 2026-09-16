#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayCueNotify_Static.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayCue_LightningAOE : public GameplayCueNotify_Static
{
public:
    GameplayCue_LightningAOE();
    ~GameplayCue_LightningAOE() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};