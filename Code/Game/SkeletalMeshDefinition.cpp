#include "Game/SkeletalMeshDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void SkeletalMeshDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_filePath = ParseXmlAttribute( element, "filePath", m_filePath );
    GUARANTEE_OR_DIE( !m_filePath.empty(), "[StaticMeshDefinition::LoadFromXmlElement] StaticMesh is missing file path" );

    std::string x = ParseXmlAttribute( element, "x", "" );
    GUARANTEE_OR_DIE( !x.empty(), "[StaticMeshDefinition::LoadFromXmlElement] StaticMesh is missing x axis" );
    m_axes[ "x" ] = ParseCoordinateAxis( x );

    std::string y = ParseXmlAttribute( element, "y", "" );
    GUARANTEE_OR_DIE( !y.empty(), "[StaticMeshDefinition::LoadFromXmlElement] StaticMesh is missing y axis" );
    m_axes[ "y" ] = ParseCoordinateAxis( y );

    std::string z = ParseXmlAttribute( element, "z", "" );
    GUARANTEE_OR_DIE( !z.empty(), "[StaticMeshDefinition::LoadFromXmlElement] StaticMesh is missing z axis" );
    m_axes[ "z" ] = ParseCoordinateAxis( z );

    m_metallic          = ParseXmlAttribute( element, "metallic", m_metallic );
    m_roughness         = ParseXmlAttribute( element, "roughness", m_roughness );
    m_ambientOcclusion  = ParseXmlAttribute( element, "ambientOcclusion", m_ambientOcclusion );
    m_emissiveIntensity = ParseXmlAttribute( element, "emissiveIntensity", m_emissiveIntensity );
}

//-----------------------------------------------------------------------------------------------
Axis SkeletalMeshDefinition::ParseCoordinateAxis( std::string axis )
{
    if ( axis == "left" ) return Axis::LEFT;
    if ( axis == "backward" ) return Axis::BACKWARD;
    if ( axis == "up" ) return Axis::UP;
    if ( axis == "forward" ) return Axis::FORWARD;
    if ( axis == "right" ) return Axis::RIGHT;
    if ( axis == "down" ) return Axis::DOWN;

    ERROR_AND_DIE( Stringf( "Invalid coordinate axis: %s", axis.c_str() ) );
}