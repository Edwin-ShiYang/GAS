#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

class AttributeSetDefinition;

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponentDefinition
{
public:
    AbilitySystemComponentDefinition()  = default;
    ~AbilitySystemComponentDefinition() = default;

    void                                                    LoadFromXmlElement( XmlElement const& element );
    static void                                             InitializeDefinitions();
    static void                                             ClearDefinitions();
    static std::vector< AbilitySystemComponentDefinition* > s_definitions;

    std::string                                             m_name            = "Unknown";
    AttributeSetDefinition*                                 m_attributeSetDef = nullptr;
};