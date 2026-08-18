#include "Game/ManaOrb.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Engine.hpp"

ManaOrb::ManaOrb()
{
}

void ManaOrb::Render() const
{
    float centerX = g_engine->m_window->GetClientDimensions().x * 0.5f;
    float orbX = centerX + 400.f;
    std::vector< Vertex > vertices;

    g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
    g_engine->m_render->SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ShaderResourceSlot::DIFFUSE );

    AddVertsForDisc2D( vertices, Vec2( orbX, 100.f ), 72.f, Rgba8( 20, 45, 150 ) );
    g_engine->m_render->BindTexture( nullptr );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( orbX - 90.f, 10.f, orbX + 90.f, 190.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* frameOrbSheen = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Frame_Orb_Sheen.png" );
    g_engine->m_render->BindTexture( frameOrbSheen );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( orbX - 100.f, 0.f, orbX + 100.f, 200.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* frameOrbRight = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Frame_Orb_Right.png" );
    g_engine->m_render->BindTexture( frameOrbRight );
    g_engine->m_render->DrawVertexArray( vertices );
    g_engine->m_render->BindTexture( nullptr );
}
