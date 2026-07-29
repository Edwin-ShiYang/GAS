#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

class AttributeSetDefinition;

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponentDefinition
{
public:
    AbilitySystemComponentDefinition() = default;
    ~AbilitySystemComponentDefinition();

    void                                                    LoadFromXmlElement( XmlElement const& element );
    std::map< std::string, float >                          GetAttributes() const;

    static void                                             InitializeDefinitions();
    static AbilitySystemComponentDefinition const*          GetDefinitionByName( std::string const& name );
    static void                                             ClearDefinitions();
    static std::vector< AbilitySystemComponentDefinition* > s_definitions;

    std::string                                             m_name            = "Unknown";
    AttributeSetDefinition*                                 m_attributeSetDef = nullptr;
};