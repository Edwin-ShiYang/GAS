#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
class AnimationSetDefinition;

//-----------------------------------------------------------------------------------------------
class SkeletalMeshDefinition
{
public:
    SkeletalMeshDefinition()  = default;
    ~SkeletalMeshDefinition() = default;

    void                                                          LoadFromXmlElement( XmlElement const& element );
    static void                                                   InitializeDefinitions();
    static void                                                   ClearDefinitions();
    static SkeletalMeshDefinition const*                          GetDefinitionById( std::string const& skeletalMeshId );
    static std::map< std::string, SkeletalMeshDefinition const* > s_definitions;

private:
    Axis ParseCoordinateAxis( std::string axis );

public:
    std::string                   m_id;
    std::string                   m_filePath;
    std::map< std::string, Axis > m_axes;
    float                         m_metallic;
    float                         m_roughness;
    float                         m_ambientOcclusion;
    float                         m_emissiveIntensity;
};