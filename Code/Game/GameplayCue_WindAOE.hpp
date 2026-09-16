#pragma once
#include <Engine/AbilitySystem/GameplayCueNotify_Static.hpp>

//-----------------------------------------------------------------------------------------------
class GameplayCue_WindAOE : public GameplayCueNotify_Static
{
public:
    GameplayCue_WindAOE();
    ~GameplayCue_WindAOE() = default;

    void HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params ) override;
};