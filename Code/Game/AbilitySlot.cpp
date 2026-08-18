#include "Game/AbilitySlot.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Texture.hpp"

//-----------------------------------------------------------------------------------------------
AbilitySlot::AbilitySlot( int slotIndex )
    : m_slotIndex( slotIndex )
{
}

//-----------------------------------------------------------------------------------------------
void AbilitySlot::Render() const
{
    float centerX  = g_engine->m_window->GetClientDimensions().x * 0.5f;
    float slotMinX = centerX - 302.f + 94.f * static_cast< float >( m_slotIndex );
    if ( m_slotIndex >= 3 ) slotMinX += 44.f;
    std::vector< Vertex > vertices;

    g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
    g_engine->m_render->SetSamplerMode( SamplerMode::BILINEAR_CLAMP, ShaderResourceSlot::DIFFUSE );

    AddVertsForAABB2D( vertices, AABB2( slotMinX + 26.f, 10.f, slotMinX + 64.f, 48.f ), Rgba8( 150, 35, 25 ), AABB2::ZERO_TO_ONE );
    Texture* keyBindingBackground = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_HUD_DarkFantasy_Box_Medium_Parchment.png" );
    g_engine->m_render->BindTexture( keyBindingBackground );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( slotMinX + 5.f, 55.f, slotMinX + 85.f, 135.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* slotUnderlay = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/ICON_DarkFantasy_Map_Lock_Underlay.png" );
    g_engine->m_render->BindTexture( slotUnderlay );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    AddVertsForAABB2D( vertices, AABB2( slotMinX, 50.f, slotMinX + 90.f, 140.f ), Rgba8::WHITE, AABB2::ZERO_TO_ONE );
    Texture* slotFrame = g_engine->m_render->CreateOrGetTextureFromFile( "Data/UI/SPR_DarkFantasy_Frame_Box_Medium.png" );
    g_engine->m_render->BindTexture( slotFrame );
    g_engine->m_render->DrawVertexArray( vertices );

    vertices.clear();
    BitmapFont* font = g_engine->m_render->m_loadedFontsByName[ "DefaultFont" ];
    font->AddVertsForTextInBox2D( vertices, std::to_string( m_slotIndex + 1 ), AABB2( slotMinX + 26.f, 10.f, slotMinX + 64.f, 48.f ), 26.f, Rgba8( 45, 10, 8 ), 0.75f );
    g_engine->m_render->BindTexture( &font->GetTexture() );
    g_engine->m_render->DrawVertexArray( vertices );
    g_engine->m_render->BindTexture( nullptr );
}
