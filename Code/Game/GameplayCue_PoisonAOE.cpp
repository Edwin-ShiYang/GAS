#include "GameplayCue_PoisonAOE.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/VFX/SpriteSheetEffect.hpp"

//-----------------------------------------------------------------------------------------------
GameplayCue_PoisonAOE::GameplayCue_PoisonAOE()
{
}

//-----------------------------------------------------------------------------------------------
void GameplayCue_PoisonAOE::HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    if ( !params.m_target )
    {
        return;
    }

    Vec3 const         aoePosition          = params.m_target->m_position;
    char const*        poisonCloudTexture   = "Data/Textures/VFX/PoisonCloudParticle.png";
    char const*        poisonDropletTexture = "Data/Textures/VFX/PolygonParticles_Bubble_02.png";
    char const*        poisonBubbleTexture  = "Data/Textures/VFX/PolygonParticles_Bubble.png";

    SpriteSheetEffect* groundCircle   = new SpriteSheetEffect( "Data/Textures/VFX/PolygonParticles_RitualCircle_01.png", IntVec2( 1, 1 ), 0, 0, 1.f, SpriteAnimPlaybackType::ONCE );
    groundCircle->m_position          = aoePosition + Vec3( 0.f, 0.f, 0.03f );
    groundCircle->m_size              = Vec2( 5.f, 5.f );
    groundCircle->m_tint              = Rgba8( 70, 190, 30, 145 );
    groundCircle->m_lifeSpan          = 2.5f;
    groundCircle->m_emissiveIntensity = 0.65f;
    groundCircle->m_renderMode        = SpriteSheetEffectRenderMode::GROUND_PLANE;
    groundCircle->m_blendMode         = SpriteSheetEffectBlendMode::ALPHA;

    g_engine->m_vfxSystem->SpawnSpriteSheetEffect( groundCircle );

    ParticleEmitterConfig dropletConfig;
    dropletConfig.m_position          = aoePosition + Vec3( 0.f, 0.f, 0.15f );
    dropletConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    dropletConfig.m_pitchRange        = FloatRange( -10.f, 2.f );
    dropletConfig.m_speedRange        = FloatRange( 3.f, 5.f );
    dropletConfig.m_lifetimeRange     = FloatRange( 0.4f, 0.7f );
    dropletConfig.m_startSizeRange    = FloatRange( 0.18f, 0.35f );
    dropletConfig.m_endSizeRange      = FloatRange( 0.08f, 0.16f );
    dropletConfig.m_startColor        = Rgba8( 220, 255, 80, 255 );
    dropletConfig.m_endColor          = Rgba8( 90, 220, 25, 0 );
    dropletConfig.m_texturePath       = poisonDropletTexture;
    dropletConfig.m_blendMode         = BlendMode::ADDITIVE;
    dropletConfig.m_emissiveIntensity = 1.5f;
    ParticleEmitter* poisonDroplets   = new ParticleEmitter( dropletConfig );

    poisonDroplets->SpawnBurst( 40 );
    g_engine->m_particleSystem->Spawn( poisonDroplets );

    ParticleEmitterConfig groundCloudConfig;
    groundCloudConfig.m_position          = aoePosition + Vec3( 0.f, 0.f, 0.12f );
    groundCloudConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    groundCloudConfig.m_pitchRange        = FloatRange( -5.f, 1.f );
    groundCloudConfig.m_speedRange        = FloatRange( 0.7f, 1.6f );
    groundCloudConfig.m_lifetimeRange     = FloatRange( 0.7f, 1.1f );
    groundCloudConfig.m_startSizeRange    = FloatRange( 0.6f, 0.9f );
    groundCloudConfig.m_endSizeRange      = FloatRange( 1.3f, 1.8f );
    groundCloudConfig.m_startColor        = Rgba8( 255, 255, 255, 220 );
    groundCloudConfig.m_endColor          = Rgba8( 255, 255, 255, 0 );
    groundCloudConfig.m_texturePath       = poisonCloudTexture;
    groundCloudConfig.m_blendMode         = BlendMode::ALPHA;
    groundCloudConfig.m_emissiveIntensity = 0.9f;
    ParticleEmitter* groundPoisonCloud    = new ParticleEmitter( groundCloudConfig );

    groundPoisonCloud->SpawnBurst( 24 );
    g_engine->m_particleSystem->Spawn( groundPoisonCloud );

    ParticleEmitterConfig bubbleConfig;
    bubbleConfig.m_position          = aoePosition + Vec3( 0.f, 0.f, 0.15f );
    bubbleConfig.m_spawnOffsetXRange = FloatRange( -1.75f, 1.75f );
    bubbleConfig.m_spawnOffsetYRange = FloatRange( -1.75f, 1.75f );
    bubbleConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.15f );
    bubbleConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    bubbleConfig.m_pitchRange        = FloatRange( -80.f, -55.f );
    bubbleConfig.m_speedRange        = FloatRange( 0.5f, 1.2f );
    bubbleConfig.m_lifetimeRange     = FloatRange( 0.8f, 1.4f );
    bubbleConfig.m_startSizeRange    = FloatRange( 0.2f, 0.4f );
    bubbleConfig.m_endSizeRange      = FloatRange( 0.06f, 0.14f );
    bubbleConfig.m_spawnRate         = 14.f;
    bubbleConfig.m_emissionDuration  = 2.5f;
    bubbleConfig.m_startColor        = Rgba8( 200, 255, 60, 240 );
    bubbleConfig.m_endColor          = Rgba8( 70, 190, 20, 0 );

    bubbleConfig.m_texturePath       = poisonBubbleTexture;
    bubbleConfig.m_blendMode         = BlendMode::ADDITIVE;
    bubbleConfig.m_emissiveIntensity = 1.3f;
    ParticleEmitter* poisonBubbles   = new ParticleEmitter( bubbleConfig );

    g_engine->m_particleSystem->Spawn( poisonBubbles );

    ParticleEmitterConfig lingeringConfig;
    lingeringConfig.m_position          = aoePosition + Vec3( 0.f, 0.f, 0.1f );
    lingeringConfig.m_spawnOffsetXRange = FloatRange( -1.75f, 1.75f );
    lingeringConfig.m_spawnOffsetYRange = FloatRange( -1.75f, 1.75f );
    lingeringConfig.m_spawnOffsetZRange = FloatRange( 0.f, 0.1f );
    lingeringConfig.m_yawRange          = FloatRange( 0.f, 360.f );
    lingeringConfig.m_pitchRange        = FloatRange( -6.f, 2.f );
    lingeringConfig.m_speedRange        = FloatRange( 0.05f, 0.2f );
    lingeringConfig.m_lifetimeRange     = FloatRange( 1.2f, 1.8f );
    lingeringConfig.m_startSizeRange    = FloatRange( 0.5f, 0.8f );
    lingeringConfig.m_endSizeRange      = FloatRange( 1.f, 1.6f );
    lingeringConfig.m_spawnRate         = 6.f;
    lingeringConfig.m_emissionDuration  = 2.5f;
    lingeringConfig.m_startColor        = Rgba8( 255, 255, 255, 160 );
    lingeringConfig.m_endColor          = Rgba8( 255, 255, 255, 0 );

    lingeringConfig.m_texturePath       = poisonCloudTexture;
    lingeringConfig.m_blendMode         = BlendMode::ALPHA;
    lingeringConfig.m_emissiveIntensity = 0.65f;

    ParticleEmitter* lingeringPoison = new ParticleEmitter( lingeringConfig );

    g_engine->m_particleSystem->Spawn( lingeringPoison );
}