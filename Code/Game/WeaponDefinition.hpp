#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class WeaponDefinition
{
public:
    WeaponDefinition()  = default;
    ~WeaponDefinition() = default;

    void                                    LoadFromXmlElement( XmlElement const& element );
    static void                             InitializeDefinitions();
    static void                             ClearDefinitions();
    static WeaponDefinition const&          GetDefinitionById( std::string const& id );
    static std::vector< WeaponDefinition* > s_definitions;

    std::string                             m_id;
    std::string                             m_staticMeshId;
    std::string                             m_socketName;
    Vec3                                    m_position;
    EulerAngles                             m_orientation;
};