#include "Game/EquipmentDefinition.hpp"
#include "Game/GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void EquipmentDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id           = ParseXmlAttribute( element, "id", m_id );
    m_staticMeshId = ParseXmlAttribute( element, "staticMeshId", m_staticMeshId );
    m_position     = ParseXmlAttribute( element, "position", m_position );
    m_orientation  = ParseXmlAttribute( element, "orientation", m_orientation );
    m_socketName   = ParseXmlAttribute( element, "socketName", m_socketName );
}

//-----------------------------------------------------------------------------------------------
void EquipmentDefinition::InitializeDefinitions()
{
    std::string filePath = "Data/Definitions/EquipmentDefinitions.xml";
    XmlDocument weaponDefsXml;
    XmlResult   result = weaponDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load EquipmentDefinitions file: %s", filePath.c_str() ) )

    XmlElement* rootElement = weaponDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* weaponDefElement = rootElement->FirstChildElement();
    while ( weaponDefElement )
    {
        std::string weaponDefElementName = weaponDefElement->Name();
        GUARANTEE_OR_DIE( weaponDefElementName == "EquipmentDefinition", Stringf( "EquipmentDefinition is not Found" ) )

        EquipmentDefinition* weaponDef = new EquipmentDefinition();
        weaponDef->LoadFromXmlElement( *weaponDefElement );

        s_definitions.push_back( weaponDef );
        weaponDefElement = weaponDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void EquipmentDefinition::ClearDefinitions()
{
    for ( EquipmentDefinition* weaponDef : s_definitions )
    {
        if ( !weaponDef ) continue;
        delete weaponDef;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
EquipmentDefinition const& EquipmentDefinition::GetDefinitionById( std::string const& id )
{
    for ( EquipmentDefinition* weaponDef : s_definitions )
    {
        if ( weaponDef->m_id == id )
        {
            return *weaponDef;
        }
    }
    ERROR_AND_DIE( "StaticMeshDefinition is not Found" );
}

//-----------------------------------------------------------------------------------------------
std::vector< EquipmentDefinition* > EquipmentDefinition::s_definitions;