#include "Game/Projectile.hpp"
#include "Game/Game.hpp"
#include "Game/PlayerController.hpp"
#include "Game/StaticMeshDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/MovementComponent.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include <Engine/Core/Clock.hpp>
#include "Engine/Core/VertexUtils.hpp"
#include <Engine/Math/MathUtils.hpp>
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <Engine/GameFramework/StaticMeshComponent.hpp>

//-----------------------------------------------------------------------------------------------
Projectile::Projectile( ProjectileDefinition const& projectileDef, Game* game )
    : m_game( game )
    , m_projectileDef( projectileDef )
    , m_deathTimer( projectileDef.m_duration, m_game->m_clock )
{
    if ( m_projectileDef.m_visualType == ProjectileVisualType::BILLBOARD )
    {
        Texture* spriteSheetTexture = g_engine->m_render->CreateOrGetTextureFromFile( m_projectileDef.m_spriteSheetPath.c_str(), true );
        m_spriteSheet               = new SpriteSheet( *spriteSheetTexture, m_projectileDef.m_gridLayout );
        m_spriteAnimDefinition      = new SpriteAnimDefinition( *m_spriteSheet, m_projectileDef.m_startFrame, m_projectileDef.m_endFrame, m_projectileDef.m_framePerSecond, SpriteAnimPlaybackType::LOOP );
    }
    else
    {
        m_staticMeshDef = &StaticMeshDefinition::GetDefinitionById( projectileDef.m_staticMeshId );
        GUARANTEE_OR_DIE( m_staticMeshDef, "StaticMeshDefinition is not found" );
        StaticModel const&   staticModel         = g_engine->m_modelAssets->CreateOrGetStaticModel( m_staticMeshDef->m_filePath );
        StaticMeshComponent* staticMeshComponent = new StaticMeshComponent( this, staticModel );
        AddComponent( staticMeshComponent );
        m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_staticMeshDef->m_axes.at( "x" ), m_staticMeshDef->m_axes.at( "y" ), m_staticMeshDef->m_axes.at( "z" ) );
    }

    m_movementComponent = new MovementComponent( this );
    AddComponent( m_movementComponent );

    m_deathTimer.Start();
}

//-----------------------------------------------------------------------------------------------
Projectile::~Projectile()
{
    SAFE_RELEASE( m_spriteSheet );
    SAFE_RELEASE( m_spriteAnimDefinition );
}

//-----------------------------------------------------------------------------------------------
void Projectile::Update()
{
    if ( m_deathTimer.HasPeriodElapsed() )
    {
        m_isDead = true;
    }

    m_position += m_movementComponent->m_velocity * static_cast< float >( Clock::GetSystemClock().GetDeltaSeconds() );
}

//-----------------------------------------------------------------------------------------------
void Projectile::Render() const
{
    float elapsedTime = static_cast< float >( m_deathTimer.GetElapsedTime() );
    if ( m_projectileDef.m_visualType == ProjectileVisualType::BILLBOARD )
    {
        std::vector< Vertex > verts;
        SpriteDefinition      spriteDef = m_spriteAnimDefinition->GetSpriteDefAtTime( elapsedTime );
        AddVertsForQuad3D( verts, Vec3( 0.0f, -0.5f, -0.5f ), Vec3( 0.0f, 0.5f, -0.5f ), Vec3( 0.0f, 0.5f, 0.5f ), Vec3( 0.0f, -0.5f, 0.5f ), Rgba8::WHITE, spriteDef.GetUVs() );

        Mat44 transform = GetBillboard( BillboardType::FULL_OPPOSING, m_game->m_playerController->m_worldCamera->GetCameraToWorldTransform(), m_position, Vec2( 2.95f, 2.f ) );
        g_engine->m_render->BindShader( ShaderType::VFX );
        g_engine->m_render->BindTextureWithSampler( { &m_spriteSheet->GetTexture(), SamplerMode::BILINEAR_CLAMP, ShaderResourceSlot::DIFFUSE } );
        g_engine->m_render->SetMaterialConstants( 0.f, 0.5f, 1.f, m_game->m_indicatorIntensity );
        g_engine->m_render->SetBlendMode( BlendMode::ALPHA );
        g_engine->m_render->SetDepthMode( DepthMode::READ_ONLY_LESS_EQUAL );
        g_engine->m_render->SetModelConstants( transform, Rgba8::WHITE );

        g_engine->m_render->DrawVertexArray( verts );

        g_engine->m_render->UnbindTexture( ShaderResourceSlot::DIFFUSE );
        g_engine->m_render->SetMaterialConstants();
        g_engine->m_render->SetDepthMode( DepthMode::READ_WRITE_LESS_EQUAL );
        g_engine->m_render->SetBlendMode( BlendMode::OPAQUE );
        g_engine->m_render->BindShader( ShaderType::Default );
    }
    else
    {
        g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
        g_engine->m_render->SetMaterialConstants( m_staticMeshDef->m_metallic, m_staticMeshDef->m_roughness, m_staticMeshDef->m_ambientOcclusion, m_staticMeshDef->m_emissiveIntensity );
        Actor::Render();
        g_engine->m_render->UnbindPBRTextures();
    }
}

Mat44 Projectile::GetModelToWorldTransform() const
{
    Mat44 transform = Actor::GetModelToWorldTransform();
    transform.Append( m_toEngineMatrix );
    return transform;
}