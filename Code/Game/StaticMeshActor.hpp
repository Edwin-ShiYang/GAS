#pragma once
#include "string"
#include "Game.hpp"
#include "Actor.hpp"

struct StaticModel;

//-----------------------------------------------------------------------------------------------
class StaticMeshActor : public Actor
{
public:
    StaticMeshActor( Game* game, std::string name );
    ~StaticMeshActor() = default;

    void Update() override;
    void Render() const override;

public:
    StaticModel* m_staticModel = nullptr;
};