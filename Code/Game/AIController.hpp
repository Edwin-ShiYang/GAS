#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Controller.hpp"

//-----------------------------------------------------------------------------------------------
class Game;
class Actor;

//-----------------------------------------------------------------------------------------------
class AIController : public Controller
{
public:
    AIController( Game* game, ActorHandle const& actorHandle );
    ~AIController() = default;
    void RenderDebug() const;

private:
    void ChasePlayer( Character& character );
    void TurnTowardDirection( Character* character );

public:
    void        Update();
    ActorHandle m_actorHandle = ActorHandle::INVALID;

private:
    Actor* m_targetActor    = nullptr;
    float  m_duration       = 2.4f;
    float  m_elapsedSeconds = 0.f;
};