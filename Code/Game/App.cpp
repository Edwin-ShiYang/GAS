#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB2.hpp"
#include "GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
App* g_app = nullptr;

//-----------------------------------------------------------------------------------------------
App::App()
{
    Startup();
}

//-----------------------------------------------------------------------------------------------
void App::Startup()
{
    LoadGameConfig( "Data/GameConfig.xml" );

    EngineConfig config = CreateEngineConfig();
    g_engine            = new Engine( config );
    g_engine->Startup();

    Clock::CreateSystemClock();

    std::string fontPath = g_gameConfigBlackboard.GetValue( "defaultFont", "Data/Fonts/SquirrelFixedFont" );
    g_engine->m_render->CreateOrGetTextureFromFile( ( fontPath + ".png" ).c_str() );
    g_engine->m_render->CreateOrGetBitmapFont( fontPath.c_str(), "DefaultFont" );

    DebugRenderConfig debugRenderConfig;
    DebugRenderSystemStartup( debugRenderConfig );

    m_game = new Game();
    m_game->Startup();

    SubscribeEventCallbackFunction( "Quit", App::Event_Quit );
    SubscribeEventCallbackFunction( "TimeScale", App::Event_SetTimeScale );
}

//-----------------------------------------------------------------------------------------------
App::~App()
{
    Shutdown();
}

//-----------------------------------------------------------------------------------------------
void App::Shutdown()
{
    if ( m_game )
    {
        delete m_game;
        m_game = nullptr;
    }

    DebugRenderSystemShutdown();
    Clock::DestroySystemClock();

    if ( g_engine )
    {
        g_engine->Shutdown();
        delete g_engine;
        g_engine = nullptr;
    }
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
    UpdateFromKeyboard();

    if ( m_game )
    {
        m_game->Update();
    }
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
void App::ResetGame()
{
    delete m_game;
    m_game = nullptr;

    m_game = new Game();
    m_game->Startup();
}

//-----------------------------------------------------------------------------------------------
EngineConfig App::CreateEngineConfig()
{
    EngineConfig config;

    config.m_particleSystemConfig.m_isEnabled  = g_gameConfigBlackboard.GetValue( "particle", false );
    config.m_animationSystemConfig.m_isEnabled = g_gameConfigBlackboard.GetValue( "animation", false );
    config.m_abilitySystemConfig.m_isEnabled   = g_gameConfigBlackboard.GetValue( "gas", false );
    config.m_networkConfig.m_isEnabled         = g_gameConfigBlackboard.GetValue( "network", false );

    if ( config.m_animationSystemConfig.m_isEnabled )
    {
        config.m_animationSystemConfig.m_animationGraphDefsFilePath = g_gameConfigBlackboard.GetValue( "animationGraphDefs", "" );
        GUARANTEE_OR_DIE( !config.m_animationSystemConfig.m_animationGraphDefsFilePath.empty(), "GameConfig is missing required attribute: animationGraphDefs" );

        config.m_animationSystemConfig.m_animationSetDefsFilePath = g_gameConfigBlackboard.GetValue( "animationSetDefs", "" );
        GUARANTEE_OR_DIE( !config.m_animationSystemConfig.m_animationSetDefsFilePath.empty(), "GameConfig is missing required attribute: animationSetDefs" );
    }

    if ( config.m_abilitySystemConfig.m_isEnabled )
    {
        config.m_abilitySystemConfig.m_gameplayEffectDefsFilePath = g_gameConfigBlackboard.GetValue( "gameplayEffectDefs", "" );
        GUARANTEE_OR_DIE( !config.m_abilitySystemConfig.m_gameplayEffectDefsFilePath.empty(), "GameConfig is missing required attribute: gameplayEffectDefs" );

        config.m_abilitySystemConfig.m_gameplayAbilityDefsFilePath = g_gameConfigBlackboard.GetValue( "gameplayAbilityDefs", "" );
        GUARANTEE_OR_DIE( !config.m_abilitySystemConfig.m_gameplayAbilityDefsFilePath.empty(), "GameConfig is missing required attribute: gameplayAbilityDefs" );

        config.m_abilitySystemConfig.m_abilitySystemComponentDefsFilePath = g_gameConfigBlackboard.GetValue( "abilitySystemComponentDefs", "" );
        GUARANTEE_OR_DIE( !config.m_abilitySystemConfig.m_abilitySystemComponentDefsFilePath.empty(), "GameConfig is missing required attribute: abilitySystemComponentDefs" );
    }

    bool        isFullScreen             = g_gameConfigBlackboard.GetValue( "isFullScreen", false );
    std::string windowTitle              = g_gameConfigBlackboard.GetValue( "windowTitle", "Unknow Title" );
    config.m_windowConfig.m_windowTitle  = windowTitle;
    config.m_windowConfig.m_isFullScreen = isFullScreen;

    std::string imguiFontPath                 = g_gameConfigBlackboard.GetValue( "imguiFontPath", "Data/Fonts/Consolas-Regular.ttf" );
    float       imguiFontSize                 = g_gameConfigBlackboard.GetValue( "imguiFontSize", 18.0f );
    config.m_dearImGUISystemConfig.m_fontPath = imguiFontPath;
    config.m_dearImGUISystemConfig.m_fontSize = imguiFontSize;

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

    return config;
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