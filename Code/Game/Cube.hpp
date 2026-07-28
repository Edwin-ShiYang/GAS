#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Primitive.hpp"

//-----------------------------------------------------------------------------------------------
class Cube : public Primitive
{
public:
    Cube( Game* owner );
    ~Cube();

    void Update() override;
    void Render() const override;
};