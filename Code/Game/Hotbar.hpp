#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
class Hotbar : public UIWidget
{
public:
    Hotbar();
    ~Hotbar() = default;

    void Render() const override;
};