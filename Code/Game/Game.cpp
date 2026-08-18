#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Primitive.hpp"

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
#include "Prop.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Character.hpp"
#include "Engine/Renderer/RenderConstants.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Weapon.hpp"
#include "Game/StaticMeshDefinition.hpp"
#include "SkeletalMeshDefinition.hpp"
#include "CharacterDefinition.hpp"
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include "Engine/AbilitySystem/GameplayAbilityRegistry.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "MeleeAttackAbility.hpp"
#include "ApplyEffectToSelfAbility.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/GameFramework/Actor.hpp"
#include "HitReactAbility.hpp"
#include "Engine/AbilitySystem/GameplayTagManager.hpp"
#include "Hotbar.hpp"
#include "Widget.hpp"
#include "HealthOrb.hpp"
#include "ManaOrb.hpp"
#include "AbilitySlot.hpp"
#include "Engine/AbilitySystem/GameplayCueManager.hpp"
#include "HitImpactGameplayCue.hpp"

//-----------------------------------------------------------------------------------------------
Game::Game()
{
    StaticMeshDefinition::InitializeDefinitions();

    SkeletalMeshDefinition::InitializeDefinitions();
    CharacterDefinition::InitializeDefinitions();

    RegisterAllGameplayAbilities();
}

//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
    m_fireballTexture      = g_engine->m_render->CreateOrGetTextureFromFile( "Data/VFX/Sprite-sheet-sheet.png" );
    m_animSpriteSheet      = new SpriteSheet( *m_fireballTexture, IntVec2( 5, 1 ) );
    m_spriteAnimDefinition = new SpriteAnimDefinition( *m_animSpriteSheet, 0, 4, 10.f, SpriteAnimPlaybackType::LOOP );

    m_lightCBO     = g_engine->m_render->CreateConstantBuffer( sizeof( LightConstants ) );
    m_screenCamera = new Camera();
    m_clock        = new Clock( Clock::GetSystemClock() );

    InitHUD();

    Primitive* floor  = new Cube( this );
    floor->m_position = Vec3( 0.f, 0.f, -0.5f );
    floor->SetNonUniformScale( Vec3( 100.f, 100.f, 1.f ) );
    m_primitives.push_back( floor );

    Character* playerActor     = new Character( this, "DarkLord" );
    Actor*     playerCharacter = CreateActor( playerActor );

    m_playerController = new PlayerController();
    m_playerController->Possess( playerCharacter );

    Character* skeleton     = new Character( this, "Skeleton" );
    skeleton->m_position    = Vec3( 5.0f, 0.f, 0.f );
    skeleton->m_orientation = EulerAngles( 180.f, 0.f, 0.f );
    skeleton->GetComponentByClass< AbilitySystemComponent >()->GiveAbility( new HitReactAbility(), GameplayTagManager::Get().RequestTag( "Event.HitReact" ) );
    CreateActor( skeleton );

    StaticMeshDefinition const& swordDef = StaticMeshDefinition::GetDefinitionById( "SM_Weapon_Sword" );
    Weapon*                     sword    = new Weapon( nullptr, swordDef );
    sword->m_owner                       = playerCharacter;
    sword->m_position                    = Vec3( 0.06f, 0.05f, -0.01f );
    sword->m_orientation                 = EulerAngles( 96.f, -16.f, 107.f );
    sword->m_socketName                  = "mixamorig:RightHand";
    m_actors.push_back( sword );
    playerActor->m_weapons.push_back( sword );

    StaticMeshDefinition const& shieldDef = StaticMeshDefinition::GetDefinitionById( "SM_Weapon_Shield" );
    Weapon*                     shield    = new Weapon( nullptr, shieldDef );
    shield->m_owner                       = playerCharacter;
    shield->m_position                    = Vec3( -0.04f, 0.03f, 0.00f );
    shield->m_orientation                 = EulerAngles( 5.3f, 170.f, 95.f );
    shield->m_socketName                  = "mixamorig:LeftHand";
    m_actors.push_back( shield );
    playerActor->m_weapons.push_back( shield );

    StaticMeshDefinition const& straightswordDef = StaticMeshDefinition::GetDefinitionById( "SM_Weapon_Straightsword" );
    Weapon*                     straightsword    = new Weapon( nullptr, straightswordDef );
    straightsword->m_owner                       = skeleton;
    straightsword->m_position                    = Vec3( 0.06f, 0.06f, -0.08f );
    straightsword->m_orientation                 = EulerAngles( 126.f, -11.f, 101.f );
    straightsword->m_socketName                  = "mixamorig:RightHand";
    m_actors.push_back( straightsword );
    skeleton->m_weapons.push_back( straightsword );

    m_vfx = g_engine->m_render->CreateOrGetShader( "Data/Shaders/VFXAdditive" );
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

    delete m_playerController;
    m_playerController = nullptr;

    StaticMeshDefinition::ClearDefinitions();
    SkeletalMeshDefinition::ClearDefinitions();
    CharacterDefinition::ClearDefinitions();

    ActorHandle::s_nextActorUID = 0;
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
    UpdateFromKeyboard();
    UpdateFromController();
    UpdateActors();

    if ( m_cameraMode == CameraMode::TopDown || m_showDebugMode )
    {
        g_engine->m_input->SetCursorMode( CursorMode::POINTER );
    }
    else
    {
        g_engine->m_input->SetCursorMode( CursorMode::FPS );
    }

    m_playerController->Update();

    UpdateCameras();

    for ( int emitterIndex = 0; emitterIndex < static_cast< int >( m_particleEmitters.size() ); ++emitterIndex )
    {
        if ( m_particleEmitters[ emitterIndex ] )
        {
            m_particleEmitters[ emitterIndex ]->Update();
        }
    }

    if ( m_showDebugMode )
    {
        DrawDebugUI();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F1 ) )
    {
        m_showDebugMode = !m_showDebugMode;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F2 ) )
    {
        m_cameraMode = m_cameraMode == CameraMode::TopDown ? CameraMode::FreeFly : CameraMode::TopDown;
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

    if ( m_playerController )
    {
        m_playerController->m_worldCamera->SetPerspectiveView( g_engine->m_config.m_windowConfig.m_clientAspect, 60.f, 0.1f, 100.0f );
    }

    m_screenCamera->SetOrthographicView( bounds.m_mins, bounds.m_maxs );
}

