#include "Game/WeaponDefinition.hpp"
#include "Game/GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id           = ParseXmlAttribute( element, "id", m_id );
    m_staticMeshId = ParseXmlAttribute( element, "staticMeshId", m_staticMeshId );
    m_position     = ParseXmlAttribute( element, "position", m_position );
    m_orientation  = ParseXmlAttribute( element, "orientation", m_orientation );
    m_socketName   = ParseXmlAttribute( element, "socketName", m_socketName );
}

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::InitializeDefinitions()
{
    std::string filePath = WEAPON_DEFINITION;
    XmlDocument weaponDefsXml;
    XmlResult   result = weaponDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load weapon defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = weaponDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* weaponDefElement = rootElement->FirstChildElement();
    while ( weaponDefElement )
    {
        std::string weaponDefElementName = weaponDefElement->Name();
        GUARANTEE_OR_DIE( weaponDefElementName == "WeaponDefinition", Stringf( "WeaponDefinition is not Found" ) )

        WeaponDefinition* weaponDef = new WeaponDefinition();
        weaponDef->LoadFromXmlElement( *weaponDefElement );

        s_definitions.push_back( weaponDef );
        weaponDefElement = weaponDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void WeaponDefinition::ClearDefinitions()
{
    for ( WeaponDefinition* weaponDef : s_definitions )
    {
        if ( !weaponDef ) continue;
        delete weaponDef;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
WeaponDefinition const& WeaponDefinition::GetDefinitionById( std::string const& id )
{
    for ( WeaponDefinition* weaponDef : s_definitions )
    {
        if ( weaponDef->m_id == id )
        {
            return *weaponDef;
        }
    }
    ERROR_AND_DIE( "StaticMeshDefinition is not Found" );
}

//-----------------------------------------------------------------------------------------------
std::vector< WeaponDefinition* > WeaponDefinition::s_definitions;