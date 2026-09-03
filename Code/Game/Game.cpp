#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/PlayerController.hpp"
#include "Game/Primitive.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Cube.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include <vector>
#include "Character.hpp"
#include "Engine/Renderer/RenderConstants.hpp"
#include "Weapon.hpp"
#include "Game/StaticMeshDefinition.hpp"
#include "SkeletalMeshDefinition.hpp"
#include "CharacterDefinition.hpp"
#include "Engine/AbilitySystem/AttributeSet.hpp"
#include "Engine/AbilitySystem/GameplayAbilityRegistry.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"
#include "MeleeAttackAbility.hpp"
#include "InstantAbility.hpp"
#include "Engine/GameFramework/Actor.hpp"
#include "Engine/AbilitySystem/GameplayTagManager.hpp"
#include "Hotbar.hpp"
#include "UIWidget.hpp"
#include "HealthOrb.hpp"
#include "ManaOrb.hpp"
#include "AbilitySlot.hpp"
#include "Engine/AbilitySystem/GameplayCueManager.hpp"
#include "GameplayCue_HitImpact.hpp"
#include "Engine/Core/Core.hpp"
#include "SpawnDefinition.hpp"
#include "WeaponDefinition.hpp"
#include "AreaAbility.hpp"
#include "AIController.hpp"

//-----------------------------------------------------------------------------------------------
Game::Game()
{
    SpawnDefinition::InitializeDefinitions();
    WeaponDefinition::InitializeDefinitions();
    StaticMeshDefinition::InitializeDefinitions();
    SkeletalMeshDefinition::InitializeDefinitions();
    CharacterDefinition::InitializeDefinitions();
    RegisterAllGameplayAbilities();
}

//-----------------------------------------------------------------------------------------------
void Game::Startup()
{
    m_lightCBO     = g_engine->m_render->CreateConstantBuffer( sizeof( LightConstants ) );
    m_screenCamera = new Camera();
    m_clock        = new Clock( Clock::GetSystemClock() );

    InitHUD();

    Primitive* floor  = new Cube( this );
    floor->m_position = Vec3( 0.f, 0.f, -0.5f );
    floor->SetNonUniformScale( Vec3( 100.f, 100.f, 1.f ) );
    m_primitives.push_back( floor );

    SpawnActors();

    m_playerCharacer = new Character( this, CharacterDefinition::GetDefinitionById( "DarkLord" ) );

    m_playerCharacer->m_position = Vec3( -10.0f, 0.f, 0.f );

    m_actors.push_back( m_playerCharacer );
    m_characters.push_back( m_playerCharacer );
    m_playerController = new PlayerController();
    m_playerController->Possess( m_playerCharacer );
}

//-----------------------------------------------------------------------------------------------
Game::~Game()
{
    DestroyEntities();
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
    WeaponDefinition::ClearDefinitions();
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
    UpdateAIControllers();
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

    if ( m_showDebugMode )
    {
        DrawDebugUI();
    }

    g_engine->m_vfxSystem->Update();
    g_engine->m_particleSystem->Update();

    DrawControlPanel();
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

    // #todo
    for ( size_t i = 0; i < m_characters.size(); ++i )
    {
        if ( m_characters[ i ] )
        {
            m_characters[ i ]->RenderShadow();
            for ( size_t j = 0; j < m_characters[ i ]->m_weapons.size(); ++j )
            {
                if ( m_characters[ i ]->m_weapons[ j ] )
                {
                    m_characters[ i ]->m_weapons[ j ]->RenderShadow();
                }
            }
        }
    }

    g_engine->m_render->EndShadowPass();

    g_engine->m_render->BeginHDRPass();
    g_engine->m_render->BeginCamera( *m_playerController->m_worldCamera );

    SetLightConstants();
    RenderProps();
    RenderActors();

    g_engine->m_vfxSystem->Render( *m_playerController->m_worldCamera );
    g_engine->m_particleSystem->Render( *m_playerController->m_worldCamera );

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->DrawSkyCube( m_playerController->m_worldCamera, 500.f );

    if ( m_showDebugMode )
    {
        RenderDebugMode();
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

    Vec3 lightPosition =
        shadowFocus - lightForward * m_lightViewDistance;

    Mat44 lightCameraMatrix =
        Mat44::MakeLookAtTransform( lightPosition, shadowFocus );

    Mat44 lightViewMatrix =
        lightCameraMatrix.GetOrthonormalInverse();

    Mat44 lightProjectionMatrix =
        Mat44::MakeOrthoProjection(
            -m_shadowHalfSize,
            m_shadowHalfSize,
            -m_shadowHalfSize,
            m_shadowHalfSize,
            m_shadowNear,
            m_shadowFar );

    lightingConstants.c_lightViewMatrix =
        lightViewMatrix;

    lightingConstants.c_lightCameraToRenderMatrix =
        Mat44::MakeCameraToRenderTransform();

    lightingConstants.c_lightProjectionMatrix =
        lightProjectionMatrix;

    g_engine->m_render->CopyCPUToGPU(
        &lightingConstants,
        sizeof( LightConstants ),
        m_lightCBO.get() );

    g_engine->m_render->BindConstantBuffer(
        static_cast< unsigned int >( ConstantBufferSlot::Light ),
        m_lightCBO.get() );
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
        ImGui::Text( "%s [ID %d]", character->m_characterDef.m_id.c_str(), character->m_handle.GetData() );
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
void Game::DrawControlPanel()
{
    ImGui::Begin( "Control" );
    ImGui::Checkbox( "Enable AI", &m_enableAI );
    ImGui::End();
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
void Game::SpawnActors()
{
    for ( SpawnDefinition* spawnDef : SpawnDefinition::s_definitions )
    {
        for ( int actorIndex = 0; actorIndex < static_cast< int >( m_actors.size() ); ++actorIndex )
        {
            if ( m_actors[ actorIndex ] == nullptr )
            {
                AddActorToGame( *spawnDef, actorIndex );
                break;
            }
        }
        AddActorToGame( *spawnDef, m_actors.size() );
    }
}

//-----------------------------------------------------------------------------------------------
void Game::AddActorToGame( SpawnDefinition const& spawnDef, size_t index )
{
    if ( spawnDef.m_type == "Character" )
    {
        CharacterDefinition const& characterDef = CharacterDefinition::GetDefinitionById( spawnDef.m_id );
        Character*                 newCharacter = new Character( this, characterDef );
        newCharacter->m_position                = spawnDef.m_position;
        newCharacter->m_orientation             = spawnDef.m_orientation;
        newCharacter->m_handle                  = GenerateActorHandle( index );

        CreateAIController( *newCharacter );

        m_characters.push_back( newCharacter );
        m_actors.push_back( newCharacter );
    }
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
void Game::RenderDebugMode() const
{
    for ( AIController const* aiController : m_aiControllers )
    {
        if ( !aiController ) continue;
        aiController->RenderDebug();
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
    GameplayAbilityRegistry::Register( "ApplyEffectToSelfAbility", []() -> GameplayAbility* { return new InstantAbility(); } );
    GameplayAbilityRegistry::Register( "AreaAbility", []() -> GameplayAbility* { return new AreaAbility(); } );

    GameplayCueManager::Get().Register( GameplayTagManager::Get().RequestTag( "GameplayCue.Combat.HitImpact" ), std::make_unique< GameplayCue_HitImpact >() );
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