#include "Game/Equipment.hpp"
#include "Game/StaticMeshDefinition.hpp"
#include "Game/EquipmentDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Engine.hpp"
#include "Engine/GameFramework/StaticMeshComponent.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"

//-----------------------------------------------------------------------------------------------
Equipment::Equipment( Actor* owner, EquipmentDefinition const& equipmentDef )
    : m_owner( owner )
    , m_weaponDef( equipmentDef )
    , m_staticMeshDef( StaticMeshDefinition::GetDefinitionById( equipmentDef.m_staticMeshId ) )
{
    m_position    = equipmentDef.m_position;
    m_orientation = equipmentDef.m_orientation;
    m_socketName  = equipmentDef.m_socketName;

    StaticModel const& staticModel = g_engine->m_modelAssets->CreateOrGetStaticModel( m_staticMeshDef.m_filePath );
    AddComponent( new StaticMeshComponent( this, staticModel ) );

    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_staticMeshDef.m_axes.at( "x" ), m_staticMeshDef.m_axes.at( "y" ), m_staticMeshDef.m_axes.at( "z" ) );
}

//-----------------------------------------------------------------------------------------------
void Equipment::Update()
{
    SkeletalMeshComponent* sm     = m_owner->GetComponentByClass< SkeletalMeshComponent >();
    m_rightHandNodeWorldTransform = sm->GetJointToModelTransform( m_socketName );
}

//-----------------------------------------------------------------------------------------------
void Equipment::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->SetMaterialConstants( m_staticMeshDef.m_metallic, m_staticMeshDef.m_roughness, m_staticMeshDef.m_ambientOcclusion, m_staticMeshDef.m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->SetMaterialConstants();
    g_engine->m_render->BindShader( ShaderType::Default );
}

void Equipment::RenderShadow() const
{
    g_engine->m_render->BindShader( ShaderType::ShadowMap );
    Actor::Render();
    g_engine->m_render->BindShader( ShaderType::Default );
}

//-----------------------------------------------------------------------------------------------
Mat44 Equipment::GetModelToWorldTransform() const
{
    Mat44 finalMatrix = m_owner->GetModelToWorldTransform();

    finalMatrix.Append( m_rightHandNodeWorldTransform );
    finalMatrix.Append( m_toEngineMatrix );

    Mat44 offsetMatrix;
    offsetMatrix.AppendTranslation3D( m_position );
    offsetMatrix.Append( m_orientation.GetAsMatrix_IFwd_JLeft_KUp() );
    finalMatrix.Append( offsetMatrix );

    return finalMatrix;
}