#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
class Game;
class PropDefinition;
class StaticMeshDefinition;

//-----------------------------------------------------------------------------------------------
class Prop : public Actor
{
public:
    Prop( Game* game, std::string id );
    ~Prop();

    void  Update() override;
    void  Render() const override;
    Mat44 GetModelToWorldTransform() const override;

public:
    Mat44                       m_toEngineMatrix;
    PropDefinition const*       m_propDef       = nullptr;
    Game*                       m_game          = nullptr;
    StaticMeshDefinition const* m_staticMeshDef = nullptr;
};