//-----------------------------------------------------------------------------------------------
void Game::Render() const
{
    g_engine->m_render->ClearScreen( Rgba8( 0, 0, 0 ) );

    g_engine->m_render->BeginShadowPass();
    SetLightConstants();

    for ( int primitiveIndex = 0; primitiveIndex < static_cast< int >( m_primitives.size() ); ++primitiveIndex )
    {
        if ( !m_primitives[ primitiveIndex ] )
        {
            continue;
        }

        m_primitives[ primitiveIndex ]->RenderShadow();
    }

    g_engine->m_render->EndShadowPass();

    g_engine->m_render->BeginHDRPass();
    g_engine->m_render->BeginCamera( *m_playerController->m_worldCamera );

    SetLightConstants();

    RenderProps();
    RenderActors();

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->DrawSkyCube( m_playerController->m_worldCamera, 500.f );

    for ( int emitterIndex = 0; emitterIndex < static_cast< int >( m_particleEmitters.size() ); ++emitterIndex )
    {
        if ( m_particleEmitters[ emitterIndex ] )
        {
            m_particleEmitters[ emitterIndex ]->Render( m_playerController->m_worldCamera );
        }
    }

    std::vector< Vertex > quadVerts;

    float                 time      = static_cast< float >( Clock::GetSystemClock().GetTotalSeconds() );
    SpriteDefinition      spriteDef = m_spriteAnimDefinition->GetSpriteDefAtTime( time );

    AddVertsForQuad3D(
        quadVerts,
        Vec3( 0.0f, -0.5f, -0.5f ),
        Vec3( 0.0f, 0.5f, -0.5f ),
        Vec3( 0.0f, 0.5f, 0.5f ),
        Vec3( 0.0f, -0.5f, 0.5f ),
        Rgba8::WHITE,
        spriteDef.GetUVs() );

    Vec3  lightningPosition = Vec3( 3.0f, 0.0f, 1.0f );

    Mat44 transform = GetBillboard( BillboardType::FULL_FACING, m_playerController->m_worldCamera->GetCameraToWorldTransform(), lightningPosition, Vec2( 1.0f, 2.0f ) );

    g_engine->m_render->BindShader( m_vfx );
    g_engine->m_render->BindTextureWithSampler( { m_fireballTexture, SamplerMode::BILINEAR_CLAMP, ShaderResourceSlot::DIFFUSE } );
    g_engine->m_render->SetMaterialConstants( 0.0f, 0.5f, 1.0f, 4.0f, Rgba8::WHITE );
    g_engine->m_render->SetModelConstants( transform, Rgba8::WHITE );
    g_engine->m_render->SetBlendMode( BlendMode::ADDITIVE );
    g_engine->m_render->SetDepthMode( DepthMode::READ_ONLY_LESS_EQUAL );
    g_engine->m_render->DrawVertexArray( quadVerts );

    g_engine->m_render->UnbindTexture( ShaderResourceSlot::DIFFUSE );
    g_engine->m_render->SetMaterialConstants();
    g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
    g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
    g_engine->m_render->BindShader( ShaderType::Default );

    g_engine->m_render->EndCamera( *m_playerController->m_worldCamera );
    g_engine->m_render->EndHDRPass();

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );

    g_engine->m_render->BeginBrightPass();
    g_engine->m_render->SetBloomConstants( m_bloomThreshold );
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndBrightPass();

    g_engine->m_render->BeginHorizontalBlurPass();
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndHorizontalBlurPass();

    g_engine->m_render->BeginVerticalBlurPass();
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndVerticalBlurPass();

    g_engine->m_render->BeginToneMappingPass();
    g_engine->m_render->SetToneMappingConstants( m_exposure, m_bloomIntensity );
    g_engine->m_render->DrawFullQuad();
    g_engine->m_render->EndToneMappingPass();

    g_engine->m_render->BindShader( ShaderType::Default );
    g_engine->m_render->ResetSamplerModes();

    m_playerController->Render();
    DebugRenderScreen( *m_screenCamera );
    DebugRenderWorld( *m_playerController->m_worldCamera );

    g_engine->m_render->BeginCamera( *m_screenCamera );
    RenderHUD();
    g_engine->m_render->EndCamera( *m_screenCamera );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderProps() const
{
    for ( int primitiveIndex = 0; primitiveIndex < static_cast< int >( m_primitives.size() ); ++primitiveIndex )
    {
        if ( !m_primitives[ primitiveIndex ] )
        {
            continue;
        }

        m_primitives[ primitiveIndex ]->Render();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::DestroyProps()
{
    for ( int primitiveIndex = 0; primitiveIndex < static_cast< int >( m_primitives.size() ); ++primitiveIndex )
    {
        if ( !m_primitives[ primitiveIndex ] )
        {
            continue;
        }

        delete m_primitives[ primitiveIndex ];
        m_primitives[ primitiveIndex ] = nullptr;
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

    Mat44 lightViewMatrix       = lightCameraMatrix.GetOrthonormalInverse();
    Mat44 lightProjectionMatrix = Mat44::MakeOrthoProjection( -m_shadowHalfSize, m_shadowHalfSize, -m_shadowHalfSize, m_shadowHalfSize, m_shadowNear, m_shadowFar );

    lightingConstants.c_lightViewMatrix           = lightViewMatrix;
    lightingConstants.c_lightCameraToRenderMatrix = Mat44::MakeCameraToRenderTransform();
    lightingConstants.c_lightProjectionMatrix     = lightProjectionMatrix;

    g_engine->m_render->CopyCPUToGPU( &lightingConstants, sizeof( LightConstants ), m_lightCBO.get() );
    g_engine->m_render->BindConstantBuffer( static_cast< unsigned int >( ConstantBufferSlot::Light ), m_lightCBO.get() );
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateAttractMode()
{
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
            delete m_actors[ actorIndex ];
            m_actors[ actorIndex ] = nullptr;
        }
    }

    m_actors.clear();
    m_characters.clear();
    m_props.clear();
}

//-----------------------------------------------------------------------------------------------
void Game::DrawDebugUI()
{
    DrawMenuBar();
    DrawRenderPanel();

    ImGui::Begin( "Actors" );

    for ( int characterIndex = 0; characterIndex < static_cast< int >( m_characters.size() ); ++characterIndex )
    {
        Character* character = m_characters[ characterIndex ];
        if ( !character )
        {
            continue;
        }

        AttributeSet* attributeSet = character->GetAttributeSet();
        if ( !attributeSet )
        {
            continue;
        }

        ImGui::PushID( characterIndex );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );
        ImGui::Text( "%s [ID %d]", character->m_characterDef->m_id.c_str(), character->m_handle.GetData() );
        ImGui::PopStyleColor();
        ImGui::Separator();

        if ( ImGui::BeginTable( "AttributesTable", 3, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp ) )
        {
            ImGui::TableSetupColumn( "Attribute" );
            ImGui::TableSetupColumn( "Base" );
            ImGui::TableSetupColumn( "Current" );
            ImGui::TableHeadersRow();

            for ( auto const& [ key, data ] : attributeSet->GetAttributes() )
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0 );
                ImGui::Text( "%s", key.c_str() );

                ImGui::TableSetColumnIndex( 1 );
                ImGui::Text( "%.2f", data.m_baseValue );

                ImGui::TableSetColumnIndex( 2 );
                ImGui::Text( "%.2f", data.m_currentValue );
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::PopID();
    }

    ImGui::End();

    ImGui::Begin( "Weapons" );

    for ( int characterIndex = 0; characterIndex < static_cast< int >( m_characters.size() ); ++characterIndex )
    {
        Character* character = m_characters[ characterIndex ];
        if ( !character )
        {
            continue;
        }

        ImGui::PushID( characterIndex );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );
        ImGui::Text( "%s [ID %d]", character->m_characterDef->m_id.c_str(), character->m_handle.GetData() );
        ImGui::PopStyleColor();

        for ( int weaponIndex = 0; weaponIndex < static_cast< int >( character->m_weapons.size() ); ++weaponIndex )
        {
            Weapon* weapon = character->m_weapons[ weaponIndex ];
            if ( !weapon )
            {
                continue;
            }

            if ( ImGui::CollapsingHeader( weapon->m_staticMeshDef.m_id.c_str() ) )
            {
                ImGui::PushID( weaponIndex );

                ImGui::Text( "X" );
                ImGui::SameLine( IMGUI_LINEWIDTH );
                ImGui::DragFloat( "##X", &weapon->m_position.x, 0.01f, -256.f, 256.f );

                ImGui::Text( "Y" );
                ImGui::SameLine( IMGUI_LINEWIDTH );
                ImGui::DragFloat( "##Y", &weapon->m_position.y, 0.01f, -256.f, 256.f );

                ImGui::Text( "Z" );
                ImGui::SameLine( IMGUI_LINEWIDTH );
                ImGui::DragFloat( "##Z", &weapon->m_position.z, 0.01f, -256.f, 256.f );

                ImGui::Text( "Yaw" );
                ImGui::SameLine( IMGUI_LINEWIDTH );
                ImGui::DragFloat( "##Yaw", &weapon->m_orientation.m_yawDegrees, 0.1f, -360.f, 360.f );

                ImGui::Text( "Pitch" );
                ImGui::SameLine( IMGUI_LINEWIDTH );
                ImGui::DragFloat( "##Pitch", &weapon->m_orientation.m_pitchDegrees, 0.1f, -360.f, 360.f );

                ImGui::Text( "Roll" );
                ImGui::SameLine( IMGUI_LINEWIDTH );
                ImGui::DragFloat( "##Roll", &weapon->m_orientation.m_rollDegrees, 0.1f, -360.f, 360.f );

                ImGui::PopID();
            }
        }

        ImGui::PopID();
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------------------------
void Game::DrawMenuBar()
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
                m_sunColor.x = static_cast< int >( colorFloat[ 0 ] * 255.0f + 0.5f );
                m_sunColor.y = static_cast< int >( colorFloat[ 1 ] * 255.0f + 0.5f );
                m_sunColor.z = static_cast< int >( colorFloat[ 2 ] * 255.0f + 0.5f );
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

        if ( ImGui::BeginMenu( "Camera" ) )
        {
            ImGui::Text( "Position" );

            ImGui::Text( "X" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##playerPosX", &m_playerController->m_position.x, 1.f, -500.f, 500.f );

            ImGui::Text( "Y" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##playerPosY", &m_playerController->m_position.y, 1.f, -500.f, 500.f );

            ImGui::Text( "Z" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##playerPosZ", &m_playerController->m_position.z, 1.f, -500.f, 500.f );

            ImGui::Text( "Rotation" );

            ImGui::Text( "Yaw" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##Yaw", &m_playerController->m_orientation.m_yawDegrees, 1.f, -180.f, 180.f );

            ImGui::Text( "Pitch" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##Pitch", &m_playerController->m_orientation.m_pitchDegrees, 1.f, -180.f, 180.f );

            ImGui::Text( "Roll" );
            ImGui::SameLine( IMGUI_LINEWIDTH );
            ImGui::DragFloat( "##Roll", &m_playerController->m_orientation.m_rollDegrees, 1.f, -180.f, 180.f );

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::DrawRenderPanel()
{
    ImGui::Begin( "Render" );

    ImGui::AlignTextToFramePadding();
    ImGui::Text( "BloomThreshold" );
    ImGui::SameLine( 200.f );
    ImGui::DragFloat( "##BloomThreshold", &m_bloomThreshold, 0.01f, 0.f, 100.f );

    ImGui::AlignTextToFramePadding();
    ImGui::Text( "Exposure" );
    ImGui::SameLine( 200.f );
    ImGui::DragFloat( "##Exposure", &m_exposure, 0.01f, 0.f, 100.f );

    ImGui::AlignTextToFramePadding();
    ImGui::Text( "BloomIntensity" );
    ImGui::SameLine( 200.f );
    ImGui::DragFloat( "##BloomIntensity", &m_bloomIntensity, 0.01f, 0.f, 100.f );

    ImGui::End();
}

//-----------------------------------------------------------------------------------------------
void Game::LoadAndRegisterTexture( char const* imageFilePath, std::string const& textureName )
{
    Texture* texture = g_engine->m_render->CreateOrGetTextureFromFile( imageFilePath );
    GUARANTEE_OR_DIE( texture, Stringf( "LoadAndRegisterTexture - Couldn't load texture with path: %s", imageFilePath ) );
    g_engine->m_render->m_loadedTexturesByName[ textureName ] = texture;
}

//-----------------------------------------------------------------------------------------------
ActorHandle Game::GenerateActorHandle( unsigned int actorIndex )
{
    GUARANTEE_OR_DIE( actorIndex < ActorHandle::MAX_ACTOR_INDEX, "Index exceeded MAX_ACTOR_INDEX!" );

    unsigned int uid = ActorHandle::s_nextActorUID;

    if ( ActorHandle::s_nextActorUID >= ActorHandle::MAX_ACTOR_UID )
    {
        ActorHandle::s_nextActorUID = 0;
    }
    else
    {
        ActorHandle::s_nextActorUID++;
    }

    return ActorHandle( uid, actorIndex );
}

//-----------------------------------------------------------------------------------------------
Actor* Game::CreateActor( Actor* newActor )
{
    for ( int actorIndex = 0; actorIndex < static_cast< int >( m_actors.size() ); ++actorIndex )
    {
        if ( m_actors[ actorIndex ] == nullptr )
        {
            newActor->m_handle     = GenerateActorHandle( actorIndex );
            m_actors[ actorIndex ] = newActor;
            AddActorToGame( newActor );
            return newActor;
        }
    }

    newActor->m_handle = GenerateActorHandle( static_cast< unsigned int >( m_actors.size() ) );
    m_actors.push_back( newActor );
    AddActorToGame( newActor );

    return newActor;
}

//-----------------------------------------------------------------------------------------------
void Game::AddActorToGame( Actor* actor )
{
    if ( Character* character = dynamic_cast< Character* >( actor ) )
    {
        m_characters.push_back( character );
        return;
    }

    if ( Prop* prop = dynamic_cast< Prop* >( actor ) )
    {
        m_props.push_back( prop );
    }
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateActors()
{
    for ( Actor* actor : m_actors )
    {
        if ( actor )
        {
            actor->Update();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::RenderActors() const
{
    for ( Actor* actor : m_actors )
    {
        if ( actor )
        {
            actor->Render();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::RegisterAllGameplayAbilities()
{
    GameplayAbilityRegistry::Register( "MeleeAttackAbility", []() -> GameplayAbility* { return new MeleeAttackAbility(); } );
    GameplayAbilityRegistry::Register( "ApplyEffectToSelfAbility", []() -> GameplayAbility* { return new ApplyEffectToSelfAbility(); } );

    GameplayCueManager::Get().Register( GameplayTagManager::Get().RequestTag( "GameplayCue.Combat.HitImpact" ), std::make_unique< HitImpactGameplayCue >() );
}

//-----------------------------------------------------------------------------------------------
void Game::InitHUD()
{
    m_widgets.push_back( new Hotbar() );

    for ( int slotIndex = 0; slotIndex < 6; ++slotIndex )
    {
        m_widgets.push_back( new AbilitySlot( slotIndex ) );
    }

    m_widgets.push_back( new HealthOrb() );
    m_widgets.push_back( new ManaOrb() );
}

//-----------------------------------------------------------------------------------------------
void Game::RenderHUD() const
{
    for ( Widget const* widget : m_widgets )
    {
        if ( widget )
        {
            widget->Render();
        }
    }
}