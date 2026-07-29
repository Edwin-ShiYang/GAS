#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class AttributeSetDefinition
{
public:
    AttributeSetDefinition()  = default;
    ~AttributeSetDefinition() = default;
    void LoadFromXmlElement( XmlElement const& element );

public:
    std::map< std::string, float > m_attributes;
};