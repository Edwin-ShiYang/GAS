#include "Game/Game.hpp"
#include "Game/Cube.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Cube::Cube( Game* owner )
    : Primitive( owner )
{
    AddVertsForAABB3D( m_vertices, m_indices, AABB3( Vec3( -0.5f, -0.5f, -0.5f ), Vec3( 0.5f, 0.5f, 0.5f ) ), Rgba8::WHITE, AABB2( 0.f, 0.f, 8.f, 8.f ) );
    CreateVertexBuffer();
    CreateIndexBuffer();

    m_diffuse = g_engine->m_render->CreateOrGetTextureFromFile( CUBE_TEXTURE );
}

//-----------------------------------------------------------------------------------------------
Cube::~Cube()
{
}

//-----------------------------------------------------------------------------------------------
void Cube::Update()
{
    Primitive::Update();
}

//-----------------------------------------------------------------------------------------------
void Cube::Render() const
{
    g_engine->m_render->SetSamplerMode( SamplerMode::BILINEAR_WRAP, ResourceSlot::DIFFUSE );
    Primitive::Render();
    g_engine->m_render->SetSamplerMode( SamplerMode::POINT_CLAMP, ResourceSlot::DIFFUSE );
}