#pragma once
#include "Game/ActorHandle.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
class Game;
struct StaticModel;
class ActorDefinition;

//-----------------------------------------------------------------------------------------------
class Actor
{
public:
    Actor( Game* game, std::string name );
    virtual ~Actor();

    virtual void  Update()       = 0;
    virtual void  Render() const = 0;
    void          SetScale( Vec3 const& scale );

    virtual Mat44 GetModelToWorldTransform() const;

public:
    Vec3                   m_position;
    EulerAngles            m_orientation;
    Vec3                   m_scale = Vec3( 1.0f, 1.0f, 1.0f );
    Mat44                  m_toEngineMatrix;
    ActorDefinition const* m_actorDef = nullptr;
    ActorHandle            m_handle   = ActorHandle::INVALID;

protected:
    Game* m_game = nullptr;
};