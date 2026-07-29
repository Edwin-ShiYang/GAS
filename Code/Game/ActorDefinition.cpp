#include "Game/ActorDefinition.hpp"
#include "Game/StaticMeshDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"
#include "Game/AnimationSetDefinition.hpp"
#include "Game/GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"

//-----------------------------------------------------------------------------------------------
std::vector< ActorDefinition* > ActorDefinition::s_definitions;

//-----------------------------------------------------------------------------------------------
ActorDefinition::~ActorDefinition()
{
    delete m_staticMeshDef;
    m_staticMeshDef = nullptr;

    delete m_skeletalMeshDef;
    m_skeletalMeshDef = nullptr;
}

//-----------------------------------------------------------------------------------------------
void ActorDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name          = ParseXmlAttribute( element, "name", m_name );
    m_spawnPosition = ParseXmlAttribute( element, "spawnPosition", m_spawnPosition );

    std::string             animSet    = ParseXmlAttribute( element, "animationSet", "" );
    AnimationSetDefinition* animSetDef = AnimationSetDefinition::GetDefinitionByName( animSet );
    m_animSetDef                       = animSetDef;

    std::string asc = ParseXmlAttribute( element, "asc", "" );
    m_ascDef        = AbilitySystemComponentDefinition::GetDefinitionByName( asc );
}

//-----------------------------------------------------------------------------------------------
void ActorDefinition::InitializeDefinitions()
{
    std::string filePath = ACTOR_DEFINITION;
    XmlDocument actorDefsXml;
    XmlResult   result = actorDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load actor defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = actorDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) );

    XmlElement* actorDefElement = rootElement->FirstChildElement();
    while ( actorDefElement )
    {
        std::string actorDefElementName = actorDefElement->Name();
        GUARANTEE_OR_DIE( actorDefElementName == "ActorDefinition", Stringf( "ActorDefinition is not Found" ) )

        ActorDefinition* actorDef = new ActorDefinition();
        actorDef->LoadFromXmlElement( *actorDefElement );

        XmlElement const* staticElement   = actorDefElement->FirstChildElement( "StaticMesh" );
        XmlElement const* skeletalElement = actorDefElement->FirstChildElement( "SkeletalMesh" );

        if ( staticElement )
        {
            StaticMeshDefinition* staticMeshDef = new StaticMeshDefinition();
            staticMeshDef->LoadFromXmlElement( *staticElement );
            actorDef->m_staticMeshDef = staticMeshDef;
        }
        else
        {
            SkeletalMeshDefinition* skeletalMeshDef = new SkeletalMeshDefinition();
            skeletalMeshDef->LoadFromXmlElement( *skeletalElement );
            actorDef->m_skeletalMeshDef = skeletalMeshDef;
        }

        s_definitions.push_back( actorDef );
        actorDefElement = actorDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void ActorDefinition::ClearDefinitions()
{
    for ( int actorDefIndex = 0; actorDefIndex < static_cast< int >( s_definitions.size() ); ++actorDefIndex )
    {
        ActorDefinition* actorDef = s_definitions[ actorDefIndex ];
        if ( actorDef )
        {
            delete actorDef;
            actorDef = nullptr;
        }
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
ActorDefinition const* ActorDefinition::GetDefinitionByName( std::string const& name )
{
    for ( int actorDefIndex = 0; actorDefIndex < static_cast< int >( s_definitions.size() ); ++actorDefIndex )
    {
        ActorDefinition* actorDef = s_definitions[ actorDefIndex ];
        if ( actorDef->m_name == name )
        {
            return actorDef;
        }
    }
    return nullptr;
}