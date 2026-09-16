#include "GameplayCue_FrostAOE.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/VFX/SpriteSheetEffect.hpp"

//-----------------------------------------------------------------------------------------------
GameplayCue_FrostAOE::GameplayCue_FrostAOE()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayCue_FrostAOE::HandleGameplayCue( [[maybe_unused]] GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    if ( !params.m_target )
    {
        return;
    }

    Vec3 const         frostPosition    = params.m_target->m_position;
    char const*        indicatorTexture = "Data/Images/FrostIndicator.png";
    char const*        frostTexture     = "Data/Textures/VFX/44.png";
    char const*        sparkleTexture   = "Data/Textures/VFX/PolygonParticles_Sparkle.png";
    char const*        ringTexture      = "Data/Textures/VFX/PolygonParticles_Ring_02.png";

    // Frost indicator
    SpriteSheetEffect* groundCircle   = new SpriteSheetEffect( indicatorTexture, IntVec2( 1, 1 ), 0, 0, 1.f, SpriteAnimPlaybackType::ONCE );
    groundCircle->m_position          = frostPosition + Vec3( 0.f, 0.f, 0.03f );
    groundCircle->m_size              = Vec2( 5.f, 5.f );
    groundCircle->m_tint              = Rgba8( 100, 210, 255, 220 );
    groundCircle->m_lifeSpan          = 1.6f;
    groundCircle->m_emissiveIntensity = 1.f;
    groundCircle->m_renderMode        = SpriteSheetEffectRenderMode::GROUND_PLANE;
    groundCircle->m_blendMode         = SpriteSheetEffectBlendMode::ADDITIVE;
    g_engine->m_vfxSystem->SpawnSpriteSheetEffect( groundCircle );

    // Short ground shock ring
    SpriteSheetEffect* shockRing   = new SpriteSheetEffect( ringTexture, IntVec2( 1, 1 ), 0, 0, 1.f, SpriteAnimPlaybackType::ONCE );
    shockRing->m_position          = frostPosition + Vec3( 0.f, 0.f, 0.05f );
    shockRing->m_size              = Vec2( 4.3f, 4.3f );
    shockRing->m_tint              = Rgba8( 90, 200, 255, 180 );
    shockRing->m_lifeSpan          = 0.4f;
    shockRing->m_emissiveIntensity = 1.2f;
    shockRing->m_renderMode        = SpriteSheetEffectRenderMode::GROUND_PLANE;
    shockRing->m_blendMode         = SpriteSheetEffectBlendMode::ADDITIVE;
    g_engine->m_vfxSystem->SpawnSpriteSheetEffect( shockRing );

    // Main frost bloom
    ParticleEmitterConfig bloomConfig;
    bloomConfig.m_position          = frostPosition + Vec3( 0.f, 0.f, 0.65f );
    bloomConfig.m_spawnOffsetXRange = FloatRange( 0.f, 0.f );
    bloomConfig.m_spawnOffsetYRange = FloatRange( 0.f, 0.f );
    bloomConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.f );
    bloomConfig.m_speedRange        = FloatRange( 0.f, 0.f );
    bloomConfig.m_lifetimeRange     = FloatRange( 0.55f, 0.55f );
    bloomConfig.m_startSizeRange    = FloatRange( 0.8f, 0.8f );
    bloomConfig.m_endSizeRange      = FloatRange( 3.4f, 3.4f );
    bloomConfig.m_sizeAspect        = Vec2( 1.f, 1.f );
    bloomConfig.m_billboardType     = BillboardType::FULL_FACING;
    bloomConfig.m_startColor        = Rgba8( 130, 220, 255, 210 );
    bloomConfig.m_endColor          = Rgba8( 35, 100, 220, 0 );
    bloomConfig.m_texturePath       = frostTexture;
    bloomConfig.m_blendMode         = BlendMode::ADDITIVE;
    bloomConfig.m_emissiveIntensity = 0.9f;

    ParticleEmitter* frostBloom = new ParticleEmitter( bloomConfig );
    frostBloom->SpawnBurst( 1 );
    g_engine->m_particleSystem->Spawn( frostBloom );

    // Secondary frost blooms
    ParticleEmitterConfig secondaryBloomConfig;
    secondaryBloomConfig.m_position          = frostPosition + Vec3( 0.f, 0.f, 0.45f );
    secondaryBloomConfig.m_spawnOffsetXRange = FloatRange( -1.4f, 1.4f );
    secondaryBloomConfig.m_spawnOffsetYRange = FloatRange( -1.4f, 1.4f );
    secondaryBloomConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.7f );
    secondaryBloomConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    secondaryBloomConfig.m_pitchRange        = FloatRange( -70.f, -35.f );
    secondaryBloomConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    secondaryBloomConfig.m_speedRange        = FloatRange( 0.5f, 1.6f );
    secondaryBloomConfig.m_lifetimeRange     = FloatRange( 0.45f, 0.85f );
    secondaryBloomConfig.m_startSizeRange    = FloatRange( 0.25f, 0.45f );
    secondaryBloomConfig.m_endSizeRange      = FloatRange( 0.65f, 1.1f );
    secondaryBloomConfig.m_sizeAspect        = Vec2( 1.f, 1.f );
    secondaryBloomConfig.m_billboardType     = BillboardType::FULL_FACING;
    secondaryBloomConfig.m_startColor        = Rgba8( 120, 215, 255, 190 );
    secondaryBloomConfig.m_endColor          = Rgba8( 30, 100, 220, 0 );
    secondaryBloomConfig.m_texturePath       = frostTexture;
    secondaryBloomConfig.m_blendMode         = BlendMode::ADDITIVE;
    secondaryBloomConfig.m_emissiveIntensity = 0.75f;

    ParticleEmitter* secondaryBlooms = new ParticleEmitter( secondaryBloomConfig );
    secondaryBlooms->SpawnBurst( 10 );
    g_engine->m_particleSystem->Spawn( secondaryBlooms );

    // Fast radial ice fragments
    ParticleEmitterConfig fragmentConfig;
    fragmentConfig.m_position          = frostPosition + Vec3( 0.f, 0.f, 0.35f );
    fragmentConfig.m_spawnOffsetXRange = FloatRange( -0.3f, 0.3f );
    fragmentConfig.m_spawnOffsetYRange = FloatRange( -0.3f, 0.3f );
    fragmentConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.3f );
    fragmentConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    fragmentConfig.m_pitchRange        = FloatRange( -40.f, 10.f );
    fragmentConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    fragmentConfig.m_speedRange        = FloatRange( 5.f, 10.f );
    fragmentConfig.m_lifetimeRange     = FloatRange( 0.25f, 0.55f );
    fragmentConfig.m_startSizeRange    = FloatRange( 0.04f, 0.11f );
    fragmentConfig.m_endSizeRange      = FloatRange( 0.01f, 0.03f );
    fragmentConfig.m_sizeAspect        = Vec2( 0.4f, 1.f );
    fragmentConfig.m_billboardType     = BillboardType::FULL_FACING;
    fragmentConfig.m_startColor        = Rgba8( 210, 245, 255, 230 );
    fragmentConfig.m_endColor          = Rgba8( 45, 130, 255, 0 );
    fragmentConfig.m_texturePath       = sparkleTexture;
    fragmentConfig.m_blendMode         = BlendMode::ADDITIVE;
    fragmentConfig.m_emissiveIntensity = 1.7f;

    ParticleEmitter* iceFragments = new ParticleEmitter( fragmentConfig );
    iceFragments->SpawnBurst( 34 );
    g_engine->m_particleSystem->Spawn( iceFragments );

    // Rising frost particles
    ParticleEmitterConfig risingConfig;
    risingConfig.m_position          = frostPosition + Vec3( 0.f, 0.f, 0.15f );
    risingConfig.m_spawnOffsetXRange = FloatRange( -1.7f, 1.7f );
    risingConfig.m_spawnOffsetYRange = FloatRange( -1.7f, 1.7f );
    risingConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.25f );
    risingConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    risingConfig.m_pitchRange        = FloatRange( -82.f, -58.f );
    risingConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    risingConfig.m_speedRange        = FloatRange( 1.f, 2.5f );
    risingConfig.m_lifetimeRange     = FloatRange( 0.5f, 0.95f );
    risingConfig.m_startSizeRange    = FloatRange( 0.035f, 0.09f );
    risingConfig.m_endSizeRange      = FloatRange( 0.015f, 0.04f );
    risingConfig.m_billboardType     = BillboardType::FULL_FACING;
    risingConfig.m_spawnRate         = 20.f;
    risingConfig.m_emissionDuration  = 1.2f;
    risingConfig.m_startColor        = Rgba8( 190, 235, 255, 210 );
    risingConfig.m_endColor          = Rgba8( 50, 130, 255, 0 );
    risingConfig.m_texturePath       = sparkleTexture;
    risingConfig.m_blendMode         = BlendMode::ADDITIVE;
    risingConfig.m_emissiveIntensity = 1.4f;

    ParticleEmitter* risingFrost = new ParticleEmitter( risingConfig );
    g_engine->m_particleSystem->Spawn( risingFrost );

    // Falling snowflakes
    ParticleEmitterConfig snowfallConfig;
    snowfallConfig.m_position          = frostPosition + Vec3( 0.f, 0.f, 2.3f );
    snowfallConfig.m_spawnOffsetXRange = FloatRange( -1.7f, 1.7f );
    snowfallConfig.m_spawnOffsetYRange = FloatRange( -1.7f, 1.7f );
    snowfallConfig.m_spawnOffsetZRange = FloatRange( -0.2f, 0.3f );
    snowfallConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    snowfallConfig.m_pitchRange        = FloatRange( 72.f, 88.f );
    snowfallConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    snowfallConfig.m_speedRange        = FloatRange( 1.5f, 3.f );
    snowfallConfig.m_lifetimeRange     = FloatRange( 0.7f, 1.2f );
    snowfallConfig.m_startSizeRange    = FloatRange( 0.08f, 0.16f );
    snowfallConfig.m_endSizeRange      = FloatRange( 0.04f, 0.09f );
    snowfallConfig.m_billboardType     = BillboardType::FULL_FACING;
    snowfallConfig.m_spawnRate         = 12.f;
    snowfallConfig.m_emissionDuration  = 1.4f;
    snowfallConfig.m_startColor        = Rgba8( 210, 245, 255, 180 );
    snowfallConfig.m_endColor          = Rgba8( 70, 150, 255, 0 );
    snowfallConfig.m_texturePath       = frostTexture;
    snowfallConfig.m_blendMode         = BlendMode::ADDITIVE;
    snowfallConfig.m_emissiveIntensity = 0.65f;

    ParticleEmitter* snowfall = new ParticleEmitter( snowfallConfig );
    g_engine->m_particleSystem->Spawn( snowfall );
}