#include "Game/Cylinder.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Cylinder::Cylinder( Game* owner, float start, float end, float m_radius )
    : Primitive( owner )
    , m_start( start )
    , m_end( end )
    , m_radius( m_radius )
{
    AddVertsForCylinder3D( m_vertices, m_indices, Vec2::ZERO, FloatRange( m_start, m_end ), m_radius, 32.f );
    CreateVertexBuffer();
    CreateIndexBuffer();

    m_diffuse = g_engine->m_render->CreateOrGetTextureFromFile( CYLINDER_TEXTURE );
}

//-----------------------------------------------------------------------------------------------
Cylinder::~Cylinder()
{
}

//-----------------------------------------------------------------------------------------------
void Cylinder::Update()
{
    Primitive::Update();
}

//-----------------------------------------------------------------------------------------------
void Cylinder::Render() const
{
    Primitive::Render();
}