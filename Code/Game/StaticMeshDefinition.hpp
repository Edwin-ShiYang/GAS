#pragma once

//-----------------------------------------------------------------------------------------------
#include <map>
#include <string>

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Model/ModelImporter.hpp"

//-----------------------------------------------------------------------------------------------
class StaticMeshDefinition
{
public:
    StaticMeshDefinition()  = default;
    ~StaticMeshDefinition() = default;

    void                                                        LoadFromXmlElement( XmlElement const& element );
    static void                                                 InitializeDefinitions();
    static void                                                 ClearDefinitions();
    static StaticMeshDefinition const*                          GetDefinitionById( std::string const& staticMeshId );
    static std::map< std::string, StaticMeshDefinition const* > s_definitions;

private:
    Axis ParseCoordinateAxis( std::string axis );

public:
    std::string                   m_id;
    std::string                   m_filePath;
    std::map< std::string, Axis > m_axes;
    float                         m_metallic          = 0.f;
    float                         m_roughness         = 0.f;
    float                         m_ambientOcclusion  = 1.f;
    float                         m_emissiveIntensity = 0.f;
};