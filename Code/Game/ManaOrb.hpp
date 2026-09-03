#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
class ManaOrb : public UIWidget
{
public:
    ManaOrb();
    ~ManaOrb() = default;
    void Render() const override;
};