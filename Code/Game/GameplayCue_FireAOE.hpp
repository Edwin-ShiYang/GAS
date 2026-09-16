#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayCueNotify_Static.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayCue_FireAOE : public GameplayCueNotify_Static
{
public:
    GameplayCue_FireAOE();
    ~GameplayCue_FireAOE() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};