#include "Game/App.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Game.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB2.hpp"
#include "GameCommon.hpp"
#include "Engine/Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;

//-----------------------------------------------------------------------------------------------
App::App()
{
    EngineConfig config;
    BuildEngineConfig( config );

    Clock::CreateSystemClock();

    g_engine = new Engine( config );
    g_engine->Startup();

    std::string fontPath = g_gameConfigBlackboard.GetValue( "defaultFont", "Data/Fonts/SquirrelFixedFont" );
    g_engine->m_render->CreateOrGetTextureFromFile( ( fontPath + ".png" ).c_str() );
    g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str(), "DefaultFont" );

    DebugRenderConfig debugRenderConfig;
    DebugRenderSystemStartup( debugRenderConfig );

    m_game = new Game();

    SubscribeEventCallbackFunction( "Quit", App::Event_Quit );
    SubscribeEventCallbackFunction( "TimeScale", App::Event_SetTimeScale );

    PrintKeyBindings();
    g_engine->m_devConsole->PrintRegisteredCommands();
}

//-----------------------------------------------------------------------------------------------
App::~App()
{
    delete m_game;
    m_game = nullptr;

    DebugRenderSystemShutdown();

    ModelImporter::CrearModels();

    Clock::DestroySystemClock();

    delete g_engine;
    g_engine = nullptr;
}

//-----------------------------------------------------------------------------------------------
void App::RunMainLoop()
{
    while ( !IsQuitting() )
    {
        RunFrame();
    }
}

//-----------------------------------------------------------------------------------------------
void App::RunFrame()
{
    Clock::GetSystemClock().TickSystemClock();

    g_engine->BeginFrame();
    DebugRenderBeginFrame();
    m_game->BeginFrame();

    Update();
    Render();

    m_game->EndFrame();
    DebugRenderEndFrame();
    g_engine->EndFrame();
}

//-----------------------------------------------------------------------------------------------
void App::Update()
{
    if ( m_game->m_currentGameMode != m_game->m_nextGameMode )
    {
        m_game->m_currentGameMode = m_game->m_nextGameMode;

        if ( m_game->m_currentGameMode == GAME_MODE_ATTRACT )
        {
            ResetGame();
        }
    }

    UpdateFromKeyboard();

    bool lostFocus = !g_engine->m_window->HasFocus();
    if ( lostFocus || g_engine->m_devConsole->IsOpen() )
    {
        g_engine->m_input->SetCursorMode( CursorMode::POINTER );
    }
    else
    {
        g_engine->m_input->SetCursorMode( CursorMode::FPS );
    }

    if ( m_game )
    {
        m_game->Update();
    }

    UpdateImGUI();
}

//-----------------------------------------------------------------------------------------------
void App::Render() const
{
    if ( m_game )
    {
        m_game->Render();
    }

    if ( g_engine->m_devConsole->GetMode() == DevConsoleMode::OPEN_FULL )
    {
        AABB2 bounds = g_engine->m_window->GetClientBounds();
        g_engine->m_devConsole->Render( bounds );
    }
}

//-----------------------------------------------------------------------------------------------
void App::UpdateFromKeyboard()
{
    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_TILDE ) )
    {
        g_engine->m_devConsole->ToggleOpen( DevConsoleMode::OPEN_FULL );
        g_engine->m_input->SetCursorMode( CursorMode::POINTER );
    }

    if ( g_engine->m_input->WasKeyJustPressed( 'O' ) )
    {
        m_game->m_clock->StepSingleFrame();
    }

    if ( g_engine->m_input->WasKeyJustPressed( 'P' ) )
    {
        m_game->m_clock->TogglePause();
    }

    if ( g_engine->m_input->IsKeyDown( 'T' ) )
    {
        m_game->m_clock->SetTimeScale( 0.1 );
    }

    if ( g_engine->m_input->WasKeyJustReleased( 'T' ) )
    {
        m_game->m_clock->SetTimeScale( 1 );
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F8 ) )
    {
        ResetGame();
    }
}

//-----------------------------------------------------------------------------------------------
void App::UpdateImGUI()
{
}

//-----------------------------------------------------------------------------------------------
void App::SetIsQuitting()
{
    m_isQuitting = true;
}

//-----------------------------------------------------------------------------------------------
bool App::IsQuitting() const
{
    return m_isQuitting;
}

//-----------------------------------------------------------------------------------------------
void App::LoadGameConfig( char const* path )
{
    XmlResult result = m_gameConfigDoc.LoadFile( path );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, "Failed to load XML file!" );

    XmlElement* root = m_gameConfigDoc.RootElement();
    GUARANTEE_OR_DIE( root != nullptr, "No root element!" );

    g_gameConfigBlackboard.PopulateFromXmlElementAttributes( *root );
}

