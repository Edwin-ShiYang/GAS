#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class EquipmentDefinition
{
public:
    EquipmentDefinition()  = default;
    ~EquipmentDefinition() = default;

    void                                       LoadFromXmlElement( XmlElement const& element );
    static void                                InitializeDefinitions();
    static void                                ClearDefinitions();
    static EquipmentDefinition const&          GetDefinitionById( std::string const& id );
    static std::vector< EquipmentDefinition* > s_definitions;

    std::string                                m_id;
    std::string                                m_staticMeshId;
    std::string                                m_socketName;
    Vec3                                       m_position;
    EulerAngles                                m_orientation;
};