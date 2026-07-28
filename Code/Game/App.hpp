#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
class Game;
class Camera;
class App;
class NamedStrings;

extern App* g_app;

//-----------------------------------------------------------------------------------------------
class App
{
public:
    App();
    ~App();

    void        RunMainLoop();
    void        RunFrame();
    void        ResetGame();

    void        Update();
    void        UpdateFromKeyboard();
    void        UpdateImGUI();

    void        Render() const;

    void        SetIsQuitting();
    bool        IsQuitting() const;

    void        LoadGameConfig( char const* path );
    void        PrintKeyBindings() const;

    static bool Event_Quit( EventArgs& args );
    static bool Event_SetTimeScale( EventArgs& args );

private:
    void BuildEngineConfig( EngineConfig& config );

public:
    bool        m_isDebugMode = false;
    bool        m_isQuitting  = false;
    Game*       m_game;
    XmlDocument m_gameConfigDoc;
};