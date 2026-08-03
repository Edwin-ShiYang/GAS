#include "Game/Prop.hpp"
#include "Game/PropDefinition.hpp"
#include "Game/StaticMeshDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Engine.hpp"
#include "Engine/Model/ModelImporter.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/StaticMeshComponent.hpp"

//-----------------------------------------------------------------------------------------------
Prop::Prop( Game* game, std::string id )
    : m_game( game )
{
    m_propDef = PropDefinition::GetDefinitionById( id );
    GUARANTEE_OR_DIE( m_propDef, Stringf( "PropDefinition is not found" ) )

    m_staticMeshDef = StaticMeshDefinition::GetDefinitionById( m_propDef->m_staticMeshId );
    GUARANTEE_OR_DIE( m_staticMeshDef, Stringf( "StaticMeshDefinition is not found" ) )

    StaticModel* staticModel = ModelImporter::CreateOrGetStaticModelFromFile( m_staticMeshDef->m_filePath );
    GUARANTEE_OR_DIE( staticModel, Stringf( "StaticModel is not found" ) )

    AddComponent( new StaticMeshComponent( this, staticModel ) );
    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_staticMeshDef->m_axes.at( "x" ), m_staticMeshDef->m_axes.at( "y" ), m_staticMeshDef->m_axes.at( "z" ) );
    m_position       = m_propDef->m_spawnPosition;
}

//-----------------------------------------------------------------------------------------------
Prop::~Prop()
{
}

//-----------------------------------------------------------------------------------------------
void Prop::Update()
{
}

//-----------------------------------------------------------------------------------------------
void Prop::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->SetMaterialConstants( m_staticMeshDef->m_metallic, m_staticMeshDef->m_roughness, m_staticMeshDef->m_ambientOcclusion, m_staticMeshDef->m_emissiveIntensity );

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