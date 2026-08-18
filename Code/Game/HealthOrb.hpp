#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Widget.hpp"

//-----------------------------------------------------------------------------------------------
class HealthOrb : public Widget
{
public:
    HealthOrb();
    ~HealthOrb() = default;
    void Render() const override;
};