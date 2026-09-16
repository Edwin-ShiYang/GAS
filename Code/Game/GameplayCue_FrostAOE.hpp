#pragma once
#include "Engine/AbilitySystem/GameplayCueNotify_Static.hpp"

//-----------------------------------------------------------------------------------------------
class GameplayCue_FrostAOE : public GameplayCueNotify_Static
{
public:
    GameplayCue_FrostAOE();
    ~GameplayCue_FrostAOE() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};