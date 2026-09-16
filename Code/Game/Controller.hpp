#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
class Controller
{
public:
    Controller() = default;
    Controller( Game* game, ActorHandle const& actorHandle );
    virtual ~Controller() = default;

    void         Possess( ActorHandle const& actorHandle );
    Actor*       GetActor() const;
    virtual void SetEnabled( bool enabled );

    ActorHandle  m_actorHandle = ActorHandle::INVALID;
    Game*        m_game        = nullptr;
    bool         m_enable      = true;
};