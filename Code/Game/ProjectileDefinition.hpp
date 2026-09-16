#pragma once

//-----------------------------------------------------------------------------------------------
#include <vector>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

//-----------------------------------------------------------------------------------------------
enum class ProjectileVisualType
{
    INVALID,
    BILLBOARD,
    MESH
};

//-----------------------------------------------------------------------------------------------
class ProjectileDefinition
{
public:
    ProjectileDefinition()  = default;
    ~ProjectileDefinition() = default;

    void                                        LoadFromXmlElement( XmlElement const& element );
    static void                                 InitializeDefinitions();
    static ProjectileDefinition const&          GetDefinitionById( std::string const& id );
    static void                                 ClearDefinitions();
    static std::vector< ProjectileDefinition* > s_definitions;

    std::string                                 m_id;
    std::string                                 m_billboardType;
    std::string                                 m_playbackMode;
    std::string                                 m_staticMeshId;
    std::string                                 m_spriteSheetPath;
    IntVec2                                     m_gridLayout;
    int                                         m_startFrame;
    int                                         m_endFrame;
    float                                       m_framePerSecond;
    ProjectileVisualType                        m_visualType = ProjectileVisualType::INVALID;
    float                                       m_duration   = 0.f;
};