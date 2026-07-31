#include "Game/ActorHandle.hpp"

//-----------------------------------------------------------------------------------------------
unsigned int      ActorHandle::s_nextActorUID = 0;

//-----------------------------------------------------------------------------------------------
ActorHandle const ActorHandle::INVALID( 0x0000ffffu, 0x0000ffffu );

//-----------------------------------------------------------------------------------------------
ActorHandle::ActorHandle( unsigned int uid, unsigned int index )
{
    unsigned maskedIndex = index & 0x0000FFFF;
    unsigned shiftedUID  = uid << 16;
    m_data               = maskedIndex | shiftedUID;
}

//-----------------------------------------------------------------------------------------------
bool ActorHandle::IsValid() const
{
    return m_data != INVALID.m_data;
}

//-----------------------------------------------------------------------------------------------
unsigned int ActorHandle::GetIndex() const
{
    return m_data & 0x0000FFFFU;
}

//-----------------------------------------------------------------------------------------------
unsigned int ActorHandle::GetData() const
{
    return m_data;
}

//-----------------------------------------------------------------------------------------------
bool ActorHandle::operator==( ActorHandle const& other ) const
{
    return m_data == other.m_data;
}

//-----------------------------------------------------------------------------------------------
bool ActorHandle::operator!=( ActorHandle const& other ) const
{
    return m_data != other.m_data;
}