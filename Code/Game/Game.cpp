#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Primitive.hpp"
#include "Game/Cylinder.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Cube.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include <vector>
#include "ActorDefinition.hpp"
#include "AnimationSetDefinition.hpp"
#include "Actor.hpp"
#include "SkeletalMeshActor.hpp"
#include "StaticMeshActor.hpp"
#include "Weapon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Character.hpp"
#include "Engine/RenderConstants.hpp"

//-----------------------------------------------------------------------------------------------
Game::Game()
{
    //----------------------------------
    m_fireballTexture      = g_engine->m_render->CreateOrGetTextureFromFile( "Data/fireball.png" );
    m_animSpriteSheet      = new SpriteSheet( *m_fireballTexture, IntVec2( 30, 1 ) );
    m_spriteAnimDefinition = new SpriteAnimDefinition( *m_animSpriteSheet, 0, 29, 30.f, SpriteAnimPlaybackType::LOOP );

    //---------------------------------
    AnimationSetDefinition::InitializeDefinitions();
    ActorDefinition::InitializeDefinitions();

    m_lightCBO = g_engine->m_render->CreateConstantBuffer( sizeof( LightConstants ) );

    m_screenCamera = new Camera();
    m_clock        = new Clock( Clock::GetSystemClock() );

    m_playerController             = new PlayerController();
    m_playerController->m_position = Vec3( 0.f, 0.f, 10.f );

    Primitive* floor  = new Cube( this );
    floor->m_position = Vec3( 0.f, 0.f, -0.5f );
    floor->SetNonUniformScale( Vec3( 100.f, 100.f, 1.f ) );
    m_primitives.push_back( floor );

    m_actors.push_back( new Weapon( this, "Mace" ) );
    m_actors.push_back( new StaticMeshActor( this, "Brazier" ) );
    m_actors.push_back( new Character( this, "DarkLord" ) );

    DebugAddWorldBasis( Mat44(), -1.0f );
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
    DestroyEntities();
    DestroyProps();

    delete m_screenCamera;
    m_screenCamera = nullptr;

    delete m_vertexBuffer;
    m_vertexBuffer = nullptr;

    delete m_indexBuffer;
    m_indexBuffer = nullptr;

    delete m_clock;
    m_clock = nullptr;

    delete m_lightCBO;
    m_lightCBO = nullptr;

    delete m_playerController;
    m_playerController = nullptr;

    AnimationSetDefinition::ClearDefinitions();
    ActorDefinition::ClearDefinitions();
}

//-----------------------------------------------------------------------------------------------
void Game::BeginFrame()
{
}

//-----------------------------------------------------------------------------------------------
void Game::EndFrame()
{
}

