#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Primitive.hpp"
#include "Game/Cube.hpp"
#include "Game/Character.hpp"
#include "Game/Equipment.hpp"
#include "Game/StaticMeshDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"
#include "Game/CharacterDefinition.hpp"
#include "Game/EquipmentDefinition.hpp"
#include "Game/GameplayAbility_Melee.hpp"
#include "Game/GameplayAbility_Projectile.hpp"
#include "Game/GameplayAbility_Summon.hpp"
#include "Game/Hotbar.hpp"
#include "Game/UIWidget.hpp"
#include "Game/HealthOrb.hpp"
#include "Game/ManaOrb.hpp"
#include "Game/AbilitySlot.hpp"
#include "Game/GameplayCue_HitImpact.hpp"
#include "Game/SpawnDefinition.hpp"
#include "Game/GameplayAbility_AOE.hpp"
#include "Game/AIController.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/Core.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/RenderConstants.hpp"
#include "Engine/Animation/AnimationSetDefinition.hpp"
#include "Engine/Animation/AnimationSequence.hpp"
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include "Engine/AbilitySystem/GameplayAbilityRegistry.hpp"
#include "Engine/AbilitySystem/GameplayAbilityDefinition.hpp"
#include "Engine/AbilitySystem/GameplayTagManager.hpp"
#include "Engine/AbilitySystem/GameplayCueManager.hpp"
#include "Engine/AbilitySystem/GameplayEffectDefinition.hpp"
#include "Engine/GameFramework/Actor.hpp"
#include <Engine/AbilitySystem/AbilitySystemComponent.hpp>

#include <vector>
#include <Engine/AbilitySystem/GameplayAbility.hpp>
#include "ProjectileDefinition.hpp"
#include "Projectile.hpp"
#include "WaveManager.hpp"
#include "GameplayAbility_Death.hpp"
#include "GameplayCue_FireAOE.hpp"
#include "GameplayCue_PoisonAOE.hpp"
#include "GameplayCue_LightningAOE.hpp"
#include "GameplayCue_WindAOE.hpp"
#include "GameplayCue_FrostAOE.hpp"
#include "GameplayAbility_Breath.hpp"
#include "GameplayCue_Breath.hpp"

//-----------------------------------------------------------------------------------------------
Game::Game()
{
    SpawnDefinition::InitializeDefinitions();
    EquipmentDefinition::InitializeDefinitions();
    StaticMeshDefinition::InitializeDefinitions();
    SkeletalMeshDefinition::InitializeDefinitions();
    ProjectileDefinition::InitializeDefinitions();
    CharacterDefinition::InitializeDefinitions();

    for ( auto const& [ name, skeletalMeshDef ] : SkeletalMeshDefinition::s_definitions )
    {
        if ( skeletalMeshDef )
        {
            g_engine->m_modelAssets->CreateOrGetSkeletalModel( skeletalMeshDef->m_filePath );
        }
    }

    for ( auto const& [ name, staticMeshDef ] : StaticMeshDefinition::s_definitions )
    {
        if ( staticMeshDef )
        {
            g_engine->m_modelAssets->CreateOrGetStaticModel( staticMeshDef->m_filePath );
        }
    }

    RegisterGameplayAbilities();
    RegisterGameplayCues();
}

