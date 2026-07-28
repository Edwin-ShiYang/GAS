#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/StaticMeshActor.hpp"
#include "Game/Game.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
class Weapon : public StaticMeshActor
{
public:
    Weapon( Game* game, std::string name );
    ~Weapon() = default;

    void Update() override;
    void Render() const override;
};