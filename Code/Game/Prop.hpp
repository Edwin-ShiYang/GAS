#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
class Game;
class StaticMeshDefinition;

//-----------------------------------------------------------------------------------------------
class Prop : public Actor
{
public:
    Prop( Game* game, StaticMeshDefinition const& staticMeshDef );
    ~Prop() = default;

    void  Update() override;
    void  Render() const override;
    Mat44 GetModelToWorldTransform() const override;

public:
    Game*                       m_game = nullptr;
    Mat44                       m_toEngineMatrix;
    StaticMeshDefinition const& m_staticMeshDef;
};