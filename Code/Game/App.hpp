#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

//-----------------------------------------------------------------------------------------------
class Game;
class Camera;
class App;
class NamedStrings;

extern App*                  g_app;
extern RandomNumberGenerator g_rng;

//-----------------------------------------------------------------------------------------------
class App
{
public:
    App();
    ~App();

    void        Startup();
    void        Shutdown();

    void        RunMainLoop();
    void        RunFrame();
    void        ResetGame();

    void        Update();
    void        UpdateFromKeyboard();

    void        Render() const;

    void        SetIsQuitting();
    bool        IsQuitting() const;

    void        LoadGameConfig( char const* path );

    static bool Event_Quit( EventArgs& args );
    static bool Event_SetTimeScale( EventArgs& args );

private:
    EngineConfig CreateEngineConfig();

public:
    bool        m_isDebugMode = false;
    bool        m_isQuitting  = false;
    Game*       m_game;
    XmlDocument m_gameConfigDoc;
};