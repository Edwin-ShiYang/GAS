#include "Game/SkeletalMeshDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
void SkeletalMeshDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id = ParseXmlAttribute( element, "id", m_id );
    GUARANTEE_OR_DIE( !m_id.empty(), "SkeletalMesh is missing id" );

    m_filePath = ParseXmlAttribute( element, "filePath", m_filePath );
    GUARANTEE_OR_DIE( !m_filePath.empty(), "SkeletalMesh is missing file path" );

    std::string x = ParseXmlAttribute( element, "x", "" );
    GUARANTEE_OR_DIE( !x.empty(), "SkeletalMesh is missing x axis" );
    m_axes[ "x" ] = ParseCoordinateAxis( x );

    std::string y = ParseXmlAttribute( element, "y", "" );
    GUARANTEE_OR_DIE( !y.empty(), "SkeletalMesh is missing y axis" );
    m_axes[ "y" ] = ParseCoordinateAxis( y );

    std::string z = ParseXmlAttribute( element, "z", "" );
    GUARANTEE_OR_DIE( !z.empty(), "SkeletalMesh is missing z axis" );
    m_axes[ "z" ] = ParseCoordinateAxis( z );

    m_metallic          = ParseXmlAttribute( element, "metallic", m_metallic );
    m_roughness         = ParseXmlAttribute( element, "roughness", m_roughness );
    m_ambientOcclusion  = ParseXmlAttribute( element, "ambientOcclusion", m_ambientOcclusion );
    m_emissiveIntensity = ParseXmlAttribute( element, "emissiveIntensity", m_emissiveIntensity );
}

//-----------------------------------------------------------------------------------------------
void SkeletalMeshDefinition::InitializeDefinitions()
{
    std::string filePath = "Data/Definitions/SkeletalMeshDefinitions.xml";

    XmlDocument skeletalMeshDefsXml;
    XmlResult   result = skeletalMeshDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load skeletal mesh defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = skeletalMeshDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* skeletalMeshDefElement = rootElement->FirstChildElement();
    while ( skeletalMeshDefElement )
    {
        std::string skeletalMeshDefElementName = skeletalMeshDefElement->Name();
        GUARANTEE_OR_DIE( skeletalMeshDefElementName == "SkeletalMeshDefinition", Stringf( "SkeletalMeshDefinition is not Found" ) );

        SkeletalMeshDefinition* skeletalMeshDef = new SkeletalMeshDefinition();
        skeletalMeshDef->LoadFromXmlElement( *skeletalMeshDefElement );

        s_definitions[ skeletalMeshDef->m_id ] = skeletalMeshDef;
        skeletalMeshDefElement                 = skeletalMeshDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void SkeletalMeshDefinition::ClearDefinitions()
{
    for ( auto const& def : s_definitions )
    {
        delete def.second;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
SkeletalMeshDefinition const* SkeletalMeshDefinition::GetDefinitionById( std::string const& skeletalMeshId )
{
    auto iter = s_definitions.find( skeletalMeshId );
    if ( iter == s_definitions.end() ) return nullptr;

    return iter->second;
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

//-----------------------------------------------------------------------------------------------
std::map< std::string, SkeletalMeshDefinition const* > SkeletalMeshDefinition::s_definitions;