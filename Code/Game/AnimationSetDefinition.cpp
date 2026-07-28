#include "Game/AnimationSetDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Model/ModelImporter.hpp"
#include "Engine/AnimationClip.hpp"

//-----------------------------------------------------------------------------------------------
void AnimationSetDefinition::LoadFromXmlElement( XmlElement const& element )
{
    m_name = ParseXmlAttribute( element, "name", m_name );
    GUARANTEE_OR_DIE( !m_name.empty(), "[AnimationSetDefinition::LoadFromXmlElement] AnimationSet is missing name" );
}

//-----------------------------------------------------------------------------------------------
AnimationSetDefinition* AnimationSetDefinition::GetDefinitionByName( std::string const& name )
{
    for ( int animSetDefIndex = 0; animSetDefIndex < static_cast< int >( s_definitions.size() ); ++animSetDefIndex )
    {
        AnimationSetDefinition* animSetDef = s_definitions[ animSetDefIndex ];
        if ( animSetDef->m_name == name )
        {
            return animSetDef;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------
void AnimationSetDefinition::InitializeDefinitions()
{
    std::string filePath = ANIMATIONSET_DEFINITION;
    XmlDocument animSetDefsXml;
    XmlResult   result = animSetDefsXml.LoadFile( filePath.c_str() );
    GUARANTEE_OR_DIE( result == XML_SUCCESS, Stringf( "Failed to load animSet defs file: %s", filePath.c_str() ) )

    XmlElement* rootElement = animSetDefsXml.RootElement();
    GUARANTEE_OR_DIE( rootElement, Stringf( "Root Element is not found!" ) );

    XmlElement* animSetDefElement = rootElement->FirstChildElement();
    while ( animSetDefElement )
    {
        std::string animSetDefElementName = animSetDefElement->Name();
        GUARANTEE_OR_DIE( animSetDefElementName == "AnimationSetDefinition", Stringf( "Expected AnimationSetDefinition, but found: %s", animSetDefElementName.c_str() ) );

        AnimationSetDefinition* animSet = new AnimationSetDefinition();
        animSet->LoadFromXmlElement( *animSetDefElement );

        XmlElement* animClipElement = animSetDefElement->FirstChildElement();
        GUARANTEE_OR_DIE( animClipElement, "AnimationClip is not found" );

        while ( animClipElement )
        {
            std::string animClipElementName = animClipElement->Name();
            GUARANTEE_OR_DIE( animClipElementName == "AnimationClip", Stringf( "Expected AnimationClip, but found: %s", animClipElementName.c_str() ) );

            std::string name = ParseXmlAttribute( *animClipElement, "name", "" );
            GUARANTEE_OR_DIE( !name.empty(), Stringf( "Animation name is not found" ) );

            std::string fbx = ParseXmlAttribute( *animClipElement, "fbx", "" );
            GUARANTEE_OR_DIE( !fbx.empty(), Stringf( "Animation fbx is not found" ) );

            bool           isLooping = ParseXmlAttribute( *animClipElement, "isLooping", true );

            AnimationClip* animClip      = ModelImporter::CreateOrGetAnimationFromFile( fbx );
            animClip->m_isLooping        = isLooping;
            animSet->m_animClips[ name ] = animClip;

            animClipElement = animClipElement->NextSiblingElement();
        }
        s_definitions.push_back( animSet );
        animSetDefElement = animSetDefElement->NextSiblingElement();
    }
}

//-----------------------------------------------------------------------------------------------
void AnimationSetDefinition::ClearDefinitions()
{
    for ( int animSetDefIndex = 0; animSetDefIndex < static_cast< int >( s_definitions.size() ); ++animSetDefIndex )
    {
        AnimationSetDefinition* animSetDef = s_definitions[ animSetDefIndex ];
        if ( animSetDef )
        {
            delete animSetDef;
            animSetDef = nullptr;
        }
    }
    s_definitions.clear();
}

//-----------------------------------------------------------------------------------------------
std::vector< AnimationSetDefinition* > AnimationSetDefinition::s_definitions;