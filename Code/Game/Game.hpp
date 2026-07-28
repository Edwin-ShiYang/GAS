#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/IntVec4.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
class Camera;
class Clock;
class Actor;
class PlayerController;
class Primitive;
class Shader;
class Texture;
class SpriteSheet;
class SpriteAnimDefinition;
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

    void BeginFrame();
    void EndFrame();

    void Update();
    void UpdateFromKeyboard();
    void UpdateFromController();
    void UpdateCameras();

    void Render() const;
    void SetLightConstants() const;

private:
    void RenderAttractMode() const;
    void UpdateAttractMode();

    void UpdateActors();

    void RenderActors() const;
    void RenderProps() const;

    void DestroyProps();
    void DestroyEntities();

    void UpdateImGUI();
    void LoadAndRegisterTexture( char const* imageFilePath, std::string const& textureName );

public:
    PlayerController*           m_playerController = nullptr;

    Clock*                      m_clock           = nullptr;
    GameMode                    m_nextGameMode    = GAME_MODE_ATTRACT;
    GameMode                    m_currentGameMode = GAME_MODE_ATTRACT;

    VertexBuffer*               m_vertexBuffer = nullptr;
    IndexBuffer*                m_indexBuffer  = nullptr;

    std::vector< Actor* >       m_actors;
    std::vector< Primitive* >   m_primitives;

    std::vector< Vertex >       m_verts;

    std::vector< Vertex >       m_testVerts;
    std::vector< unsigned int > m_testIndices;

    // light
    ConstantBuffer*             m_lightCBO = nullptr;
    ConstantBuffer*             m_skinCBO  = nullptr;

    Vec3                        m_sunDirection      = Vec3( 3.f, 1.0f, -2.0f );
    IntVec4                     m_sunColor          = IntVec4( 255, 255, 255, 255 );
    float                       m_shadowHalfSize    = 20.f;
    float                       m_shadowNear        = 0.1f;
    float                       m_shadowFar         = 50.f;
    float                       m_lightViewDistance = 10.f;

    Texture*                    m_fireballTexture      = nullptr;
    SpriteSheet*                m_animSpriteSheet      = nullptr;
    SpriteAnimDefinition*       m_spriteAnimDefinition = nullptr;

private:
    Camera*
        m_screenCamera;
};