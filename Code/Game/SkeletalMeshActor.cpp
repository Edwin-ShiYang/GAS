#include "Game/SkeletalMeshActor.hpp"
#include "Game/Game.hpp"

#include "Game/SkeletalMeshDefinition.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"

#include "Engine/Model/ModelImporter.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
SkeletalMeshActor::SkeletalMeshActor( Game* game, std::string const& name )
    : Actor( game, name )
{
    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_actorDef->m_skeletalMeshDef->m_axes[ "x" ], m_actorDef->m_skeletalMeshDef->m_axes[ "y" ], m_actorDef->m_skeletalMeshDef->m_axes[ "z" ] );

    m_skeletonModel = ModelImporter::CreateOrGetSkeletonModelFromFile( m_actorDef->m_skeletalMeshDef->m_filePath );
    GUARANTEE_OR_DIE( m_skeletonModel, Stringf( "[SkeletalMeshActor::SkeletalMeshActor] SkeletonModel is not found" ) )

    m_skinMatrices.resize( m_skeletonModel->GetNumJoints() );
}

//-----------------------------------------------------------------------------------------------
SkeletalMeshActor::~SkeletalMeshActor()
{
}

//-----------------------------------------------------------------------------------------------
void SkeletalMeshActor::Update()
{
}

//-----------------------------------------------------------------------------------------------
void SkeletalMeshActor::Render() const
{
}

//-----------------------------------------------------------------------------------------------
void SkeletalMeshActor::RenderMesh() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );

    SkeletalMeshDefinition const* skeletalMeshDef = m_actorDef->m_skeletalMeshDef;
    g_engine->m_render->SetMaterialConstants( skeletalMeshDef->m_metallic, skeletalMeshDef->m_roughness, skeletalMeshDef->m_ambientOcclusion, skeletalMeshDef->m_emissiveIntensity );

    Mat44 modelToWorldTransform = GetModelToWorldTransform();
    modelToWorldTransform.Append( m_toEngineMatrix );

    m_skeletonModel->m_skeletonMesh.RenderMesh( m_skeletonModel, modelToWorldTransform );

    g_engine->m_render->SetMaterialConstants();
    g_engine->m_render->BindShader( ShaderType::Default );
}