//-----------------------------------------------------------------------------------------------
void Game::Update()
{
    if ( m_currentGameMode == GAME_MODE_ATTRACT )
    {
        g_engine->m_input->SetCursorMode( CursorMode::POINTER );
        UpdateAttractMode();
    }

    if ( m_currentGameMode == GAME_MODE_PLAYING )
    {
        UpdateFromKeyboard();
        UpdateFromController();
        UpdateActors();
    }

    m_playerController->Update();

    UpdateImGUI();

    UpdateCameras();
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
    {
        m_nextGameMode = GAME_MODE_ATTRACT;
    }
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateFromController()
{
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateCameras()
{
    AABB2 bounds = g_engine->m_window->GetClientBounds();
    m_playerController->m_worldCamera->SetPerspectiveView( g_engine->m_config.m_windowConfig.m_clientAspect, 60.f, 0.1f, 100.0f );
    m_screenCamera->SetOrthographicView( bounds.m_mins, bounds.m_maxs );
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
    g_engine->m_render->ClearScreen( Rgba8( 0, 0, 0 ) );
    if ( m_currentGameMode == GAME_MODE_ATTRACT )
    {
        RenderAttractMode();
        return;
    }

    // shadow pass
    g_engine->m_render->BeginShadowPass();
    SetLightConstants();

    for ( int propIndex = 0; propIndex < static_cast< int >( m_primitives.size() ); ++propIndex )
    {
        if ( !m_primitives[ propIndex ] )
        {
            continue;
        }
        Primitive* prop = m_primitives[ propIndex ];
        prop->RenderShadow();
    }

    g_engine->m_render->EndShadowPass();

    // HDRPass
    g_engine->m_render->BeginHDRPass();
    g_engine->m_render->BeginCamera( *m_playerController->m_worldCamera );

    SetLightConstants();

    RenderProps();
    RenderActors();

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->DrawSkyCube( m_playerController->m_worldCamera );

    // begin test
    std::vector< Vertex > quadVerts;
    g_engine->m_render->BindShader( ShaderType::Default );
    g_engine->m_render->BindTexture( m_fireballTexture, ShaderResourceSlot ::DIFFUSE );

    float            time      = static_cast< float >( Clock::GetSystemClock().GetTotalSeconds() );
    SpriteDefinition spriteDef = m_spriteAnimDefinition->GetSpriteDefAtTime( time );

    AddVertsForQuad3D(
        quadVerts,
        Vec3( 0.f, -0.5f, -0.5f ),
        Vec3( 0.f, 0.5f, -0.5f ),
        Vec3( 0.f, 0.5f, 0.5f ),
        Vec3( 0.f, -0.5f, 0.5f ),
        Rgba8::WHITE,
        spriteDef.GetUVs() );

    Vec3         fireballPosition = Vec3( 0.f, 0.f, 5.f );
    Vec3         fireballVelocity = Vec3( 1.f, 1.f, 0.f );

    Mat44 const& cameraToWorld  = m_playerController->m_worldCamera->GetCameraToWorldTransform();
    Vec3         cameraPosition = cameraToWorld.GetTranslation3D();

    Vec3         iBasis = ( cameraPosition - fireballPosition ).GetNormalized();

    Vec3         projectedVelocity = fireballVelocity - iBasis * DotProduct3D( fireballVelocity, iBasis );

    Vec3         jBasis;
    Vec3         kBasis;

    if ( projectedVelocity.GetLengthSquared() > 0.0001f )
    {
        kBasis = projectedVelocity.GetNormalized();
        jBasis = CrossProduct3D( kBasis, iBasis ).GetNormalized();
    }
    else
    {
        jBasis = cameraToWorld.GetJBasis3D().GetNormalized();
        kBasis = cameraToWorld.GetKBasis3D().GetNormalized();
    }

    Mat44 transform;
    transform.SetIJKT3D( iBasis, jBasis, kBasis, fireballPosition );

    g_engine->m_render->SetModelConstants( transform );
    g_engine->m_render->DrawVertexArray( quadVerts );
    g_engine->m_render->UnbindTexture( ShaderResourceSlot ::DIFFUSE );
    // end test

    g_engine->m_render->BindShader( ShaderType::Default );

    g_engine->m_render->EndCamera( *m_playerController->m_worldCamera );
    g_engine->m_render->EndHDRPass();

    //EndHDRPass

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );

    g_engine->m_render->BeginBrightPass();
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndBrightPass();

    g_engine->m_render->BeginHorizontalBlurPass();
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndHorizontalBlurPass();

    g_engine->m_render->BeginVerticalBlurPass();
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndVerticalBlurPass();

    g_engine->m_render->BeginToneMappingPass();
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndToneMappingPass();

    g_engine->m_render->BindShader( ShaderType::Default );
    g_engine->m_render->ResetSamplerModes();

    m_playerController->Render();
    DebugRenderScreen( *m_screenCamera );
    DebugRenderWorld( *m_playerController->m_worldCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderProps() const
{
    for ( int propIndex = 0; propIndex < static_cast< int >( m_primitives.size() ); ++propIndex )
    {
        if ( !m_primitives[ propIndex ] )
        {
            continue;
        }

        Primitive* prop = m_primitives[ propIndex ];
        prop->Render();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::DestroyProps()
{
    for ( int propIndex = 0; propIndex < static_cast< int >( m_primitives.size() ); ++propIndex )
    {
        if ( !m_primitives[ propIndex ] )
        {
            continue;
        }

        delete m_primitives[ propIndex ];
        m_primitives[ propIndex ] = nullptr;
    }
}

//-----------------------------------------------------------------------------------------------
void Game::SetLightConstants() const
{
    LightConstants lightingConstants = {};

    lightingConstants.c_sunColor[ 0 ] = NormalizeByte( static_cast< unsigned char >( m_sunColor.x ) );
    lightingConstants.c_sunColor[ 1 ] = NormalizeByte( static_cast< unsigned char >( m_sunColor.y ) );
    lightingConstants.c_sunColor[ 2 ] = NormalizeByte( static_cast< unsigned char >( m_sunColor.z ) );
    lightingConstants.c_sunColor[ 3 ] = NormalizeByte( static_cast< unsigned char >( m_sunColor.w ) );
    lightingConstants.c_sunNormal     = m_sunDirection.GetNormalized();

    Mat44 lightCameraMatrix;
    Vec3  iBasis = m_sunDirection.GetNormalized();
    Vec3  jBasis;
    Vec3  kBasis;
    Vec3  lightPosition = Vec3::ZERO + ( -iBasis * m_lightViewDistance );

    if ( abs( DotProduct3D( iBasis, Vec3::WORLD_UP ) ) < 0.99999f )
    {
        jBasis = CrossProduct3D( Vec3::WORLD_UP, iBasis ).GetNormalized();
        kBasis = CrossProduct3D( iBasis, jBasis ).GetNormalized();
    }
    else
    {
        kBasis = CrossProduct3D( iBasis, Vec3::WORLD_LEFT ).GetNormalized();
        jBasis = CrossProduct3D( kBasis, iBasis ).GetNormalized();
    }

    lightCameraMatrix.SetIJKT3D( iBasis, jBasis, kBasis, lightPosition );
    Mat44 lightViewMatrix = lightCameraMatrix.GetOrthonormalInverse();

    Mat44 lightProjectionMatrix                   = Mat44::MakeOrthoProjection( -m_shadowHalfSize, m_shadowHalfSize, -m_shadowHalfSize, m_shadowHalfSize, m_shadowNear, m_shadowFar );
    lightingConstants.c_lightViewMatrix           = lightViewMatrix;
    lightingConstants.c_lightCameraToRenderMatrix = Mat44::MakeCameraToRenderTransform();
    lightingConstants.c_lightProjectionMatrix     = lightProjectionMatrix;

    g_engine->m_render->CopyCPUToGPU( &lightingConstants, sizeof( LightConstants ), m_lightCBO );
    g_engine->m_render->BindConstantBuffer( static_cast< unsigned int >( ConstantBufferSlot::Light ), m_lightCBO );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode()
{
    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_SPACE ) )
    {
        m_nextGameMode = GAME_MODE_PLAYING;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_ESC ) )
    {
        g_app->SetIsQuitting();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAttractMode() const
{
    g_engine->m_render->BeginCamera( *m_screenCamera );

    std::vector< Vertex > textVerts;
    AABB2                 bounds = g_engine->m_window->GetClientBounds();
    BitmapFont*           font   = g_engine->m_render->m_loadedFontsByName[ "SquirrelFixedFont" ];
    g_engine->m_render->BindTexture( &font->GetTexture() );

    font->AddVertsForTextInBox2D( textVerts, "Attract Mode", bounds, 100.f, Rgba8::CATPPUCCIN_SKY, 1.f, Vec2( 0.5f, 0.5f ), TextDrawMode::SHRINK_TO_FIT );
    g_engine->m_render->DrawVertexArray( textVerts );
    g_engine->m_render->BindTexture( nullptr );

    g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::DestroyEntities()
{
    for ( int actorIndex = 0; actorIndex < static_cast< int >( m_actors.size() ); ++actorIndex )
    {
        if ( m_actors[ actorIndex ] )
        {
            Actor* actor = m_actors[ actorIndex ];
            delete actor;
            actor = nullptr;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateImGUI()
{
    if ( ImGui::BeginMainMenuBar() )
    {
        if ( ImGui::BeginMenu( "Light" ) )
        {
            ImGui::Text( "Sun Direction" );
            ImGui::Text( "X" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##sunX", &m_sunDirection.x, 1.f, -180.f, 180.f );

            ImGui::Text( "Y" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##sunY", &m_sunDirection.y, 1.f, -180.f, 180.f );

            ImGui::Text( "Z" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##sunZ", &m_sunDirection.z, 1.f, -180.f, 180.f );

            ImGui::Text( "Intensity" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragInt( "##sunIntensity", &m_sunColor.w, 1, 0, 255 );

            ImGui::Text( "Sun Color" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            float colorFloat[ 3 ] = {
                static_cast< float >( m_sunColor.x ) / 255.0f,
                static_cast< float >( m_sunColor.y ) / 255.0f,
                static_cast< float >( m_sunColor.z ) / 255.0f
            };

            if ( ImGui::ColorEdit3( "##sunColor", colorFloat, ImGuiColorEditFlags_DisplayRGB ) )
            {
                m_sunColor.x = (int)( colorFloat[ 0 ] * 255.0f + 0.5f );
                m_sunColor.y = (int)( colorFloat[ 1 ] * 255.0f + 0.5f );
                m_sunColor.z = (int)( colorFloat[ 2 ] * 255.0f + 0.5f );
            }

            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Shadow" ) )
        {
            ImGui::Text( "HalfSize" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##HalfSize", &m_shadowHalfSize, 0.1f, 0.1f, 100.0f );

            ImGui::Text( "Near" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##Near", &m_shadowNear, 0.1f, 0.1f, 500.0f );

            ImGui::Text( "Far" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##Far", &m_shadowFar, 0.1f, 0.1f, 500.0f );

            ImGui::Text( "ViewDistance" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##ViewDistance", &m_lightViewDistance, 0.1f, 0.1f, 100.0f );

            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Performance" ) )
        {
            ImGuiIO&     io = ImGui::GetIO();

            static float frameTimes[ 240 ] = {};
            static int   frameIndex        = 0;
            static int   frameCount        = 0;

            frameTimes[ frameIndex ] = io.DeltaTime * 1000.0f;
            frameIndex               = ( frameIndex + 1 ) % 240;

            if ( frameCount < 240 )
            {
                frameCount++;
            }

            int valuesOffset = frameCount == 240 ? frameIndex : 0;

            ImGui::Text( "FPS       : %3.0f", io.Framerate );
            ImGui::Text( "Frame Time: %5.2f ms", io.DeltaTime * 1000.0f );
            ImGui::PlotLines( "##Frame Time", frameTimes, frameCount, valuesOffset, nullptr, 0.0f, 33.3f, ImVec2( 320.0f, 100.0f ) );

            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Info" ) )
        {
            ImGui::Text( "PlayerPos" );
            ImGui::Text( "X" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##playerPosX", &m_playerController->m_position.x, 1.f, -500.f, 500.f );

            ImGui::Text( "Y" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##playerPosY", &m_playerController->m_position.y, 1.f, -500.f, 500.f );

            ImGui::Text( "Z" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##playerPosZ", &m_playerController->m_position.z, 1.f, -500.f, 500.f );

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::LoadAndRegisterTexture( char const* imageFilePath, std::string const& textureName )
{
    Texture* texture = g_engine->m_render->CreateOrGetTextureFromFile( imageFilePath );
    GUARANTEE_OR_DIE( texture, Stringf( "LoadAndRegisterTexture - Couldn't load texture with path: %s", imageFilePath ) );
    g_engine->m_render->m_loadedTexturesByName[ textureName ] = texture;
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateActors()
{
    for ( int actorIndex = 0; actorIndex < static_cast< int >( m_actors.size() ); ++actorIndex )
    {
        if ( m_actors[ actorIndex ] )
        {
            Actor* actor = m_actors[ actorIndex ];
            actor->Update();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::RenderActors() const
{
    for ( int actorIndex = 0; actorIndex < static_cast< int >( m_actors.size() ); ++actorIndex )
    {
        if ( m_actors[ actorIndex ] )
        {
            Actor* actor = m_actors[ actorIndex ];
            actor->Render();
        }
    }
}