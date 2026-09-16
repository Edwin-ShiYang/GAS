#include "Game/Controller.hpp"

//-----------------------------------------------------------------------------------------------
Controller::Controller( Game* game, ActorHandle const& actorHandle )
    : m_game( game )
    , m_actorHandle( actorHandle )
{
}

//-----------------------------------------------------------------------------------------------
Actor* Controller::GetActor() const
{
    if ( m_actorHandle == ActorHandle::INVALID )
    {
        return nullptr;
    }

    unsigned int index = m_actorHandle.GetIndex();
    return m_game->m_actors[ index ];
}

//-----------------------------------------------------------------------------------------------
void Controller::SetEnabled( bool enabled )
{
    m_enable = enabled;
}