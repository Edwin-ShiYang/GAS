#include "Game/Prop.hpp"
#include "Game/StaticMeshDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Engine.hpp"
#include "Engine/Model/ModelImporter.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/StaticMeshComponent.hpp"

//-----------------------------------------------------------------------------------------------
Prop::Prop( Game* game, StaticMeshDefinition const& staticMeshDef )
    : m_game( game )
    , m_staticMeshDef( staticMeshDef )
{
    StaticModel const& staticModel = g_engine->m_modelAssets->CreateOrGetStaticModel( m_staticMeshDef.m_filePath );
    AddComponent( new StaticMeshComponent( this, staticModel ) );

    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_staticMeshDef.m_axes.at( "x" ), m_staticMeshDef.m_axes.at( "y" ), m_staticMeshDef.m_axes.at( "z" ) );
}

//-----------------------------------------------------------------------------------------------
void Prop::Update()
{
}

//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->SetMaterialConstants( m_staticMeshDef.m_metallic, m_staticMeshDef.m_roughness, m_staticMeshDef.m_ambientOcclusion, m_staticMeshDef.m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->SetMaterialConstants();
    g_engine->m_render->BindShader( ShaderType::Default );
}

//-----------------------------------------------------------------------------------------------
Mat44 Prop::GetModelToWorldTransform() const
{
    Mat44 modelToWorldTransform = Actor::GetModelToWorldTransform();
    modelToWorldTransform.Append( m_toEngineMatrix );

    return modelToWorldTransform;
}