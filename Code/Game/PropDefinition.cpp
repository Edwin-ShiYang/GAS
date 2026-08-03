#include "Game/PropDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"
#include "Game/AnimationSetDefinition.hpp"
#include "Game/GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"

//-----------------------------------------------------------------------------------------------
std::vector< PropDefinition* > PropDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
PropDefinition::~PropDefinition()
{
    delete m_skeletalMeshDef;
    m_skeletalMeshDef = nullptr;
}

//-----------------------------------------------------------------------------------------------
void PropDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id            = ParseXmlAttribute( element, "id", m_id );
    m_staticMeshId  = ParseXmlAttribute( element, "staticMeshId", m_staticMeshId );
    m_spawnPosition = ParseXmlAttribute( element, "spawnPosition", m_spawnPosition );

    std::string             animSet    = ParseXmlAttribute( element, "animationSet", "" );
    AnimationSetDefinition* animSetDef = AnimationSetDefinition::GetDefinitionByName( animSet );
    m_animSetDef                       = animSetDef;

    std::string asc = ParseXmlAttribute( element, "asc", "" );
    m_ascDef        = AbilitySystemComponentDefinition::GetDefinitionByName( asc );
}

//-----------------------------------------------------------------------------------------------
void PropDefinition::InitializeDefinitions()
{
    std::string filePath = PROP_DEFINITION;
    XmlDocument propDefsXml;
    XmlResult   result = propDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load prop defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = propDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* propDefElement = rootElement->FirstChildElement();
    while ( propDefElement )
    {
        std::string propDefElementName = propDefElement->Name();
        GUARANTEE_OR_DIE( propDefElementName == "PropDefinition", Stringf( "PropDefinition is not Found" ) )

        PropDefinition* propDef = new PropDefinition();
        propDef->LoadFromXmlElement( *propDefElement );

        s_definitions.push_back( propDef );
        propDefElement = propDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void PropDefinition::ClearDefinitions()
{
    for ( int propDefIndex = 0; propDefIndex < static_cast< int >( s_definitions.size() ); ++propDefIndex )
    {
        PropDefinition* propDef = s_definitions[ propDefIndex ];
        if ( propDef )
        {
            delete propDef;
            propDef = nullptr;
        }
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
PropDefinition const* PropDefinition::GetDefinitionById( std::string const& id )
{
    for ( int propDefIndex = 0; propDefIndex < static_cast< int >( s_definitions.size() ); ++propDefIndex )
    {
        PropDefinition* propDef = s_definitions[ propDefIndex ];
        if ( propDef->m_id == id )
        {
            return propDef;
        }
    }
    return nullptr;
}