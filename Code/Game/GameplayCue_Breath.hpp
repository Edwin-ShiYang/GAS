#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AbilitySystem/GameplayCueNotify_Actor.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayCue_Breath : public GameplayCueNotify_Actor
{
public:
    GameplayCue_Breath()  = default;
    ~GameplayCue_Breath() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};