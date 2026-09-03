#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
class HealthOrb : public UIWidget
{
public:
    HealthOrb();
    ~HealthOrb() = default;
    void Render() const override;
};