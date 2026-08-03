#include "Game/StaticMeshDefinition.hpp"
//-----------------------------------------------------------------------------------------------

#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void StaticMeshDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id = ParseXmlAttribute( element, "id", m_id );
    GUARANTEE_OR_DIE( !m_id.empty(), "StaticMesh is missing id" );

    m_filePath = ParseXmlAttribute( element, "filePath", m_filePath );
    GUARANTEE_OR_DIE( !m_filePath.empty(), "StaticMesh is missing filePath" );

    std::string x = ParseXmlAttribute( element, "x", "" );
    GUARANTEE_OR_DIE( !x.empty(), "StaticMesh is missing x axis" );
    m_axes[ "x" ] = ParseCoordinateAxis( x );

    std::string y = ParseXmlAttribute( element, "y", "" );
    GUARANTEE_OR_DIE( !y.empty(), "StaticMesh is missing y axis" );
    m_axes[ "y" ] = ParseCoordinateAxis( y );

    std::string z = ParseXmlAttribute( element, "z", "" );
    GUARANTEE_OR_DIE( !z.empty(), "StaticMesh is missing z axis" );
    m_axes[ "z" ] = ParseCoordinateAxis( z );

    m_metallic          = ParseXmlAttribute( element, "metallic", m_metallic );
    m_roughness         = ParseXmlAttribute( element, "roughness", m_roughness );
    m_ambientOcclusion  = ParseXmlAttribute( element, "ambientOcclusion", m_ambientOcclusion );
    m_emissiveIntensity = ParseXmlAttribute( element, "emissiveIntensity", m_emissiveIntensity );
}

//-----------------------------------------------------------------------------------------------
void StaticMeshDefinition::InitializeDefinitions()
{
    std::string filePath = "Data/Definitions/StaticMeshDefinitions.xml";
    XmlDocument staticMeshDefsXml;
    XmlResult   result = staticMeshDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load static mesh defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = staticMeshDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* staticMeshDefElement = rootElement->FirstChildElement();
    while ( staticMeshDefElement )
    {
        std::string staticMeshDefElementName = staticMeshDefElement->Name();
        GUARANTEE_OR_DIE( staticMeshDefElementName == "StaticMeshDefinition", Stringf( "StaticMeshDefinition is not Found" ) );
        StaticMeshDefinition* staticMeshDef = new StaticMeshDefinition();
        staticMeshDef->LoadFromXmlElement( *staticMeshDefElement );

        s_definitions[ staticMeshDef->m_id ] = staticMeshDef;
        staticMeshDefElement                 = staticMeshDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void StaticMeshDefinition::ClearDefinitions()
{
    for ( auto const& entry : s_definitions )
    {
        delete entry.second;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
StaticMeshDefinition const* StaticMeshDefinition::GetDefinitionById( std::string const& staticMeshId )
{
    auto iter = s_definitions.find( staticMeshId );
    if ( iter == s_definitions.end() ) return nullptr;

    return iter->second;
}

//-----------------------------------------------------------------------------------------------
Axis StaticMeshDefinition::ParseCoordinateAxis( std::string axis )
{
    if ( axis == "left" ) return Axis::LEFT;
    if ( axis == "backward" ) return Axis::BACKWARD;
    if ( axis == "up" ) return Axis::UP;
    if ( axis == "forward" ) return Axis::FORWARD;
    if ( axis == "right" ) return Axis::RIGHT;
    if ( axis == "down" ) return Axis::DOWN;

    ERROR_AND_DIE( Stringf( "Invalid coordinate axis: %s", axis.c_str() ) );
}

//-----------------------------------------------------------------------------------------------
std::map< std::string, StaticMeshDefinition const* > StaticMeshDefinition::s_definitions;