#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

class AnimationSetDefinition;
class AbilitySystemComponentDefinition;
class AnimationGraphDefinition;

//-----------------------------------------------------------------------------------------------
class CharacterDefinition
{
public:
    CharacterDefinition()  = default;
    ~CharacterDefinition() = default;

    void                                       LoadFromXmlElement( XmlElement const& element );

    static void                                InitializeDefinitions();
    static void                                ClearDefinitions();
    static CharacterDefinition const*          GetDefinitionById( std::string const& id );
    static std::vector< CharacterDefinition* > s_definitions;

    std::string                                m_id;
    std::string                                m_skeletalMesh;

    AbilitySystemComponentDefinition const*    m_ascDef            = nullptr;
    AnimationSetDefinition*                    m_animationSetDef   = nullptr;
    AnimationGraphDefinition const*            m_animationGraphDef = nullptr;
};