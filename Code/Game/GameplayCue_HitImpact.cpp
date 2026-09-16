#include "Game/GameplayCue_HitImpact.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "App.hpp"

void GameplayCue_HitImpact::HandleGameplayCue( [[maybe_unused]] GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    Vec3                  enemyPosition = params.m_target->m_position + Vec3( 0.f, 0.f, 1.f );
    Vec3                  hitDirection  = ( params.m_target->m_position - params.m_instigator->m_position ).GetNormalized();
    float                 centerYaw     = hitDirection.GetOrientationAboutZDegrees();
    float                 coneHalfAngle = 10.f;

    ParticleEmitterConfig config;
    config.m_position          = enemyPosition;
    config.m_yawRange          = FloatRange( centerYaw - coneHalfAngle, centerYaw + coneHalfAngle );
    config.m_pitchRange        = FloatRange( -20.f, 20.f );
    config.m_rollRange         = FloatRange( 0.f, 360.f );
    config.m_speedRange        = FloatRange( 2.f, 5.f );
    config.m_lifetimeRange     = FloatRange( 0.25f, 0.45f );
    config.m_startSizeRange    = FloatRange( 0.12f, 0.2f );
    config.m_endSizeRange      = FloatRange( 0.02f, 0.05f );
    config.m_startColor        = Rgba8( 255, 210, 140, 255 );
    config.m_endColor          = Rgba8( 255, 60, 20, 0 );
    config.m_texturePath       = "Data/Textures/VFX/PolygonParticles_Sparkle.png";
    config.m_blendMode         = BlendMode::ADDITIVE;
    config.m_emissiveIntensity = 6.f;

    ParticleEmitter* emitter = new ParticleEmitter( config );
    emitter->SpawnBurst( 48 );
    g_engine->m_particleSystem->Spawn( emitter );
}