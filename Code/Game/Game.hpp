#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/GameFramework/ActorHandle.hpp"
#include "Engine/GameFramework/Actor.hpp"
#include "UIWidget.hpp"
#include "GameCommon.hpp"
#include "SpawnDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include <memory>
#include <vector>

//-----------------------------------------------------------------------------------------------
class Camera;
class SpriteSheetEffect;
class Clock;
class Prop;
class PlayerController;
class Character;
class Primitive;
class Shader;
class Texture;
class AIController;
class SpriteSheet;
class SpriteAnimDefinition;
class ParticleEmitter;
struct Vertex;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11SamplerState;

//-----------------------------------------------------------------------------------------------
enum GameMode
{
    GAME_MODE_ATTRACT,
    GAME_MODE_PLAYING,
};

//-----------------------------------------------------------------------------------------------
class Game
{
public:
    Game();
    ~Game();

    void Startup();

    void BeginFrame();
    void EndFrame();

    void Update();
    void UpdateFromKeyboard();
    void UpdateFromController();
    void UpdateCameras();

    void Render() const;
    void SetLightConstants() const;

private:
    void        RenderAttractMode() const;
    void        UpdateAttractMode();

    void        UpdateActors();

    void        RenderActors() const;
    void        RenderProps() const;

    void        DestroyProps();
    void        DestroyEntities();

    ActorHandle GenerateActorHandle( size_t actorIndex );
    void        SpawnActors();

    void        RegisterAllGameplayAbilities();
    void        InitHUD();
    void        RenderHUD() const;
    void        AddActorToGame( SpawnDefinition const& spawnDef, size_t index );

    // ImGUI
    void        DrawDebugUI();
    void        DrawMenuBar();
    void        DrawRenderPanel();
    void        DrawControlPanel();

    // AI
    void        CreateAIController( Character& character );
    void        UpdateAIControllers();
    void        DestroyAIControllers();

    // Debug
    void        RenderDebugMode() const;

public:
    PlayerController*                 m_playerController = nullptr;
    Character*                        m_playerCharacer   = nullptr;

    Clock*                            m_clock = nullptr;

    VertexBuffer*                     m_vertexBuffer = nullptr;
    IndexBuffer*                      m_indexBuffer  = nullptr;

    std::vector< Actor* >             m_actors;
    std::vector< Character* >         m_characters;
    std::vector< Prop* >              m_props;
    std::vector< Primitive* >         m_primitives;
    std::vector< Vertex >             m_verts;
    CameraMode                        m_cameraMode = CameraMode::TopDown;

    // light - move to render
    std::unique_ptr< ConstantBuffer > m_lightCBO     = nullptr;
    Vec3                              m_sunDirection = Vec3( 3.f, 1.0f, -2.0f );

    Vec3                              m_sunColor     = Vec3( 1.f, 1.f, 1.f );
    float                             m_sunIntensity = 2.f;

    float                             m_shadowHalfSize    = 20.f;
    float                             m_lightViewDistance = 25.f;
    float                             m_shadowNear        = 0.1f;
    float                             m_shadowFar         = 60.f;

    // ParticleEmitter*                m_particleEmitter      = nullptr;
    std::vector< Vertex >             m_testVerts;
    std::vector< unsigned int >       m_testIndices;
    bool                              m_showDebugMode = false;

    //debug
    bool                              m_enableAI = true;

private:
    std::vector< AIController* > m_aiControllers;

    Camera*                      m_screenCamera;
    std::vector< UIWidget* >     m_widgets;
    float                        m_bloomThreshold = 1.0f;
    float                        m_exposure       = 1.0f;
    float                        m_bloomIntensity = 0.3f;
};