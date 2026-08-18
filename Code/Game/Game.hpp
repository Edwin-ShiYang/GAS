#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/IntVec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/GameFramework/ActorHandle.hpp"
#include "Engine/GameFramework/Actor.hpp"
#include <string>
#include <vector>
#include "Widget.hpp"
#include "GameCommon.hpp"
#include "memory"

//-----------------------------------------------------------------------------------------------
class Camera;
class Clock;
class Prop;
class PlayerController;
class Character;
class Primitive;
class Shader;
class Texture;
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

    // ImGUI
    void        DrawDebugUI();
    void        DrawMenuBar();
    void        DrawRenderPanel();

    void        LoadAndRegisterTexture( char const* imageFilePath, std::string const& textureName );
    ActorHandle GenerateActorHandle( unsigned int actorIndex );
    Actor*      CreateActor( Actor* newActor );
    void        RegisterAllGameplayAbilities();
    void        InitHUD();
    void        RenderHUD() const;
    void        AddActorToGame( Actor* actor );

public:
    PlayerController*                 m_playerController = nullptr;

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
    std::unique_ptr< ConstantBuffer > m_lightCBO          = nullptr;
    Vec3                              m_sunDirection      = Vec3( 3.f, 1.0f, -2.0f );
    IntVec4                           m_sunColor          = IntVec4( 255, 255, 255, 255 );
    float                             m_shadowHalfSize    = 20.f;
    float                             m_shadowNear        = 0.1f;
    float                             m_shadowFar         = 50.f;
    float                             m_lightViewDistance = 10.f;

    // test
    Texture*                          m_fireballTexture      = nullptr;
    SpriteSheet*                      m_animSpriteSheet      = nullptr;
    SpriteAnimDefinition*             m_spriteAnimDefinition = nullptr;
    // ParticleEmitter*                m_particleEmitter      = nullptr;
    std::vector< Vertex >             m_testVerts;
    std::vector< unsigned int >       m_testIndices;
    bool                              m_showDebugMode = false;
    std::vector< ParticleEmitter* >   m_particleEmitters;
    Shader*                           m_vfx = nullptr;

private:
    Camera*                m_screenCamera;
    std::vector< Widget* > m_widgets;
    float                  m_bloomThreshold = 1.0f;
    float                  m_exposure       = 1.0f;
    float                  m_bloomIntensity = 0.3f;
};