//-----------------------------------------------------------------------------------------------
void App::PrintKeyBindings() const
{
    g_engine->m_devConsole->AddLine( Rgba8( 166, 227, 161 ), "Key Bindings" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Mouse x-axis       |Right stick x-axis               - Yaw" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Mouse y-axis       |Right stick y-axis               - Pitch" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Q/E                                                  - Left trigger / right trigger - Roll" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|A/D                |Left stick x-axis                - Move left or right, relative to player orientation" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|W/S                |Left stick y-axis                - Move forward or back, relative to player orientation" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Z/C                |Left shoulder/right shoulder     - Move down or up, relative to the world" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|H/Start button                                       - Reset position and orientation to zero" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Shift/A button                                       - Increase speed by a factor of 10 while held" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|P                                                    - Toggle Pause" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|O                                                    - Step Single Frame" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Hold T                                               - Slow Motion" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|F8                                                   - Reset Game" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|1                                                    - Spawn Line" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|2(Held)                                              - Spawn Point" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|3                                                    - Spawn Wireframe Sphere" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|4                                                    - Spawn Basis" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|5                                                    - Spawn Billboard Text" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|6                                                    - Spawn Wireframe Cylinder" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|7                                                    - Add   Message" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|~                                                    - Toggle DevConsole" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|ESC                                                  - Quit Game / Attract Mode" );
    g_engine->m_devConsole->AddLine( Rgba8::CATPPUCCIN_SKY, "|Space                                                - Start Game" );
}

//-----------------------------------------------------------------------------------------------
void App::ResetGame()
{
    delete m_game;
    m_game = nullptr;
    m_game = new Game();
}

//-----------------------------------------------------------------------------------------------
bool App::Event_Quit( [[maybe_unused]] EventArgs& args )
{
    g_app->SetIsQuitting();
    return true;
}

//-----------------------------------------------------------------------------------------------
bool App::Event_SetTimeScale( EventArgs& args )
{
    if ( !g_engine->m_devConsole || !g_engine->m_devConsole->IsOpen() )
    {
        return false;
    }

    float scale = static_cast< float >( args.GetValue( "Scale", -1.f ) );

    if ( scale < 0.f )
    {
        g_engine->m_devConsole->AddLine( DevConsole::ERROR_COLOR, "[Error] Set the time scale by typing: TimeScale Scale=0.1" );

        return false;
    }

    g_app->m_game->m_clock->SetTimeScale( scale );
    return true;
}

//-----------------------------------------------------------------------------------------------
void App::BuildEngineConfig( EngineConfig& config )
{
    LoadGameConfig( "Data/GameConfig.xml" );
    bool        isFullScreen = g_gameConfigBlackboard.GetValue( "isFullScreen", false );
    std::string windowTitle  = g_gameConfigBlackboard.GetValue( "windowTitle", "Unknow Title" );

    std::string imguiFontPath = g_gameConfigBlackboard.GetValue( "imguiFontPath", "Data/Fonts/Consolas-Regular.ttf" );
    float       imguiFontSize = g_gameConfigBlackboard.GetValue( "imguiFontSize", 18.0f );

    // Window
    config.m_windowConfig.m_windowTitle  = windowTitle;
    config.m_windowConfig.m_isFullScreen = isFullScreen;

    // ImGUI
    config.m_dearImGUISystemConfig.m_fontPath = imguiFontPath;
    config.m_dearImGUISystemConfig.m_fontSize = imguiFontSize;

    config.m_networkConfig.m_isEnabled = false;

    // Render
    config.m_renderConfig.m_enablePBR                = true;
    config.m_renderConfig.m_hdrTexture               = g_gameConfigBlackboard.GetValue( "hdrTexture", "" );
    config.m_renderConfig.m_pbrLitStatic             = g_gameConfigBlackboard.GetValue( "pbrLitStatic", "Data/Shaders/pbrLitStatic" );
    config.m_renderConfig.m_pbrLitSkinned            = g_gameConfigBlackboard.GetValue( "pbrLitSkinned", "Data/Shaders/pbrLitSkinned" );
    config.m_renderConfig.m_brightPass               = g_gameConfigBlackboard.GetValue( "brightPass", "Data/Shaders/BrightPass" );
    config.m_renderConfig.m_horizontalBlurPass       = g_gameConfigBlackboard.GetValue( "horizontalBlurPass", "Data/Shaders/HorizontalBlurPass" );
    config.m_renderConfig.m_verticalBlurPass         = g_gameConfigBlackboard.GetValue( "verticalBlurPass", "Data/Shaders/VerticalBlur" );
    config.m_renderConfig.m_toneMappingPass          = g_gameConfigBlackboard.GetValue( "toneMappingPass", "Data/Shaders/ToneMapping" );
    config.m_renderConfig.m_shadowMap                = g_gameConfigBlackboard.GetValue( "shadowMap", "Data/Shaders/ShadowMap" );
    config.m_renderConfig.m_skybox                   = g_gameConfigBlackboard.GetValue( "skybox", "Data/Shaders/Skybox" );
    config.m_renderConfig.m_equirectangularToCubemap = g_gameConfigBlackboard.GetValue( "equirectangularToCubemap", "Data/Shaders/EquirectangularToCubemap" );
    config.m_renderConfig.m_irradianceConvolution    = g_gameConfigBlackboard.GetValue( "irradianceConvolution", "Data/Shaders/IrradianceConvolution" );
    config.m_renderConfig.m_prefilterEnvironment     = g_gameConfigBlackboard.GetValue( "prefilterEnvironment", "Data/Shaders/PrefilterEnvironment" );
    config.m_renderConfig.m_brdfIntegration          = g_gameConfigBlackboard.GetValue( "brdfIntegration", "Data/Shaders/BRDFIntegration" );

    // Ability System
    config.m_abilitySystemConfig.m_isEnabled                          = true;
    config.m_abilitySystemConfig.m_gameplayEffectDefsFilePath         = g_gameConfigBlackboard.GetValue( "gameplayEffectDefs", "Data/Definitions/GameplayEffectDefinitions.xml" );
    config.m_abilitySystemConfig.m_gameplayAbilityDefsFilePath        = g_gameConfigBlackboard.GetValue( "gameplayAbilityDefs", "Data/Definitions/GameplayAbilityDefinitions.xml" );
    config.m_abilitySystemConfig.m_abilitySystemComponentDefsFilePath = g_gameConfigBlackboard.GetValue( "abilitySystemComponentDefs", "Data/Definitions/AbilitySystemComponentDefinitions.xml" );
}