#include "Game/Cone.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Engine.hpp"


//-----------------------------------------------------------------------------------------------
Cone::Cone( Game* owner, Vec3 const& start, Vec3 const& end, float m_radius, Rgba8 const& color )
	: Prop( owner, color )
	, m_start( start )
	, m_end( end )
	, m_radius( m_radius )
{
	AddVertsForCone3D( m_vertexes, m_start, m_end, m_radius, color, AABB2::ZERO_TO_ONE );
}


//-----------------------------------------------------------------------------------------------
Cone::~Cone()
{
}


//-----------------------------------------------------------------------------------------------
void Cone::Update()
{
	Prop::Update();
}


//-----------------------------------------------------------------------------------------------
void Cone::Render() const
{
	g_engine->m_render->BindTexture( m_texture );
	Prop::Render();
	g_engine->m_render->BindTexture( nullptr );
}