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

//-----------------------------------------------------------------------------------------------
#include <memory>
#include <vector>
#include <Engine/AbilitySystem/GameplayTagManager.hpp>
#include "CharacterDefinition.hpp"

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
class Projectile;
class WaveManager;
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

    void       Startup();

    void       BeginFrame();
    void       EndFrame();

    void       Update();
    void       UpdateFromKeyboard();
    void       UpdateFromController();
    void       UpdateCameras();

    void       Render() const;

    void       SetLightConstants() const;

    // Actor
    void       UpdateEquipments();

    void       RenderEquipments() const;
    void       RenderShadows() const;

    void       SpawnProjectile( Projectile* projectile );
    Character* SpawnEnemy( CharacterDefinition const& characterDef );

private:
    void        RenderAttractMode() const;
    void        UpdateAttractMode();

    void        UpdateActors();
    bool        IsAlive( Actor* actor ) const;
    void        RenderActors() const;
    void        RenderProps() const;

    void        DestroyProps();
    void        DestroyActors();
    void        DestroyDeadActors();

    void        InitHUD();
    void        RenderHUD() const;

    // GAS
    void        RegisterGameplayAbilities();
    void        RegisterGameplayCues();

    //Actor
    ActorHandle GenerateActorHandle( size_t actorIndex );
    void        AddActorToGame( Actor& newActor );

    // ImGUI
    void        DrawActorPanel();
    void        DrawMenuBar();
    void        DrawAnimationPanel();
    void        DrawDebugToolPanel();
    void        DrawGasPanel();
    void        DrawGameplayTagTableNode( GameplayTagNode const* node );

    // AI
    void        CreateAIController( Character& character );
    void        UpdateAIControllers();
    void        DestroyAIControllers();

    // Debug
    void        RenderAIDebug() const;

    //Gameplay
    bool        IsCurrentWaveCleared() const;

public:
    PlayerController*                 m_playerController = nullptr;
    Character*                        m_playerCharacer   = nullptr;

    Clock*                            m_clock = nullptr;

    VertexBuffer*                     m_vertexBuffer = nullptr;
    IndexBuffer*                      m_indexBuffer  = nullptr;

    std::vector< Actor* >             m_actors;
    std::vector< Projectile* >        m_projectiles;
    std::vector< Character* >         m_enemies;

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

    //debug
    bool                              m_enableAI           = true;
    bool                              m_showAiDebug        = false;
    bool                              m_showAnimationDebug = false;
    bool                              m_showDebugGAS       = false;
    bool                              m_showDebugActor     = false;
    bool                              m_showDebugRender    = false;

    //gameplay
    WaveManager*                      m_waveManager        = nullptr;
    int                               m_waveType           = 1;
    float                             m_indicatorIntensity = 1.0f;

private:
    std::vector< AIController* > m_aiControllers;

    Camera*                      m_screenCamera;
    std::vector< UIWidget* >     m_widgets;
    float                        m_bloomThreshold = 1.0f;
    float                        m_exposure       = 1.0f;
    float                        m_bloomIntensity = 0.3f;
};