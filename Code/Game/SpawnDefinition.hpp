#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>
#include <string>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class SpawnDefinition
{
public:
    SpawnDefinition()  = default;
    ~SpawnDefinition() = default;

    void                                   LoadFromXmlElement( XmlElement const& element );
    static void                            InitializeDefinitions();
    static void                            ClearDefinitions();
    static std::vector< SpawnDefinition* > s_definitions;

    std::string                            m_id;
    std::string                            m_type;
    Vec3                                   m_position;
    EulerAngles                            m_orientation;
};