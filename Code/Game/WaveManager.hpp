#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Timer.hpp"

//-----------------------------------------------------------------------------------------------
class Game;

//-----------------------------------------------------------------------------------------------
class WaveManager
{
public:
    WaveManager( Game* game );
    ~WaveManager() = default;

    void  StartWave();
    void  Update();

    Game* m_game = nullptr;
    Timer m_nextWaveTimer;
    int   m_currentWave = 0;
};