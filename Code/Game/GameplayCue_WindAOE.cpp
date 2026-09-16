#include "GameplayCue_WindAOE.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/VFX/SpriteSheetEffect.hpp"

//-----------------------------------------------------------------------------------------------
GameplayCue_WindAOE::GameplayCue_WindAOE()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayCue_WindAOE::HandleGameplayCue( [[maybe_unused]] GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    if ( !params.m_target )
    {
        return;
    }

    Vec3 const         windPosition     = params.m_target->m_position;
    char const*        indicatorTexture = "Data/Images/WindIndicator.png";
    char const*        vortexTexture    = "Data/Textures/VFX/7.png";
    char const*        windTexture      = "Data/Textures/VFX/PolygonParticles_Wind.png";
    char const*        arcTexture       = "Data/Textures/VFX/PolygonParticles_HalfCircle.png";
    char const*        sparkleTexture   = "Data/Textures/VFX/PolygonParticles_Sparkle.png";

    // Wind indicator
    SpriteSheetEffect* groundCircle   = new SpriteSheetEffect( indicatorTexture, IntVec2( 1, 1 ), 0, 0, 1.f, SpriteAnimPlaybackType::ONCE );
    groundCircle->m_position          = windPosition + Vec3( 0.f, 0.f, 0.03f );
    groundCircle->m_size              = Vec2( 5.f, 5.f );
    groundCircle->m_tint              = Rgba8( 255, 225, 80, 220 );
    groundCircle->m_lifeSpan          = 2.2f;
    groundCircle->m_emissiveIntensity = 1.f;
    groundCircle->m_renderMode        = SpriteSheetEffectRenderMode::GROUND_PLANE;
    groundCircle->m_blendMode         = SpriteSheetEffectBlendMode::ADDITIVE;
    g_engine->m_vfxSystem->SpawnSpriteSheetEffect( groundCircle );

    // Main airborne vortex
    ParticleEmitterConfig vortexConfig;
    vortexConfig.m_position          = windPosition + Vec3( 0.f, 0.f, 0.65f );
    vortexConfig.m_spawnOffsetXRange = FloatRange( -0.15f, 0.15f );
    vortexConfig.m_spawnOffsetYRange = FloatRange( -0.15f, 0.15f );
    vortexConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.15f );
    vortexConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    vortexConfig.m_pitchRange        = FloatRange( 0.f, 0.f );
    vortexConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    vortexConfig.m_speedRange        = FloatRange( 0.f, 0.f );
    vortexConfig.m_lifetimeRange     = FloatRange( 0.55f, 0.75f );
    vortexConfig.m_startSizeRange    = FloatRange( 0.9f, 1.2f );
    vortexConfig.m_endSizeRange      = FloatRange( 2.4f, 3.f );
    vortexConfig.m_sizeAspect        = Vec2( 1.f, 1.f );
    vortexConfig.m_billboardType     = BillboardType::FULL_FACING;
    vortexConfig.m_startColor        = Rgba8( 255, 235, 125, 165 );
    vortexConfig.m_endColor          = Rgba8( 210, 155, 20, 0 );
    vortexConfig.m_texturePath       = vortexTexture;
    vortexConfig.m_blendMode         = BlendMode::ADDITIVE;
    vortexConfig.m_emissiveIntensity = 0.7f;

    ParticleEmitter* mainVortex = new ParticleEmitter( vortexConfig );
    mainVortex->SpawnBurst( 2 );
    g_engine->m_particleSystem->Spawn( mainVortex );

    // Secondary vortex shapes around the character
    ParticleEmitterConfig secondaryVortexConfig;
    secondaryVortexConfig.m_position          = windPosition + Vec3( 0.f, 0.f, 0.4f );
    secondaryVortexConfig.m_spawnOffsetXRange = FloatRange( -1.15f, 1.15f );
    secondaryVortexConfig.m_spawnOffsetYRange = FloatRange( -1.15f, 1.15f );
    secondaryVortexConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.55f );
    secondaryVortexConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    secondaryVortexConfig.m_pitchRange        = FloatRange( -55.f, -25.f );
    secondaryVortexConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    secondaryVortexConfig.m_speedRange        = FloatRange( 0.5f, 1.3f );
    secondaryVortexConfig.m_lifetimeRange     = FloatRange( 0.4f, 0.7f );
    secondaryVortexConfig.m_startSizeRange    = FloatRange( 0.25f, 0.4f );
    secondaryVortexConfig.m_endSizeRange      = FloatRange( 0.65f, 1.f );
    secondaryVortexConfig.m_sizeAspect        = Vec2( 1.f, 1.f );
    secondaryVortexConfig.m_billboardType     = BillboardType::FULL_FACING;
    secondaryVortexConfig.m_startColor        = Rgba8( 255, 240, 155, 110 );
    secondaryVortexConfig.m_endColor          = Rgba8( 205, 155, 25, 0 );
    secondaryVortexConfig.m_texturePath       = vortexTexture;
    secondaryVortexConfig.m_blendMode         = BlendMode::ADDITIVE;
    secondaryVortexConfig.m_emissiveIntensity = 0.5f;

    ParticleEmitter* secondaryVortex = new ParticleEmitter( secondaryVortexConfig );
    secondaryVortex->SpawnBurst( 5 );
    g_engine->m_particleSystem->Spawn( secondaryVortex );

    // Configuration shared by the four tangential wind streams
    ParticleEmitterConfig streamConfig;
    streamConfig.m_spawnOffsetXRange = FloatRange( -0.18f, 0.18f );
    streamConfig.m_spawnOffsetYRange = FloatRange( -0.18f, 0.18f );
    streamConfig.m_spawnOffsetZRange = FloatRange( 0.05f, 0.65f );
    streamConfig.m_pitchRange        = FloatRange( -5.f, 8.f );
    streamConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    streamConfig.m_speedRange        = FloatRange( 3.2f, 5.f );
    streamConfig.m_lifetimeRange     = FloatRange( 0.35f, 0.6f );
    streamConfig.m_startSizeRange    = FloatRange( 0.35f, 0.55f );
    streamConfig.m_endSizeRange      = FloatRange( 0.65f, 1.f );
    streamConfig.m_sizeAspect        = Vec2( 1.3f, 0.42f );
    streamConfig.m_billboardType     = BillboardType::FULL_FACING;
    streamConfig.m_startColor        = Rgba8( 255, 235, 135, 155 );
    streamConfig.m_endColor          = Rgba8( 210, 160, 25, 0 );
    streamConfig.m_texturePath       = arcTexture;
    streamConfig.m_blendMode         = BlendMode::ADDITIVE;
    streamConfig.m_emissiveIntensity = 0.65f;

    // East side moving north
    streamConfig.m_position     = windPosition + Vec3( 1.35f, 0.f, 0.3f );
    streamConfig.m_yawRange     = FloatRange( 82.f, 98.f );
    ParticleEmitter* eastStream = new ParticleEmitter( streamConfig );
    eastStream->SpawnBurst( 5 );
    g_engine->m_particleSystem->Spawn( eastStream );

    // North side moving west
    streamConfig.m_position      = windPosition + Vec3( 0.f, 1.35f, 0.3f );
    streamConfig.m_yawRange      = FloatRange( 172.f, 188.f );
    ParticleEmitter* northStream = new ParticleEmitter( streamConfig );
    northStream->SpawnBurst( 5 );
    g_engine->m_particleSystem->Spawn( northStream );

    // West side moving south
    streamConfig.m_position     = windPosition + Vec3( -1.35f, 0.f, 0.3f );
    streamConfig.m_yawRange     = FloatRange( 262.f, 278.f );
    ParticleEmitter* westStream = new ParticleEmitter( streamConfig );
    westStream->SpawnBurst( 5 );
    g_engine->m_particleSystem->Spawn( westStream );

    // South side moving east
    streamConfig.m_position      = windPosition + Vec3( 0.f, -1.35f, 0.3f );
    streamConfig.m_yawRange      = FloatRange( -8.f, 8.f );
    ParticleEmitter* southStream = new ParticleEmitter( streamConfig );
    southStream->SpawnBurst( 5 );
    g_engine->m_particleSystem->Spawn( southStream );

    // Rising wind wisps
    ParticleEmitterConfig risingConfig;
    risingConfig.m_position          = windPosition + Vec3( 0.f, 0.f, 0.15f );
    risingConfig.m_spawnOffsetXRange = FloatRange( -1.5f, 1.5f );
    risingConfig.m_spawnOffsetYRange = FloatRange( -1.5f, 1.5f );
    risingConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.35f );
    risingConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    risingConfig.m_pitchRange        = FloatRange( -82.f, -55.f );
    risingConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    risingConfig.m_speedRange        = FloatRange( 1.2f, 2.8f );
    risingConfig.m_lifetimeRange     = FloatRange( 0.5f, 0.95f );
    risingConfig.m_startSizeRange    = FloatRange( 0.16f, 0.3f );
    risingConfig.m_endSizeRange      = FloatRange( 0.5f, 0.85f );
    risingConfig.m_sizeAspect        = Vec2( 1.5f, 0.42f );
    risingConfig.m_billboardType     = BillboardType::FULL_FACING;
    risingConfig.m_spawnRate         = 12.f;
    risingConfig.m_emissionDuration  = 2.f;
    risingConfig.m_startColor        = Rgba8( 255, 245, 170, 135 );
    risingConfig.m_endColor          = Rgba8( 210, 170, 35, 0 );
    risingConfig.m_texturePath       = windTexture;
    risingConfig.m_blendMode         = BlendMode::ALPHA;
    risingConfig.m_emissiveIntensity = 0.55f;

    ParticleEmitter* risingWind = new ParticleEmitter( risingConfig );
    g_engine->m_particleSystem->Spawn( risingWind );

    // Initial wind fragments
    ParticleEmitterConfig fragmentConfig;
    fragmentConfig.m_position          = windPosition + Vec3( 0.f, 0.f, 0.35f );
    fragmentConfig.m_spawnOffsetXRange = FloatRange( -0.25f, 0.25f );
    fragmentConfig.m_spawnOffsetYRange = FloatRange( -0.25f, 0.25f );
    fragmentConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.25f );
    fragmentConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    fragmentConfig.m_pitchRange        = FloatRange( -35.f, 12.f );
    fragmentConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    fragmentConfig.m_speedRange        = FloatRange( 3.5f, 6.5f );
    fragmentConfig.m_lifetimeRange     = FloatRange( 0.25f, 0.55f );
    fragmentConfig.m_startSizeRange    = FloatRange( 0.03f, 0.075f );
    fragmentConfig.m_endSizeRange      = FloatRange( 0.01f, 0.025f );
    fragmentConfig.m_billboardType     = BillboardType::FULL_FACING;
    fragmentConfig.m_startColor        = Rgba8( 255, 250, 195, 210 );
    fragmentConfig.m_endColor          = Rgba8( 210, 165, 30, 0 );
    fragmentConfig.m_texturePath       = sparkleTexture;
    fragmentConfig.m_blendMode         = BlendMode::ADDITIVE;
    fragmentConfig.m_emissiveIntensity = 1.35f;

    ParticleEmitter* windFragments = new ParticleEmitter( fragmentConfig );
    windFragments->SpawnBurst( 26 );
    g_engine->m_particleSystem->Spawn( windFragments );

    // Residual wind particles
    ParticleEmitterConfig residualConfig;
    residualConfig.m_position          = windPosition + Vec3( 0.f, 0.f, 0.25f );
    residualConfig.m_spawnOffsetXRange = FloatRange( -1.5f, 1.5f );
    residualConfig.m_spawnOffsetYRange = FloatRange( -1.5f, 1.5f );
    residualConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.6f );
    residualConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    residualConfig.m_pitchRange        = FloatRange( -55.f, 15.f );
    residualConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    residualConfig.m_speedRange        = FloatRange( 1.2f, 2.8f );
    residualConfig.m_lifetimeRange     = FloatRange( 0.4f, 0.75f );
    residualConfig.m_startSizeRange    = FloatRange( 0.025f, 0.055f );
    residualConfig.m_endSizeRange      = FloatRange( 0.01f, 0.02f );
    residualConfig.m_billboardType     = BillboardType::FULL_FACING;
    residualConfig.m_spawnRate         = 8.f;
    residualConfig.m_emissionDuration  = 1.6f;
    residualConfig.m_startColor        = Rgba8( 255, 245, 190, 140 );
    residualConfig.m_endColor          = Rgba8( 205, 165, 35, 0 );
    residualConfig.m_texturePath       = sparkleTexture;
    residualConfig.m_blendMode         = BlendMode::ADDITIVE;
    residualConfig.m_emissiveIntensity = 0.85f;

    ParticleEmitter* residualWind = new ParticleEmitter( residualConfig );
    g_engine->m_particleSystem->Spawn( residualWind );
}