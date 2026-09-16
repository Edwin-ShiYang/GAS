#include "Game/ProjectileDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void ProjectileDefinition::InitializeDefinitions()
{
    std::string filePath = "Data/Definitions/ProjectileDefinitions.xml";
    XmlDocument projectileDefsXml;
    XmlResult   result = projectileDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load ProjectileDefinitions: %s", filePath.c_str() ) );

    XmlElement* rootElement = projectileDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* projectileDefElement = rootElement->FirstChildElement( "ProjectileDefinition" );
    while ( projectileDefElement )
    {
        ProjectileDefinition* projectileDef = new ProjectileDefinition();
        projectileDef->LoadFromXmlElement( *projectileDefElement );

        s_definitions.push_back( projectileDef );
        projectileDefElement = projectileDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
ProjectileDefinition const& ProjectileDefinition::GetDefinitionById( std::string const& id )
{
    for ( ProjectileDefinition* projectileDef : s_definitions )
    {
        if ( projectileDef->m_id == id )
        {
            return *projectileDef;
        }
    }
    ERROR_AND_DIE( "ProjectileDef is not found" );
}

//-----------------------------------------------------------------------------------------------
void ProjectileDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id       = ParseXmlAttribute( element, "id", m_id );
    m_duration = ParseXmlAttribute( element, "duration", m_duration );

    if ( XmlElement const* billboardElement = element.FirstChildElement( "Billboard" ) )
    {
        m_billboardType   = ParseXmlAttribute( *billboardElement, "billboardType", m_billboardType );
        m_spriteSheetPath = ParseXmlAttribute( *billboardElement, "spriteSheetPath", m_spriteSheetPath );
        m_gridLayout      = ParseXmlAttribute( *billboardElement, "gridLayout", m_gridLayout );
        m_startFrame      = ParseXmlAttribute( *billboardElement, "startFrame", m_startFrame );
        m_endFrame        = ParseXmlAttribute( *billboardElement, "endFrame", m_endFrame );
        m_framePerSecond  = ParseXmlAttribute( *billboardElement, "framePerSecond", m_framePerSecond );
        m_playbackMode    = ParseXmlAttribute( *billboardElement, "playbackMode", m_playbackMode );
        m_visualType      = ProjectileVisualType::BILLBOARD;
    }
    else if ( XmlElement const* staticMeshElement = element.FirstChildElement( "StaticMesh" ) )
    {
        m_staticMeshId = ParseXmlAttribute( *staticMeshElement, "staticMeshId", m_staticMeshId );
        m_visualType   = ProjectileVisualType::MESH;
    }
}

//-----------------------------------------------------------------------------------------------
void ProjectileDefinition::ClearDefinitions()
{
    for ( ProjectileDefinition* projectileDef : s_definitions )
    {
        if ( !projectileDef ) continue;
        delete projectileDef;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
std::vector< ProjectileDefinition* > ProjectileDefinition::s_definitions;