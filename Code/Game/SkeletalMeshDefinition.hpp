#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Model/ModelImporter.hpp"

class AnimationSetDefinition;

//-----------------------------------------------------------------------------------------------
class SkeletalMeshDefinition
{
public:
    SkeletalMeshDefinition()  = default;
    ~SkeletalMeshDefinition() = default;
    void LoadFromXmlElement( XmlElement const& element );

private:
    Axis ParseCoordinateAxis( std::string axis );

public:
    std::string                   m_filePath;
    std::map< std::string, Axis > m_axes;

    float                         m_metallic          = 0.f;
    float                         m_roughness         = 0.f;
    float                         m_ambientOcclusion  = 1.f;
    float                         m_emissiveIntensity = 0.f;
};