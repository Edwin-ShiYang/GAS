#include "Game/StaticMeshActor.hpp"
#include "Game/StaticMeshDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"

#include "Engine/Model/ModelImporter.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
StaticMeshActor::StaticMeshActor( Game* game, std::string name )
    : Actor( game, name )
{
    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_actorDef->m_staticMeshDef->m_axes[ "x" ], m_actorDef->m_staticMeshDef->m_axes[ "y" ], m_actorDef->m_staticMeshDef->m_axes[ "z" ] );

    m_staticModel = ModelImporter::CreateOrGetStaticModelFromFile( m_actorDef->m_staticMeshDef->m_filePath );
    GUARANTEE_OR_DIE( m_staticModel, Stringf( "[StaticMeshActor::StaticMeshActor] model is not found" ) )
}

//-----------------------------------------------------------------------------------------------
void StaticMeshActor::Update()
{
}

//-----------------------------------------------------------------------------------------------
void StaticMeshActor::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );

    StaticMeshDefinition const* staticMeshDef = m_actorDef->m_staticMeshDef;
    g_engine->m_render->SetMaterialConstants( staticMeshDef->m_metallic, staticMeshDef->m_roughness, staticMeshDef->m_ambientOcclusion, staticMeshDef->m_emissiveIntensity );

    Mat44 modelToWorldTransform = GetModelToWorldTransform();
    modelToWorldTransform.Append( m_toEngineMatrix );

    m_staticModel->m_staticMesh.Render( m_staticModel, modelToWorldTransform );

    g_engine->m_render->SetMaterialConstants();
    g_engine->m_render->BindShader( ShaderType::Default );
}