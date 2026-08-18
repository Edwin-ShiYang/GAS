#include "Game/CharacterDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Animation/AnimationSetDefinition.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponentDefinition.hpp"
#include "Engine/Animation/AnimationGraphDefinition.hpp"

//-----------------------------------------------------------------------------------------------
void CharacterDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_id = ParseXmlAttribute( element, "id", m_id );
    GUARANTEE_OR_DIE( !m_id.empty(), "Character is missing id" );

    m_skeletalMesh = ParseXmlAttribute( element, "skeletalMesh", m_skeletalMesh );
    GUARANTEE_OR_DIE( !m_skeletalMesh.empty(), "Character is missing skeletalMeshId" );

    std::string animSet = ParseXmlAttribute( element, "animationSet", "" );
    GUARANTEE_OR_DIE( !animSet.empty(), "Character is missing animationSet" );

    m_animationSetDef = AnimationSetDefinition::GetDefinitionById( animSet );
    GUARANTEE_OR_DIE( m_animationSetDef, "Character is missing AnimationSetDefinition" );

    std::string asc = ParseXmlAttribute( element, "asc", "" );
    GUARANTEE_OR_DIE( !asc.empty(), "Character is missing asc" );

    m_ascDef = AbilitySystemComponentDefinition::GetDefinitionByName( asc );
    GUARANTEE_OR_DIE( m_ascDef, "Character is missing AbilitySystemComponentDefinition" );

    std::string animationGraph = ParseXmlAttribute( element, "animationGraph", "" );
    GUARANTEE_OR_DIE( !animationGraph.empty(), "Character is missing AnimationGraphDefinition" );
    m_animationGraphDef = AnimationGraphDefinition::GetDefinitionById( animationGraph );
}

//-----------------------------------------------------------------------------------------------
void CharacterDefinition::InitializeDefinitions()
{
    std::string filePath = "Data/Definitions/CharacterDefinitions.xml";

    XmlDocument characterDefsXml;
    XmlResult   result = characterDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load character defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = characterDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) )

    XmlElement* characterDefElement = rootElement->FirstChildElement();
    while ( characterDefElement )
    {
        std::string characterDefElementName = characterDefElement->Name();
        GUARANTEE_OR_DIE( characterDefElementName == "CharacterDefinition", Stringf( "CharacterDefinition is not Found" ) )

        CharacterDefinition* characterDef = new CharacterDefinition();
        characterDef->LoadFromXmlElement( *characterDefElement );

        s_definitions.push_back( characterDef );
        characterDefElement = characterDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void CharacterDefinition::ClearDefinitions()
{
    for ( CharacterDefinition* characterDef : s_definitions )
    {
        if ( !characterDef ) continue;
        delete characterDef;
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
CharacterDefinition const* CharacterDefinition::GetDefinitionById( std::string const& id )
{
    for ( CharacterDefinition* characterDef : s_definitions )
    {
        if ( characterDef->m_id == id )
        {
            return characterDef;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------
std::vector< CharacterDefinition* > CharacterDefinition::s_definitions;