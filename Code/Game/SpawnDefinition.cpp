#include "Game/SpawnDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void SpawnDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id          = ParseXmlAttribute( element, "id", m_id );
    m_position    = ParseXmlAttribute( element, "position", m_position );
    m_orientation = ParseXmlAttribute( element, "orientation", m_orientation );
    m_type        = ParseXmlAttribute( element, "type", m_type );
}

//-----------------------------------------------------------------------------------------------
void SpawnDefinition::InitializeDefinitions()
{
    std::string filePath = "Data/Definitions/SpawnDefinitions.xml";
    XmlDocument spawnDefsXml;
    XmlResult   result = spawnDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load skeletal mesh defs file: %s", filePath.c_str() ) );

    XmlElement* rootElement = spawnDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* spawnDefElement = rootElement->FirstChildElement();
    while ( spawnDefElement )
    {
        SpawnDefinition* spawnDef = new SpawnDefinition();
        spawnDef->LoadFromXmlElement( *spawnDefElement );

        s_definitions.push_back( spawnDef );
        spawnDefElement = spawnDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void SpawnDefinition::ClearDefinitions()
{
    for ( SpawnDefinition* spawnDef : s_definitions )
    {
        if ( !spawnDef ) { continue; };
        delete spawnDef;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
std::vector< SpawnDefinition* > SpawnDefinition::s_definitions;