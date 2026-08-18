#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Widget.hpp"

//-----------------------------------------------------------------------------------------------
class ManaOrb : public Widget
{
public:
    ManaOrb();
    ~ManaOrb() = default;
    void Render() const override;
};