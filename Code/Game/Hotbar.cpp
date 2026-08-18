#include "Game/Hotbar.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"

//-----------------------------------------------------------------------------------------------
Hotbar::Hotbar()
{
}

//-----------------------------------------------------------------------------------------------
void Hotbar::Render() const
{
    float                 centerX = g_engine->m_window->GetClientDimensions().x * 0.5f;
    std::vector< Vertex > vertices;

    g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
    g_engine->m_render->SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ShaderResourceSlot::DIFFUSE );

    Texture* greebleWings = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Greeble_Wings_01.png" );

    AddVertsForAABB2D( vertices, AABB2( centerX - 620.f, 20.f, centerX - 450.f, 190.f ), Rgba8::WHITE, AABB2( 0.f, 0.f, 0.5f, 1.f ) );
    g_engine->m_render->BindTexture( greebleWings );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( centerX + 450.f, 20.f, centerX + 620.f, 190.f ), Rgba8::WHITE, AABB2( 0.5f, 0.f, 1.f, 1.f ) );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( centerX - 500.f, 0.f, centerX, 200.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* greebleShieldLeft = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Greeble_Shield_Left.png" );
    g_engine->m_render->BindTexture( greebleShieldLeft );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( centerX, 0.f, centerX + 500.f, 200.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* greebleShieldRight = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Greeble_Shield_Right.png" );
    g_engine->m_render->BindTexture( greebleShieldRight );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( centerX - 24.f, 155.f, centerX + 26.f, 205.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* greebleSkull = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Greeble_Skull.png" );
    g_engine->m_render->BindTexture( greebleSkull );
    g_engine->m_render->DrawVertexArray( vertices );

    Texture* greebleSpikes = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Greeble_Spikes.png" );
    g_engine->m_render->BindTexture( greebleSpikes );
    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( centerX - 24.f, 45.f, centerX + 24.f, 145.f ), Rgba8::WHITE, AABB2( 0.2f, 0.f, 0.8f, 1.f ) );
    g_engine->m_render->DrawVertexArray( vertices );

    g_engine->m_render->BindTexture( nullptr );
}