//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
    m_lightCBO     = g_engine->m_render->CreateConstantBuffer( sizeof( LightConstants ) );
    m_screenCamera = new Camera();
    m_clock        = new Clock( Clock::GetSystemClock() );

    for ( ProjectileDefinition const* projectileDef : ProjectileDefinition::s_definitions )
    {
        if ( !projectileDef )
        {
            continue;
        }

        if ( projectileDef->m_visualType == ProjectileVisualType::BILLBOARD )
        {
            g_engine->m_render->CreateOrGetTextureFromFile(
                projectileDef->m_spriteSheetPath.c_str(),
                true );
        }
        else if ( projectileDef->m_visualType == ProjectileVisualType::MESH )
        {
            StaticMeshDefinition const& staticMeshDef =
                StaticMeshDefinition::GetDefinitionById( projectileDef->m_staticMeshId );

            g_engine->m_modelAssets->CreateOrGetStaticModel( staticMeshDef.m_filePath );
        }
    }

    InitHUD();

    m_waveManager = new WaveManager( this );

    Primitive* floor  = new Cube( this );
    floor->m_position = Vec3( 0.f, 0.f, -0.5f );
    floor->SetNonUniformScale( Vec3( 100.f, 100.f, 1.f ) );
    m_primitives.push_back( floor );

    //SpawnActors();

    m_playerCharacer             = new Character( this, CharacterDefinition::GetDefinitionById( "Player" ) );
    m_playerCharacer->m_faction  = Faction::GOOD;
    m_playerCharacer->m_position = Vec3( -10.0f, 0.f, 0.f );

    m_actors.push_back( m_playerCharacer );
    m_playerController = new PlayerController();
    m_playerController->Possess( m_playerCharacer );
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
    DestroyActors();
    DestroyProps();
    DestroyAIControllers();

    SAFE_RELEASE( m_screenCamera )
    SAFE_RELEASE( m_vertexBuffer )
    SAFE_RELEASE( m_indexBuffer )
    SAFE_RELEASE( m_clock )
    SAFE_RELEASE( m_playerController )

    StaticMeshDefinition::ClearDefinitions();
    SkeletalMeshDefinition::ClearDefinitions();
    CharacterDefinition::ClearDefinitions();
    SpawnDefinition::ClearDefinitions();
    EquipmentDefinition::ClearDefinitions();
    ProjectileDefinition::ClearDefinitions();

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
    if ( IsCurrentWaveCleared() )
    {
        m_waveManager->StartWave();
    }

    UpdateFromKeyboard();
    UpdateFromController();
    UpdateAIControllers();
    UpdateActors();
    UpdateEquipments();

    m_waveManager->Update();

    if ( m_cameraMode == CameraMode::TopDown )
    {
        g_engine->m_input->SetCursorMode( CursorMode::POINTER );
    }
    else
    {
        g_engine->m_input->SetCursorMode( CursorMode::FPS );
    }

    m_playerController->Update();

    UpdateCameras();

    g_engine->m_vfxSystem->Update();
    g_engine->m_particleSystem->Update();

    DestroyDeadActors();

    DrawMenuBar();
    DrawDebugToolPanel();

    if ( m_showDebugActor ) DrawActorPanel();
    if ( m_showAnimationDebug ) DrawAnimationPanel();
    if ( m_showDebugGAS ) DrawGasPanel();
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateFromKeyboard()
{
    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F1 ) )
    {
        m_cameraMode = m_cameraMode == CameraMode::TopDown ? CameraMode::FreeFly : CameraMode::TopDown;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F2 ) )
    {
        m_showDebugActor = !m_showDebugActor;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F3 ) )
    {
        m_showDebugGAS = !m_showDebugGAS;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F4 ) )
    {
        m_showAnimationDebug = !m_showAnimationDebug;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F5 ) )
    {
        m_showAiDebug = !m_showAiDebug;
    }

    if ( g_engine->m_input->WasKeyJustPressed( KEYCODE_F6 ) )
    {
        m_showDebugRender = !m_showDebugRender;
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
    RenderShadows();
    g_engine->m_render->EndShadowPass();

    g_engine->m_render->BeginHDRPass();
    g_engine->m_render->BeginCamera( *m_playerController->m_worldCamera );

    SetLightConstants();
    RenderProps();
    RenderActors();
    RenderEquipments();

    g_engine->m_vfxSystem->Render( *m_playerController->m_worldCamera );
    g_engine->m_particleSystem->Render( *m_playerController->m_worldCamera );

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->DrawSkyCube( m_playerController->m_worldCamera, 500.f );

    if ( m_showAiDebug )
    {
        RenderAIDebug();
    }

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
void Game::UpdateEquipments()
{
    for ( Actor* actor : m_actors )
    {
        Character* character = dynamic_cast< Character* >( actor );
        if ( !character )
        {
            continue;
        }

        for ( Equipment* equipment : character->m_equipments )
        {
            if ( equipment )
            {
                equipment->Update();
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::RenderEquipments() const
{
    for ( Actor* actor : m_actors )
    {
        Character* character = dynamic_cast< Character* >( actor );
        if ( !character )
        {
            continue;
        }

        for ( Equipment* equipment : character->m_equipments )
        {
            if ( equipment )
            {
                equipment->Render();
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::RenderShadows() const
{
    for ( Actor* actor : m_actors )
    {
        Character* character = dynamic_cast< Character* >( actor );
        if ( !character )
        {
            continue;
        }

        character->RenderShadow();

        for ( Equipment* equipment : character->m_equipments )
        {
            if ( equipment )
            {
                equipment->RenderShadow();
            }
        }
    }
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
    LightConstants lightingConstants  = {};
    lightingConstants.c_sunColor[ 0 ] = m_sunColor.x;
    lightingConstants.c_sunColor[ 1 ] = m_sunColor.y;
    lightingConstants.c_sunColor[ 2 ] = m_sunColor.z;
    lightingConstants.c_sunColor[ 3 ] = m_sunIntensity;
    lightingConstants.c_iblSettings   = Vec4( 0.25f, 0.15f, 0.5f, 0.6f );

    Vec3 lightForward             = m_sunDirection.GetNormalized();
    lightingConstants.c_sunNormal = lightForward;

    Vec3   shadowFocus = Vec3::ZERO;

    Actor* possessedActor = m_playerController->GetPossessedActor();
    if ( possessedActor )
    {
        shadowFocus = possessedActor->m_position;
    }

    Vec3  lightPosition         = shadowFocus - lightForward * m_lightViewDistance;
    Mat44 lightCameraMatrix     = Mat44::MakeLookAtTransform( lightPosition, shadowFocus );
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
void Game::DestroyActors()
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
    m_enemies.clear();
    m_props.clear();
}

//-----------------------------------------------------------------------------------------------
void Game::DestroyDeadActors()
{
    for ( Actor*& actor : m_actors )
    {
        if ( !actor || !actor->m_isDead )
        {
            continue;
        }

        Character* enemy = dynamic_cast< Character* >( actor );
        if ( enemy && enemy->m_faction == Faction::EVIL )
        {
            for ( auto controllerIter = m_aiControllers.begin(); controllerIter != m_aiControllers.end(); ++controllerIter )
            {
                if ( *controllerIter == enemy->m_controller )
                {
                    delete *controllerIter;
                    m_aiControllers.erase( controllerIter );
                    enemy->m_controller = nullptr;
                    break;
                }
            }

            for ( auto enemyIter = m_enemies.begin(); enemyIter != m_enemies.end(); ++enemyIter )
            {
                if ( *enemyIter == enemy )
                {
                    m_enemies.erase( enemyIter );
                    break;
                }
            }
        }

        delete actor;
        actor = nullptr;
    }
}

//-----------------------------------------------------------------------------------------------
void Game::DrawActorPanel()
{
    // Actors
    ImGui::Begin( "Actors" );

    for ( int characterIndex = 0; characterIndex < static_cast< int >( m_actors.size() ); ++characterIndex )
    {
        Character* character = dynamic_cast< Character* >( m_actors[ characterIndex ] );
        if ( !character )
        {
            continue;
        }

        ImGui::PushID( characterIndex );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );

        ImGui::Text( "%s [ID %d]", character->m_characterDef.m_id.c_str(), character->m_handle.GetData() );
        ImGui::PopStyleColor();
        ImGui::Separator();

        ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;

        if ( ImGui::BeginTable( "ProjectileSpawnPosition", 4, flags ) )
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "ActorPosition" );

            ImGui::TableSetColumnIndex( 1 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##ActorPositionX", &character->m_position.x, 0.01f, -10.f, 10.f, "X: %.2f" );

            ImGui::TableSetColumnIndex( 2 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##ActorPositionY", &character->m_position.y, 0.01f, -10.f, 10.f, "Y: %.2f" );

            ImGui::TableSetColumnIndex( 3 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##ActorPositionZ", &character->m_position.z, 0.01f, -10.f, 10.f, "Z: %.2f" );

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "ActorRotation" );

            ImGui::TableSetColumnIndex( 1 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##ActorRotationYaw", &character->m_orientation.m_yawDegrees, 1.f, -0.f, 360.f, "Yaw: %.2f" );

            ImGui::TableSetColumnIndex( 2 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##ActorRotationPitch", &character->m_orientation.m_pitchDegrees, 1.f, -0.f, 360.f, "Pitch: %.2f" );
            ImGui::TableSetColumnIndex( 3 );

            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##ActorRotationRoll", &character->m_orientation.m_rollDegrees, 1.f, -0.f, 360.f, "Roll: %.2f" );

            for ( int weaponIndex = 0; weaponIndex < static_cast< int >( character->m_equipments.size() ); ++weaponIndex )
            {
                Equipment* weapon = character->m_equipments[ weaponIndex ];
                if ( !weapon )
                {
                    continue;
                }

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0 );
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "%s Position", weapon->m_staticMeshDef.m_id.c_str() );

                ImGui::TableSetColumnIndex( 1 );
                ImGui::SetNextItemWidth( -FLT_MIN );
                std::string positionXId = "##" + weapon->m_staticMeshDef.m_id + "_PositionX";
                ImGui::DragFloat( positionXId.c_str(), &weapon->m_position.x, 0.01f, -100.f, 100.f, "X: %.2f" );

                ImGui::TableSetColumnIndex( 2 );
                ImGui::SetNextItemWidth( -FLT_MIN );
                std::string positionYId = "##" + weapon->m_staticMeshDef.m_id + "_PositionY";
                ImGui::DragFloat( positionYId.c_str(), &weapon->m_position.y, 0.01f, -100.f, 100.f, "Y: %.2f" );

                ImGui::TableSetColumnIndex( 3 );
                ImGui::SetNextItemWidth( -FLT_MIN );
                std::string positionZId = "##" + weapon->m_staticMeshDef.m_id + "_PositionZ";
                ImGui::DragFloat( positionZId.c_str(), &weapon->m_position.z, 0.01f, -100.f, 100.f, "Z: %.2f" );

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0 );
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "%s Rotation", weapon->m_staticMeshDef.m_id.c_str() );

                ImGui::TableSetColumnIndex( 1 );
                ImGui::SetNextItemWidth( -FLT_MIN );
                std::string rotationYawId = "##" + weapon->m_staticMeshDef.m_id + "_RotationYaw";
                ImGui::DragFloat( rotationYawId.c_str(), &weapon->m_orientation.m_yawDegrees, 0.01f, 0.f, 360.f, "Yaw: %.2f" );

                ImGui::TableSetColumnIndex( 2 );
                ImGui::SetNextItemWidth( -FLT_MIN );
                std::string rotationPitchId = "##" + weapon->m_staticMeshDef.m_id + "_RotationPitch ";
                ImGui::DragFloat( rotationPitchId.c_str(), &weapon->m_orientation.m_pitchDegrees, 0.01f, 0.f, 360.f, "Pitch: %.2f" );

                ImGui::TableSetColumnIndex( 3 );
                ImGui::SetNextItemWidth( -FLT_MIN );
                std::string rotationRollId = "##" + weapon->m_staticMeshDef.m_id + "_RotationRoll";
                ImGui::DragFloat( rotationRollId.c_str(), &weapon->m_orientation.m_rollDegrees, 0.01f, 0.f, 360.f, "Roll: %.2f" );
            }

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );
            ImGui::AlignTextToFramePadding();
            ImGui::Text( "ProjectileSpawnerPoint" );

            ImGui::TableSetColumnIndex( 1 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##X", &character->m_projectileSpawnPoint.m_position.x, 0.01f, -10.f, 10.f, "X: %.2f" );

            ImGui::TableSetColumnIndex( 2 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##Y", &character->m_projectileSpawnPoint.m_position.y, 0.01f, -10.f, 10.f, "Y: %.2f" );

            ImGui::TableSetColumnIndex( 3 );
            ImGui::SetNextItemWidth( -FLT_MIN );
            ImGui::DragFloat( "##Z", &character->m_projectileSpawnPoint.m_position.z, 0.01f, -10.f, 10.f, "Z: %.2f" );

            ImGui::EndTable();
        }

        ImGui::Separator();
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
            ImGui::DragFloat( "##sunIntensity", &m_sunIntensity, 0.05f, 0.f, 10.f );

            ImGui::Text( "Sun Color" );
            ImGui::SameLine( IMGUI_LINEWIDTH );

            float colorFloat[ 3 ] = { m_sunColor.x, m_sunColor.y, m_sunColor.z };

            if ( ImGui::ColorEdit3( "##sunColor", colorFloat, ImGuiColorEditFlags_DisplayRGB ) )
            {
                m_sunColor = Vec3( colorFloat[ 0 ], colorFloat[ 1 ], colorFloat[ 2 ] );
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

        if ( ImGui::BeginMenu( "Render" ) )
        {
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

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

//-----------------------------------------------------------------------------------------------
void Game::DrawAnimationPanel()
{
    ImGui::Begin( "AnimationSet" );

    ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings;
    for ( AnimationSetDefinition* animationSetDef : AnimationSetDefinition::s_definitions )
    {
        ImGui::PushID( animationSetDef->m_id.c_str() );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );
        ImGui::Text( "%s", animationSetDef->m_id.c_str() );
        ImGui::PopStyleColor();
        ImGui::Separator();

        if ( ImGui::BeginTable( "AnimationSetTable", 2, flags ) )
        {
            ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthFixed, 300.f );
            ImGui::TableSetupColumn( "DurationSeconds", ImGuiTableColumnFlags_WidthStretch );

            ImGui::TableHeadersRow();

            for ( AnimationSequenceDefinition animationSequenceDef : animationSetDef->m_sequenceDefs )
            {
                AnimationSequence const& sequence = g_engine->m_modelAssets->CreateOrGetAnimation( animationSequenceDef.m_animation );
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0 );
                ImGui::Text( "%s", animationSequenceDef.m_id.c_str() );

                ImGui::TableSetColumnIndex( 1 );
                ImGui::Text( "%.2f", sequence.m_durationSeconds );
            }

            ImGui::EndTable();
        }

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::PopID();
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------------------------
void Game::DrawDebugToolPanel()
{
    ImGuiTableFlags flags = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings;

    ImGui::Begin( "DebugTool" );

    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );
    ImGui::Text( "Performance" );
    ImGui::PopStyleColor();
    ImGui::Separator();

    if ( ImGui::BeginTable( "PerformanceTable", 2, flags ) )
    {
        ImGui::TableSetupColumn( "Metric", ImGuiTableColumnFlags_WidthFixed, 200.f );
        ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch );

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex( 0 );
        ImGui::Text( "FPS" );

        ImGui::TableSetColumnIndex( 1 );
        ImGui::Text( "%3.0f", Clock::GetSystemClock().GetFrameRate() );

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex( 0 );
        ImGui::Text( "FPS Avg" );

        ImGui::TableSetColumnIndex( 1 );
        ImGui::Text( "%3.0f", Clock::GetSystemClock().GetFrameRate() );

        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex( 0 );
        ImGui::Text( "Frame Avg" );

        ImGui::TableSetColumnIndex( 1 );
        ImGui::Text( "%5.2f ms", 1000.0f / ImGui::GetIO().Framerate );

        ImGui::EndTable();
    }

    ImGui::Spacing();

    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );
    ImGui::Text( "Control" );
    ImGui::PopStyleColor();
    ImGui::Separator();

    if ( ImGui::BeginTable( "ControlTable", 2, flags ) )
    {
        ImGui::TableSetupColumn( "Key", ImGuiTableColumnFlags_WidthFixed, 200.f );

        ImGui::TableSetupColumn( "Action", ImGuiTableColumnFlags_WidthStretch );

        ImGui::TableHeadersRow();

        char const* controls[][ 2 ] = {
            { "F1",  "Toggle Camera Mode"          },
            { "F2",  "Toggle Actor Debug"          },
            { "F3",  "Toggle Ability System Debug" },
            { "F4",  "Toggle Animation Debug"      },
            { "F5",  "Toggle AI Debug"             },
            { "LMB", "Light Attack"                },
            { "RMB", "Move"                        },
            { "1",   "Combo Attack"                },
            { "2",   "Jump Attack"                 },
            { "3",   "Lightning"                   },
        };

        for ( auto const& control : controls )
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );
            ImGui::TextUnformatted( control[ 0 ] );

            ImGui::TableSetColumnIndex( 1 );
            ImGui::TextUnformatted( control[ 1 ] );
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::Checkbox( "Enable AI", &m_enableAI );

    char const* elements[] = {
        "Physical",
        "Fire",
        "Poison",
        "Wind",
        "Frost",
        "Lightning"
    };

    ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x );
    ImGui::Combo( "#WaveType", &m_waveType, elements, IM_ARRAYSIZE( elements ) );

    ImGui::AlignTextToFramePadding();
    ImGui::Text( "IndicatorIntensity" );
    ImGui::SameLine( 200.f );
    ImGui::DragFloat( "##IndicatorIntensity", &m_indicatorIntensity, 0.01f, 0.f, 100.f );

    ImGui::End();
}

//-----------------------------------------------------------------------------------------------
void Game::DrawGasPanel()
{
    ImGui::Begin( "AbilitySystemComponent" );
    for ( int characterIndex = 0; characterIndex < static_cast< int >( m_actors.size() ); ++characterIndex )
    {
        Character* character = dynamic_cast< Character* >( m_actors[ characterIndex ] );
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
        ImGui::Text( "%s [ID %d]", character->m_characterDef.m_id.c_str(), character->m_handle.GetData() );
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

    ImGui::Begin( "GameplayEffect" );

    for ( GameplayEffectDefinition* gameplayEffectDef : GameplayEffectDefinition::s_definitions )
    {
        if ( !gameplayEffectDef ) continue;

        ImGui::PushID( gameplayEffectDef->m_id.c_str() );
        ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );

        ImGui::Text( "%s [%s]", gameplayEffectDef->m_id.c_str(), gameplayEffectDef->GetDurationPolicyString() );
        ImGui::PopStyleColor();
        ImGui::Separator();

        if ( ImGui::BeginTable( "ModifierTable", 3, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp ) )
        {
            ImGui::TableSetupColumn( "Attribute" );
            ImGui::TableSetupColumn( "operation" );
            ImGui::TableSetupColumn( "magnitude" );
            ImGui::TableHeadersRow();

            for ( GameplayModifierDefinition* gameplayModifierDef : gameplayEffectDef->m_modifiers )
            {
                if ( !gameplayModifierDef ) continue;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex( 0 );
                ImGui::Text( "%s", gameplayModifierDef->m_attributeName.c_str() );

                ImGui::TableSetColumnIndex( 1 );
                ImGui::Text( "%s", gameplayModifierDef->GetOperationString() );

                ImGui::TableSetColumnIndex( 2 );
                ImGui::Text( "%.2f", gameplayModifierDef->m_magnitude );

                ImGui::Spacing();
            }

            ImGui::EndTable();
        }

        ImGui::PopID();
    }

    ImGui::End();

    ImGui::Begin( "GameplayTags" );

    ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;
    if ( ImGui::BeginTable( "GameplayTagTable", 2, tableFlags ) )
    {
        ImGui::TableSetupColumn( "Tag", ImGuiTableColumnFlags_WidthStretch, 3.f );
        ImGui::TableSetupColumn( "ID", ImGuiTableColumnFlags_WidthFixed, 80.f );

        ImGui::TableHeadersRow();

        GameplayTagNode const* root = GameplayTagManager::Get().GetRootNode();

        if ( root )
        {
            for ( GameplayTagNode const* child :
                  root->m_children )
            {
                DrawGameplayTagTableNode( child );
            }
        }

        ImGui::EndTable();
    }
    ImGui::End();

    ImGui::Begin( "Gameplay Abilities" );

    ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp;

    if ( ImGui::BeginTable( "GameplayAbilityTable", 7, flags ) )
    {
        ImGui::TableSetupColumn( "Tag" );
        ImGui::TableSetupColumn( "Type" );
        ImGui::TableSetupColumn( "Effect" );
        ImGui::TableSetupColumn( "Animation Trigger" );
        ImGui::TableSetupColumn( "Cooldown Tag" );
        ImGui::TableSetupColumn( "Range" );
        ImGui::TableSetupColumn( "Radius" );

        ImGui::TableHeadersRow();

        for ( GameplayAbilityDefinition const* abilityDef : GameplayAbilityDefinition::s_definitions )
        {
            if ( !abilityDef )
            {
                continue;
            }

            ImGui::PushID( abilityDef );

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );
            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.80f, 0.65f, 0.99f, 1.0f ) );
            ImGui::TextUnformatted( abilityDef->m_tag.c_str() );
            ImGui::PopStyleColor();

            ImGui::TableSetColumnIndex( 1 );
            ImGui::TextUnformatted( abilityDef->m_type.c_str() );

            ImGui::TableSetColumnIndex( 2 );
            ImGui::TextUnformatted( abilityDef->m_gameplayEffectDef ? abilityDef->m_gameplayEffectDef->m_id.c_str() : "--" );

            ImGui::TableSetColumnIndex( 3 );
            ImGui::TextUnformatted( abilityDef->m_animationTrigger.empty() ? "--" : abilityDef->m_animationTrigger.c_str() );

            ImGui::TableSetColumnIndex( 4 );
            ImGui::TextUnformatted( abilityDef->m_cooldownTag.empty() ? "--" : abilityDef->m_cooldownTag.c_str() );

            ImGui::TableSetColumnIndex( 5 );
            ImGui::Text( "%.2f", abilityDef->m_range );

            ImGui::TableSetColumnIndex( 6 );
            ImGui::Text( "%.2f", abilityDef->m_radius );

            ImGui::PopID();
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

//-----------------------------------------------------------------------------------------------
void Game::DrawGameplayTagTableNode( GameplayTagNode const* node )
{
    if ( !node )
    {
        return;
    }

    bool isLeaf = node->m_children.empty();

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex( 0 );

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_SpanAllColumns;
    if ( isLeaf )
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    bool isOpen = ImGui::TreeNodeEx( node, nodeFlags, "%s", node->m_tag.c_str() );

    ImGui::TableSetColumnIndex( 1 );

    if ( node->m_id >= 0 )
    {
        ImGui::Text( "%d", node->m_id );
    }
    else
    {
        ImGui::TextUnformatted( "--" );
    }

    if ( isOpen && !isLeaf )
    {
        for ( GameplayTagNode const* child : node->m_children )
        {
            DrawGameplayTagTableNode( child );
        }

        ImGui::TreePop();
    }
}

//-----------------------------------------------------------------------------------------------
ActorHandle Game::GenerateActorHandle( size_t actorIndex )
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
void Game::SpawnProjectile( Projectile* projectile )
{
    for ( int projectileIndex = 0; projectileIndex < static_cast< int >( m_projectiles.size() ); ++projectileIndex )
    {
        if ( m_projectiles[ projectileIndex ] == nullptr )
        {
            projectile->m_handle = GenerateActorHandle( projectileIndex );
            break;
        }
    }

    projectile->m_handle = GenerateActorHandle( m_projectiles.size() );
    m_actors.push_back( projectile );
    m_projectiles.push_back( projectile );
}

//-----------------------------------------------------------------------------------------------
Character* Game::SpawnEnemy( CharacterDefinition const& characterDef )
{
    Character* newEnemy = new Character( this, characterDef );

    if ( m_playerCharacer )
    {
        float const randomAngle = g_rng.RollRandomFloatInRange( 0.f, 360.f );

        float const randomRadius = g_rng.RollRandomFloatInRange( 6.f, 8.f );

        Vec2 const  spawnOffset = Vec2::MakeFromPolarDegrees( randomAngle, randomRadius );

        newEnemy->m_position =
            m_playerCharacer->m_position +
            Vec3( spawnOffset.x, spawnOffset.y, 0.f );

        Vec3 const directionToPlayer =
            m_playerCharacer->m_position -
            newEnemy->m_position;

        newEnemy->m_orientation.m_yawDegrees =
            directionToPlayer.GetOrientationAboutZDegrees();
    }
    else
    {
        newEnemy->m_position = Vec3::ZERO;
    }

    newEnemy->m_faction = Faction::EVIL;

    AddActorToGame( *newEnemy );
    m_enemies.push_back( newEnemy );
    CreateAIController( *newEnemy );

    return newEnemy;
}

//-----------------------------------------------------------------------------------------------
void Game::AddActorToGame( Actor& newActor )
{
    size_t actorCount = m_actors.size();
    for ( int actorIndex = 0; actorIndex < actorCount; ++actorIndex )
    {
        if ( !m_actors[ actorIndex ] )
        {
            m_actors[ actorIndex ] = &newActor;
            newActor.m_handle      = GenerateActorHandle( actorIndex );
            return;
        }
    }
    newActor.m_handle = GenerateActorHandle( actorCount );
    m_actors.push_back( &newActor );
}

//-----------------------------------------------------------------------------------------------
void Game::CreateAIController( Character& character )
{
    if ( character.m_characterDef.m_aiEnabled )
    {
        AIController* aiController = new AIController( this, character.m_handle );
        character.m_controller     = aiController;
        m_aiControllers.push_back( aiController );
    }
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateAIControllers()
{
    for ( AIController* aiController : m_aiControllers )
    {
        if ( aiController )
        {
            aiController->Update();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void Game::DestroyAIControllers()
{
    for ( AIController*& aiController : m_aiControllers )
    {
        if ( !aiController ) continue;
        delete aiController;
    }
    m_aiControllers.clear();
}

//-----------------------------------------------------------------------------------------------
void Game::RenderAIDebug() const
{
    for ( AIController const* aiController : m_aiControllers )
    {
        if ( !aiController ) continue;
        aiController->RenderDebug();
    }
}

//-----------------------------------------------------------------------------------------------
bool Game::IsCurrentWaveCleared() const
{
    return m_enemies.size() == 0;
}

//-----------------------------------------------------------------------------------------------
void Game::UpdateActors()
{
    size_t actorCount = m_actors.size();

    for ( size_t actorIndex = 0; actorIndex < actorCount; ++actorIndex )
    {
        Actor* actor = m_actors[ actorIndex ];

        if ( !IsAlive( actor ) )
        {
            continue;
        }

        AbilitySystemComponent* asc = actor->GetComponentByClass< AbilitySystemComponent >();

        // projectile does not have a asc
        if ( asc && asc->m_attributeSet->GetAttribute( "Health" ).m_currentValue <= 0 )
        {
            asc->TryActivateAbility( GameplayTagManager::Get().RequestTag( "Ability.Death" ) );
            asc->CancelActiveAbility();

            Character* character = dynamic_cast< Character* >( actor );
            character->m_controller->SetEnabled( false );
        }

        if ( IsAlive( actor ) )
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
        if ( IsAlive( actor ) )
        {
            actor->Render();
        }
    }
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
    for ( UIWidget const* widget : m_widgets )
    {
        if ( widget )
        {
            widget->Render();
        }
    }
}

//-----------------------------------------------------------------------------------------------
bool Game::IsAlive( Actor* actor ) const
{
    if ( !actor )
    {
        return false;
    }

    return !actor->m_isDead;
}

//-----------------------------------------------------------------------------------------------
void Game::RegisterGameplayAbilities()
{
    GameplayAbilityRegistry::Register( "MeleeAttackAbility", []() -> GameplayAbility* { return new GameplayAbility_Melee(); } );
    GameplayAbilityRegistry::Register( "ApplyEffectToSelfAbility", []() -> GameplayAbility* { return new GameplayAbility_Summon(); } );
    GameplayAbilityRegistry::Register( "AreaAbility", []() -> GameplayAbility* { return new GameplayAbility_AOE(); } );
    GameplayAbilityRegistry::Register( "ProjectileAbility", []() -> GameplayAbility* { return new GameplayAbility_Projectile(); } );
    GameplayAbilityRegistry::Register( "GA_Death", []() -> GameplayAbility* { return new GameplayAbility_Death(); } );
    GameplayAbilityRegistry::Register( "GA_Summon", []() -> GameplayAbility* { return new GameplayAbility_Summon(); } );
    GameplayAbilityRegistry::Register( "GA_Breath", []() -> GameplayAbility* { return new GameplayAbility_Breath(); } );
}

//-----------------------------------------------------------------------------------------------
void Game::RegisterGameplayCues()
{
    GameplayTagManager& gameplayTagManager = GameplayTagManager::Get();

    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.Combat.HitImpact" ), std::make_unique< GameplayCue_HitImpact >() );
    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.VFX.FireAOE" ), std::make_unique< GameplayCue_FireAOE >() );
    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.VFX.PoisonAOE" ), std::make_unique< GameplayCue_PoisonAOE >() );
    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.VFX.LightningAOE" ), std::make_unique< GameplayCue_LightningAOE >() );
    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.VFX.WindAOE" ), std::make_unique< GameplayCue_WindAOE >() );
    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.VFX.FrostAOE" ), std::make_unique< GameplayCue_FrostAOE >() );
    GameplayCueManager::Get().Register( gameplayTagManager.RequestTag( "GameplayCue.VFX.Breath" ), std::make_unique< GameplayCue_Breath >() );
}