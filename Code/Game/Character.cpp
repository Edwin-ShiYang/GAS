#include "Game/Character.hpp"
#include "Game/Game.hpp"
#include "Game/CharacterAnimationController.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/SkeletalMeshDefinition.hpp"
#include "Engine/Core/Engine.hpp"
#include "Actor.hpp"
#include "Engine/RenderConstants.hpp"

//-----------------------------------------------------------------------------------------------
Character::Character( Game* game, std::string const& name )
    : SkeletalMeshActor( game, name )
{
    m_animationController = new CharacterAnimationController( m_game->m_clock, this );
}

//-----------------------------------------------------------------------------------------------
Character::~Character()
{
}

//-----------------------------------------------------------------------------------------------
void Character::Update()
{
    m_animationController->Update();
}

//-----------------------------------------------------------------------------------------------
void Character::Render() const
{
    g_engine->m_render->BindShader( ShaderType::PBRLitSkinned );

    g_engine->m_render->SetMaterialConstants( m_actorDef->m_skeletalMeshDef->m_metallic, m_actorDef->m_skeletalMeshDef->m_roughness, m_actorDef->m_skeletalMeshDef->m_ambientOcclusion, m_actorDef->m_skeletalMeshDef->m_emissiveIntensity );

    g_engine->m_render->SetSkinConstant( m_skinMatrices );

    for ( int nodeIndex = 0; nodeIndex < static_cast< int >( m_skeletonModel->m_nodes.size() ); ++nodeIndex )
    {
        Node const* node = &m_skeletonModel->m_nodes[ nodeIndex ];
        for ( int meshIndex = 0; meshIndex < static_cast< int >( node->m_meshIndexes.size() ); ++meshIndex )
        {
            SkeletonMeshSection const& section       = m_skeletonModel->m_skeletonMesh.m_sections[ node->m_meshIndexes[ meshIndex ] ];
            int                        materialIndex = section.m_materialIndex;
            Material const*            material      = &m_skeletonModel->m_materials[ materialIndex ];

            Mat44                      modelToWorldTransform = GetModelToWorldTransform();
            g_engine->m_render->SetModelConstants( modelToWorldTransform );

            g_engine->m_render->BindTextureWithSampler( { material->m_diffuseTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot::DIFFUSE } );
            g_engine->m_render->BindTextureWithSampler( { g_defaultNormalTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot ::NORMAL } );
            g_engine->m_render->BindTextureWithSampler( { g_defaultSGETexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot ::SPEC_GLOSS_EMIT } );
            g_engine->m_render->BindTextureWithSampler( { g_defaultAmbientOcclusionTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot ::AMBIENT_OCCLUSION } );
            g_engine->m_render->BindTextureWithSampler( { g_defaultMetallicTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot ::METALLIC } );
            g_engine->m_render->BindTextureWithSampler( { g_defaultRoughnessTexture, SamplerMode::POINT_CLAMP, ShaderResourceSlot ::ROUGHNESS } );

            g_engine->m_render->DrawIndexedVertexBuffer( section.m_vertexBuffer, section.m_indexBuffer, static_cast< unsigned int >( section.m_indices.size() ) );

            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::DIFFUSE );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::NORMAL );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::SPEC_GLOSS_EMIT );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::AMBIENT_OCCLUSION );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::METALLIC );
            g_engine->m_render->UnbindTexture( ShaderResourceSlot ::ROUGHNESS );
        }
    }

    g_engine->m_render->BindShader( g_engine->m_render->m_defaultShader );
}