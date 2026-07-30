#include "Game/Game.hpp"
#include "Game/Sphere.hpp"
#include "Game/Actor.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Sphere::Sphere( Game* owner, Rgba8 const& color )
    : Prop( owner )
{
    AddVertsForUVSphereZ3D( m_vertexes, m_position, 1.f, 32.f, 16.f, color );
}

//-----------------------------------------------------------------------------------------------
Sphere::~Sphere()
{
}

//-----------------------------------------------------------------------------------------------
void Sphere::Update()
{
    Prop::Update();
}

//-----------------------------------------------------------------------------------------------
void Sphere::Render() const
{
    g_engine->m_render->BindTexture( m_texture );
    Prop::Render();
    g_engine->m_render->BindTexture( nullptr );
}