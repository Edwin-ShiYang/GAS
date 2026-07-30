#include "Game/Game.hpp"
#include "Game/Arrow.hpp"
#include "Game/Actor.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Arrow::Arrow( Game* owner, Rgba8 const& color )
    : Prop( owner )
{
    m_color    = color;
    Vec3 start = Vec3( 0.f, 0.f, 4.f );
    Vec3 end   = Vec3( 0.f, 0.f, 4.5f );

    AddVertsForArrow3D( m_vertexes, start, end, 0.2f, color );
}

//-----------------------------------------------------------------------------------------------
Arrow::~Arrow()
{
}

//-----------------------------------------------------------------------------------------------
void Arrow::Update()
{
    Prop::Update();
}

//-----------------------------------------------------------------------------------------------
void Arrow::Render() const
{
    Prop::Render();
}