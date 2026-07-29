#include "Game/AbilitySystemComponentDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Game/AttributeSetDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
AbilitySystemComponentDefinition::~AbilitySystemComponentDefinition()
{
    delete m_attributeSetDef;
    m_attributeSetDef = nullptr;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name = ParseXmlAttribute( element, "name", m_name );
    GUARANTEE_OR_DIE( !m_name.empty(), "AbilitySystemComponentDefinition::LoadFromXmlElement - name is missing" );
}

//-----------------------------------------------------------------------------------------------
std::map< std::string, float > AbilitySystemComponentDefinition::GetAttributes() const
{
    return m_attributeSetDef->m_attributes;
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
AbilitySystemComponentDefinition const* AbilitySystemComponentDefinition::GetDefinitionByName( std::string const& name )
{
    for ( int ascDefIndex = 0; ascDefIndex < static_cast< int >( s_definitions.size() ); ++ascDefIndex )
    {
        AbilitySystemComponentDefinition* ascDef = s_definitions[ ascDefIndex ];
        if ( ascDef->m_name == name )
        {
            return ascDef;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------
void AbilitySystemComponentDefinition::ClearDefinitions()
{
    for ( int ascDefIndex = 0; ascDefIndex < static_cast< int >( s_definitions.size() ); ++ascDefIndex )
    {
        AbilitySystemComponentDefinition* ascDef = s_definitions[ ascDefIndex ];
        if ( ascDef )
        {
            delete ascDef;
            ascDef = nullptr;
        }
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
std::vector< AbilitySystemComponentDefinition* > AbilitySystemComponentDefinition::s_definitions;