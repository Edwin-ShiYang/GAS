#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Prop.hpp"
#include "Game/Game.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
class Weapon : public Prop
{
public:
    Weapon( Game* game, std::string name );
    ~Weapon() = default;

    void Update() override;
    void Render() const override;
};