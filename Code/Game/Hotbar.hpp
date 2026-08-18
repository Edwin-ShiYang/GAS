#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Widget.hpp"

//-----------------------------------------------------------------------------------------------
class Hotbar : public Widget
{
public:
    Hotbar();
    ~Hotbar() = default;

    void Render() const override;
};