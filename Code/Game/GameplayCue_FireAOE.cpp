#include "GameplayCue_FireAOE.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/VFX/SpriteSheetEffect.hpp"

//-----------------------------------------------------------------------------------------------
GameplayCue_FireAOE::GameplayCue_FireAOE()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayCue_FireAOE::HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    SpriteSheetEffect* groundCircle   = new SpriteSheetEffect( "Data/Textures/VFX/PolygonParticles_RitualCircle_01.png", IntVec2( 1, 1 ), 0, 0, 1.f, SpriteAnimPlaybackType::ONCE );
    groundCircle->m_position          = params.m_target->m_position + Vec3( 0.f, 0.f, 0.03f );
    groundCircle->m_size              = Vec2( 5.f, 5.f );
    groundCircle->m_tint              = Rgba8( 255, 80, 15, 210 );
    groundCircle->m_lifeSpan          = 2.5f;
    groundCircle->m_emissiveIntensity = 0.9f;
    groundCircle->m_renderMode        = SpriteSheetEffectRenderMode::GROUND_PLANE;
    groundCircle->m_blendMode         = SpriteSheetEffectBlendMode::ALPHA;
    g_engine->m_vfxSystem->SpawnSpriteSheetEffect( groundCircle );

    // Core
    ParticleEmitterConfig coreConfig;
    coreConfig.m_position          = params.m_target->m_position + Vec3( 0.f, 0.f, 0.25f );
    coreConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    coreConfig.m_pitchRange        = FloatRange( -75.f, -35.f );
    coreConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    coreConfig.m_speedRange        = FloatRange( 1.f, 3.f );
    coreConfig.m_lifetimeRange     = FloatRange( 0.25f, 0.5f );
    coreConfig.m_startSizeRange    = FloatRange( 0.7f, 1.1f );
    coreConfig.m_endSizeRange      = FloatRange( 1.4f, 2.2f );
    coreConfig.m_startColor        = Rgba8( 255, 245, 180, 255 );
    coreConfig.m_endColor          = Rgba8( 255, 50, 5, 0 );
    coreConfig.m_texturePath       = "Data/Textures/VFX/fire_01_a.png";
    coreConfig.m_blendMode         = BlendMode::ALPHA;
    coreConfig.m_emissiveIntensity = 1.2f;

    ParticleEmitter* coreExplosion = new ParticleEmitter( coreConfig );
    coreExplosion->SpawnBurst( 18 );
    g_engine->m_particleSystem->Spawn( coreExplosion );

    // Ground Fire
    ParticleEmitterConfig groundFireConfig;
    groundFireConfig.m_position          = params.m_target->m_position + Vec3( 0.f, 0.f, 0.08f );
    groundFireConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    groundFireConfig.m_pitchRange        = FloatRange( -8.f, 4.f );
    groundFireConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    groundFireConfig.m_speedRange        = FloatRange( 4.f, 7.f );
    groundFireConfig.m_lifetimeRange     = FloatRange( 0.3f, 0.65f );
    groundFireConfig.m_startSizeRange    = FloatRange( 0.25f, 0.5f );
    groundFireConfig.m_endSizeRange      = FloatRange( 0.8f, 1.4f );
    groundFireConfig.m_startColor        = Rgba8( 255, 210, 70, 230 );
    groundFireConfig.m_endColor          = Rgba8( 255, 30, 0, 0 );
    groundFireConfig.m_texturePath       = "Data/Textures/VFX/FireCloudParticle.png";
    groundFireConfig.m_blendMode         = BlendMode::ALPHA;
    groundFireConfig.m_emissiveIntensity = 1.8f;

    ParticleEmitter* groundFire = new ParticleEmitter( groundFireConfig );
    groundFire->SpawnBurst( 28 );
    g_engine->m_particleSystem->Spawn( groundFire );

    // FlameColumn
    ParticleEmitterConfig flameColumnConfig;
    flameColumnConfig.m_position          = params.m_target->m_position + Vec3( 0.f, 0.f, 0.15f );
    flameColumnConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    flameColumnConfig.m_pitchRange        = FloatRange( -88.f, -68.f );
    flameColumnConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    flameColumnConfig.m_speedRange        = FloatRange( 3.f, 6.f );
    flameColumnConfig.m_lifetimeRange     = FloatRange( 0.45f, 0.9f );
    flameColumnConfig.m_startSizeRange    = FloatRange( 0.35f, 0.65f );
    flameColumnConfig.m_endSizeRange      = FloatRange( 0.8f, 1.35f );
    flameColumnConfig.m_startColor        = Rgba8( 255, 235, 120, 240 );
    flameColumnConfig.m_endColor          = Rgba8( 255, 45, 0, 0 );
    flameColumnConfig.m_texturePath       = "Data/Textures/VFX/flame_02_a.png";
    flameColumnConfig.m_blendMode         = BlendMode::ALPHA;
    flameColumnConfig.m_emissiveIntensity = 1.1f;

    ParticleEmitter* flameColumn = new ParticleEmitter( flameColumnConfig );
    flameColumn->SpawnBurst( 16 );
    g_engine->m_particleSystem->Spawn( flameColumn );

    //Spark
    ParticleEmitterConfig sparkConfig;
    sparkConfig.m_position          = params.m_target->m_position + Vec3( 0.f, 0.f, 0.3f );
    sparkConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    sparkConfig.m_pitchRange        = FloatRange( -55.f, 5.f );
    sparkConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    sparkConfig.m_speedRange        = FloatRange( 6.f, 12.f );
    sparkConfig.m_lifetimeRange     = FloatRange( 0.2f, 0.6f );
    sparkConfig.m_startSizeRange    = FloatRange( 0.05f, 0.14f );
    sparkConfig.m_endSizeRange      = FloatRange( 0.01f, 0.03f );
    sparkConfig.m_startColor        = Rgba8( 255, 245, 130, 255 );
    sparkConfig.m_endColor          = Rgba8( 255, 50, 0, 0 );
    sparkConfig.m_texturePath       = "Data/Textures/VFX/PolygonParticles_Sparkle.png";
    sparkConfig.m_blendMode         = BlendMode::ADDITIVE;
    sparkConfig.m_emissiveIntensity = 4.f;

    ParticleEmitter* sparks = new ParticleEmitter( sparkConfig );
    sparks->SpawnBurst( 42 );
    g_engine->m_particleSystem->Spawn( sparks );

    //Smoke
    ParticleEmitterConfig smokeConfig;
    smokeConfig.m_position          = params.m_target->m_position + Vec3( 0.f, 0.f, 0.35f );
    smokeConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    smokeConfig.m_pitchRange        = FloatRange( -85.f, -60.f );
    smokeConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    smokeConfig.m_speedRange        = FloatRange( 0.3f, 0.8f );
    smokeConfig.m_lifetimeRange     = FloatRange( 1.f, 1.8f );
    smokeConfig.m_startSizeRange    = FloatRange( 0.4f, 0.8f );
    smokeConfig.m_endSizeRange      = FloatRange( 1.2f, 2.2f );
    smokeConfig.m_spawnRate         = 4.f;
    smokeConfig.m_emissionDuration  = 0.8f;
    smokeConfig.m_startColor        = Rgba8( 70, 55, 45, 100 );
    smokeConfig.m_endColor          = Rgba8( 25, 20, 20, 0 );
    smokeConfig.m_texturePath       = "Data/Textures/VFX/SmokeParticle.png";
    smokeConfig.m_blendMode         = BlendMode::ALPHA;
    smokeConfig.m_emissiveIntensity = 1.f;

    ParticleEmitter* smoke = new ParticleEmitter( smokeConfig );
    g_engine->m_particleSystem->Spawn( smoke );
}