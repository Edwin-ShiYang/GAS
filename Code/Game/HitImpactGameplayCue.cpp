#include "Game/HitImpactGameplayCue.hpp"
#include "Engine/ParticleSystem/ParticleEmitter.hpp"
#include "App.hpp"
#include "Game.hpp"

void HitImpactGameplayCue::HandleGameplayCue( GameplayCueEvent eventType, GameplayCueParameters const& params )
{
    m_particleEmitter = new ParticleEmitter( params.m_target->m_position );
    g_app->m_game->m_particleEmitters.push_back( m_particleEmitter );
}