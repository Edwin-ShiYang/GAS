#include "Game/Weapon.hpp"
#include "Game/StaticMeshDefinition.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/GameFramework/StaticMeshComponent.hpp"
#include "Engine/GameFramework/SkeletalMeshComponent.hpp"

//-----------------------------------------------------------------------------------------------
Weapon::Weapon( Actor* owner, StaticMeshDefinition const& staticMeshDef )
    : m_owner( owner )
    , m_staticMeshDef( staticMeshDef )
{
    StaticModel const& staticModel = g_engine->m_modelAssets->CreateOrGetStaticModel( m_staticMeshDef.m_filePath );
    AddComponent( new StaticMeshComponent( this, staticModel ) );

    m_toEngineMatrix = ModelImporter::MakeToEngineMatrix( m_staticMeshDef.m_axes.at( "x" ), m_staticMeshDef.m_axes.at( "y" ), m_staticMeshDef.m_axes.at( "z" ) );
}

//-----------------------------------------------------------------------------------------------
void Weapon::Update()
{
    SkeletalMeshComponent* sm     = m_owner->GetComponentByClass< SkeletalMeshComponent >();
    m_rightHandNodeWorldTransform = sm->GetJointToModelTransform( m_socketName );
}

//-----------------------------------------------------------------------------------------------
void Weapon::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );
    g_engine->m_render->SetMaterialConstants( m_staticMeshDef.m_metallic, m_staticMeshDef.m_roughness, m_staticMeshDef.m_ambientOcclusion, m_staticMeshDef.m_emissiveIntensity );

    Actor::Render();

    g_engine->m_render->SetMaterialConstants();
    g_engine->m_render->BindShader( ShaderType::Default );
}

//-----------------------------------------------------------------------------------------------
Mat44 Weapon::GetModelToWorldTransform() const
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