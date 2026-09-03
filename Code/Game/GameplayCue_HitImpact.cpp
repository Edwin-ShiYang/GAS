#include "Game/GameplayCue_HitImpact.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "App.hpp"

void GameplayCue_HitImpact::HandleGameplayCue( [[maybe_unused]] GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    Vec3             enemyPosition = params.m_target->m_position + Vec3( 0.f, 0.f, 1.f );
    Vec3             toPlayer      = ( params.m_target->m_position - params.m_instigator->m_position ).GetNormalized();

    float            centerYaw     = toPlayer.GetOrientationAboutZDegrees();
    float            coneHalfAngle = 10.f;

    ParticleEmitter* emitter = new ParticleEmitter( enemyPosition,
                                                    FloatRange( centerYaw - coneHalfAngle, centerYaw + coneHalfAngle ),
                                                    FloatRange( -20.f, 20.f ),
                                                    FloatRange( 0.f, 0.f ),
                                                    Rgba8( 255, 210, 140, 255 ),
                                                    Rgba8( 255, 60, 20, 0 ) );
    emitter->SpawnBurst( 48 );
    g_engine->m_particleSystem->Spawn( emitter );
}