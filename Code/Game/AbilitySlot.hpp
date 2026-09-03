#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/UIWidget.hpp"

//-----------------------------------------------------------------------------------------------
class AbilitySlot : public UIWidget
{
public:
    explicit AbilitySlot( int slotIndex );
    ~AbilitySlot() = default;
    void Render() const override;

private:
    int m_slotIndex = 0;
};