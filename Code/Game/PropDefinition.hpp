#pragma once

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"

//-----------------------------------------------------------------------------------------------
class SkeletalMeshDefinition;
class AnimationSetDefinition;
class AbilitySystemComponentDefinition;

//-----------------------------------------------------------------------------------------------
class PropDefinition
{
public:
    PropDefinition() = default;
    ~PropDefinition();

    void                                    LoadFromXmlElement( XmlElement const& element );

    static void                             InitializeDefinitions();
    static void                             ClearDefinitions();
    static PropDefinition const*            GetDefinitionById( std::string const& id );
    static std::vector< PropDefinition* >   s_definitions;

    SkeletalMeshDefinition*                 m_skeletalMeshDef = nullptr;
    AnimationSetDefinition*                 m_animSetDef      = nullptr;
    AbilitySystemComponentDefinition const* m_ascDef          = nullptr;
    Vec3                                    m_spawnPosition   = Vec3::ZERO;

    std::string                             m_id;
    std::string                             m_staticMeshId;
};