#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Primitive.hpp"

//-----------------------------------------------------------------------------------------------
class Cylinder : public Primitive
{
public:
    Cylinder( Game* owner, float start, float end, float m_radius );
    ~Cylinder();

    void  Update() override;
    void  Render() const override;

    float m_start;
    float m_end;
    float m_radius;
};