#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Widget.hpp"

//-----------------------------------------------------------------------------------------------
class AbilitySlot : public Widget
{
public:
    explicit AbilitySlot( int slotIndex );
    ~AbilitySlot() = default;
    void Render() const override;

private:
    int m_slotIndex = 0;
};