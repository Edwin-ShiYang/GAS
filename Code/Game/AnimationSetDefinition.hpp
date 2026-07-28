#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>

//-----------------------------------------------------------------------------------------------
class AnimationClip;

//-----------------------------------------------------------------------------------------------
class AnimationSetDefinition
{
public:
    AnimationSetDefinition()  = default;
    ~AnimationSetDefinition() = default;

    void                                          LoadFromXmlElement( XmlElement const& element );

    static AnimationSetDefinition*                GetDefinitionByName( std::string const& name );
    static void                                   InitializeDefinitions();
    static void                                   ClearDefinitions();
    static std::vector< AnimationSetDefinition* > s_definitions;

public:
    std::string                             m_name;
    std::map< std::string, AnimationClip* > m_animClips;
};