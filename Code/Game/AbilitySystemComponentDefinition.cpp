#include "Game/AbilitySystemComponentDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/AttributeSetDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name = ParseXmlAttribute( element, "name", m_name );
    GUARANTEE_OR_DIE( !m_name.empty(), "AbilitySystemComponentDefinition::LoadFromXmlElement - name is missing" );
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::InitializeDefinitions()
{
    std::string filePath = ABILITY_SYSTEM_COMPONENT;
    XmlDocument ascDefsXml;
    XmlResult   result = ascDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load AbilitySystemComponentDefinitions: %s", filePath.c_str() ) )

    XmlElement* rootElement = ascDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) );

    XmlElement* ascDefElement = rootElement->FirstChildElement();
    while ( ascDefElement )
    {
        std::string ascDefElementName = ascDefElement->Name();
        GUARANTEE_OR_DIE( ascDefElementName == "AbilitySystemComponentDefinition", Stringf( "AbilitySystemComponentDefinition is not Found" ) )

        AbilitySystemComponentDefinition* ascDef = new AbilitySystemComponentDefinition();
        ascDef->LoadFromXmlElement( *ascDefElement );

        XmlElement const* attributeSetElement = ascDefElement->FirstChildElement( "AttributeSet" );
        if ( attributeSetElement )
        {
            ascDef->m_attributeSetDef = new AttributeSetDefinition();
            ascDef->m_attributeSetDef->LoadFromXmlElement( *attributeSetElement );
        }

        s_definitions.push_back( ascDef );
        ascDefElement = ascDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
std::vector< AbilitySystemComponentDefinition* > AbilitySystemComponentDefinition::s_definitions;