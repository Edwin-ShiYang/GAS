#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class StaticMeshDefinition;
class SkeletalMeshDefinition;
class AnimationSetDefinition;

//-----------------------------------------------------------------------------------------------
class ActorDefinition
{
public:
    ActorDefinition() = default;
    ~ActorDefinition();

    void                                   LoadFromXmlElement( XmlElement const& element );

    static void                            InitializeDefinitions();
    static void                            ClearDefinitions();
    static ActorDefinition const*          GetDefinitionByName( std::string const& name );
    static std::vector< ActorDefinition* > s_definitions;

    StaticMeshDefinition*                  m_staticMeshDef   = nullptr;
    SkeletalMeshDefinition*                m_skeletalMeshDef = nullptr;
    AnimationSetDefinition*                m_animSetDef      = nullptr;
    std::string                            m_name            = "Unknown";
    Vec3                                   m_spawnPosition   = Vec3::ZERO;
};