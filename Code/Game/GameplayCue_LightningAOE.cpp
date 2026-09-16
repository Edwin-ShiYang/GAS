#include "GameplayCue_LightningAOE.hpp"
#include "Engine/VFX/SpriteSheetEffect.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"

//-----------------------------------------------------------------------------------------------
GameplayCue_LightningAOE::GameplayCue_LightningAOE()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayCue_LightningAOE::HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    if ( !params.m_target )
    {
        return;
    }

    Vec3               position     = params.m_target->m_position;
    char const*        boltTexture  = "Data/Textures/VFX/LightningBoltParticle.png";
    char const*        coreTexture  = "Data/Textures/VFX/PurpleLightningParticle.png";
    char const*        sparkTexture = "Data/Textures/VFX/PolygonParticles_Sparkle.png";

    SpriteSheetEffect* groundCircle = new SpriteSheetEffect( "Data/Textures/VFX/PolygonParticles_RitualCircle_01.png", IntVec2( 1, 1 ), 0, 0, 1.f, SpriteAnimPlaybackType::ONCE );

    groundCircle->m_position          = position + Vec3( 0.f, 0.f, 0.03f );
    groundCircle->m_size              = Vec2( 5.f, 5.f );
    groundCircle->m_tint              = Rgba8( 170, 65, 255, 210 );
    groundCircle->m_lifeSpan          = 2.f;
    groundCircle->m_emissiveIntensity = 1.2f;
    groundCircle->m_renderMode        = SpriteSheetEffectRenderMode::GROUND_PLANE;
    groundCircle->m_blendMode         = SpriteSheetEffectBlendMode::ALPHA;
    g_engine->m_vfxSystem->SpawnSpriteSheetEffect( groundCircle );

    ParticleEmitterConfig boltConfig;

    boltConfig.m_position          = position + Vec3( 0.f, 0.f, 2.f );
    boltConfig.m_spawnOffsetXRange = FloatRange( -2.f, 2.f );
    boltConfig.m_spawnOffsetYRange = FloatRange( -2.f, 2.f );
    boltConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.f );
    boltConfig.m_speedRange        = FloatRange( 0.f, 0.f );
    boltConfig.m_lifetimeRange     = FloatRange( 0.06f, 0.1f );
    boltConfig.m_startSizeRange    = FloatRange( 3.5f, 4.2f );
    boltConfig.m_endSizeRange      = FloatRange( 3.5f, 4.2f );
    boltConfig.m_sizeAspect        = Vec2( 0.17f, 1.f );
    boltConfig.m_billboardType     = BillboardType::WORLD_UP_FACING;
    boltConfig.m_spawnRate         = 8.f;
    boltConfig.m_emissionDuration  = 2.f;
    boltConfig.m_startColor        = Rgba8( 195, 100, 255, 210 );
    boltConfig.m_endColor          = Rgba8( 90, 15, 255, 0 );
    boltConfig.m_texturePath       = boltTexture;
    boltConfig.m_blendMode         = BlendMode::ADDITIVE;
    boltConfig.m_emissiveIntensity = 1.5f;
    ParticleEmitter* bolts         = new ParticleEmitter( boltConfig );
    g_engine->m_particleSystem->Spawn( bolts );

    ParticleEmitterConfig coreConfig;
    coreConfig.m_position          = position + Vec3( 0.f, 0.f, 0.25f );
    coreConfig.m_spawnOffsetXRange = FloatRange( -0.3f, 0.3f );
    coreConfig.m_spawnOffsetYRange = FloatRange( -0.3f, 0.3f );
    coreConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.2f );
    coreConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    coreConfig.m_pitchRange        = FloatRange( -20.f, 5.f );
    coreConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    coreConfig.m_speedRange        = FloatRange( 0.2f, 1.2f );
    coreConfig.m_lifetimeRange     = FloatRange( 0.25f, 0.55f );
    coreConfig.m_startSizeRange    = FloatRange( 0.8f, 1.3f );
    coreConfig.m_endSizeRange      = FloatRange( 1.8f, 2.8f );
    coreConfig.m_sizeAspect        = Vec2( 1.f, 1.f );
    coreConfig.m_billboardType     = BillboardType::FULL_FACING;
    coreConfig.m_startColor        = Rgba8( 235, 190, 255, 255 );
    coreConfig.m_endColor          = Rgba8( 100, 20, 255, 0 );
    coreConfig.m_texturePath       = coreTexture;
    coreConfig.m_blendMode         = BlendMode::ADDITIVE;
    coreConfig.m_emissiveIntensity = 2.2f;
    ParticleEmitter* electricCore  = new ParticleEmitter( coreConfig );
    electricCore->SpawnBurst( 8 );
    g_engine->m_particleSystem->Spawn( electricCore );

    ParticleEmitterConfig sparkConfig;

    sparkConfig.m_position          = position + Vec3( 0.f, 0.f, 0.3f );
    sparkConfig.m_spawnOffsetXRange = FloatRange( -0.4f, 0.4f );
    sparkConfig.m_spawnOffsetYRange = FloatRange( -0.4f, 0.4f );
    sparkConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.3f );
    sparkConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    sparkConfig.m_pitchRange        = FloatRange( -40.f, 10.f );
    sparkConfig.m_rollRange         = FloatRange( 0.f, 360.f );
    sparkConfig.m_speedRange        = FloatRange( 6.f, 12.f );
    sparkConfig.m_lifetimeRange     = FloatRange( 0.18f, 0.55f );
    sparkConfig.m_startSizeRange    = FloatRange( 0.05f, 0.16f );
    sparkConfig.m_endSizeRange      = FloatRange( 0.01f, 0.04f );
    sparkConfig.m_sizeAspect        = Vec2( 1.f, 1.f );
    sparkConfig.m_billboardType     = BillboardType::FULL_FACING;
    sparkConfig.m_startColor        = Rgba8( 230, 180, 255, 255 );
    sparkConfig.m_endColor          = Rgba8( 90, 15, 255, 0 );
    sparkConfig.m_texturePath       = sparkTexture;
    sparkConfig.m_blendMode         = BlendMode::ADDITIVE;
    sparkConfig.m_emissiveIntensity = 4.f;
    ParticleEmitter* sparks         = new ParticleEmitter( sparkConfig );
    sparks->SpawnBurst( 48 );
    g_engine->m_particleSystem->Spawn( sparks );

    // Residual electricity
    ParticleEmitterConfig residualConfig;
    residualConfig.m_position            = position + Vec3( 0.f, 0.f, 0.2f );
    residualConfig.m_spawnOffsetXRange   = FloatRange( -1.8f, 1.8f );
    residualConfig.m_spawnOffsetYRange   = FloatRange( -1.8f, 1.8f );
    residualConfig.m_spawnOffsetZRange   = FloatRange( 0.f, 0.5f );
    residualConfig.m_yawRange            = FloatRange( 0.f, 360.f );
    residualConfig.m_pitchRange          = FloatRange( -50.f, 10.f );
    residualConfig.m_rollRange           = FloatRange( 0.f, 360.f );
    residualConfig.m_speedRange          = FloatRange( 0.1f, 0.5f );
    residualConfig.m_lifetimeRange       = FloatRange( 0.4f, 0.9f );
    residualConfig.m_startSizeRange      = FloatRange( 0.2f, 0.5f );
    residualConfig.m_endSizeRange        = FloatRange( 0.5f, 0.9f );
    residualConfig.m_sizeAspect          = Vec2( 1.f, 1.f );
    residualConfig.m_billboardType       = BillboardType::FULL_FACING;
    residualConfig.m_spawnRate           = 14.f;
    residualConfig.m_emissionDuration    = 1.2f;
    residualConfig.m_startColor          = Rgba8( 205, 120, 255, 190 );
    residualConfig.m_endColor            = Rgba8( 80, 10, 220, 0 );
    residualConfig.m_texturePath         = coreTexture;
    residualConfig.m_blendMode           = BlendMode::ADDITIVE;
    residualConfig.m_emissiveIntensity   = 1.5f;
    ParticleEmitter* residualElectricity = new ParticleEmitter( residualConfig );

    g_engine->m_particleSystem->Spawn( residualElectricity );
}