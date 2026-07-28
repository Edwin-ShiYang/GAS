#include "Game/Primitive.hpp"
#include "Game/Game.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Engine.hpp"

//-----------------------------------------------------------------------------------------------
Primitive::Primitive( Game* game )
    : m_game( game )
{
}

//-----------------------------------------------------------------------------------------------
Primitive::~Primitive()
{
    delete m_vertexBuffer;
    m_vertexBuffer = nullptr;

    delete m_indexBuffer;
    m_indexBuffer = nullptr;
}

//-----------------------------------------------------------------------------------------------
void Primitive::Render() const
{
    Mat44 modelToWorldTransform = GetModelToWorldTransform();
    g_engine->m_render->SetModelConstants( modelToWorldTransform, Rgba8::WHITE );

    g_engine->m_render->BindShader( ShaderType::PBRLitStatic );

    g_engine->m_render->BindTexture( m_diffuse, ResourceSlot::DIFFUSE );
    g_engine->m_render->BindTexture( g_engine->m_render->m_defaultNormalTexture, ResourceSlot::NORMAL );
    g_engine->m_render->BindTexture( g_engine->m_render->m_defaultRoughnessTexture, ResourceSlot::ROUGHNESS );
    g_engine->m_render->BindTexture( g_engine->m_render->m_defaultMetallicTexture, ResourceSlot::METALLIC );
    g_engine->m_render->BindTexture( g_engine->m_render->m_defaultAmbientOcclusionTexture, ResourceSlot::AMBIENT_OCCLUSION );

    g_engine->m_render->DrawIndexedVertexBuffer( m_vertexBuffer, m_indexBuffer, static_cast< unsigned int >( m_indices.size() ) );

    g_engine->m_render->UnbindTexture( ResourceSlot::DIFFUSE );
    g_engine->m_render->UnbindTexture( ResourceSlot::NORMAL );
    g_engine->m_render->UnbindTexture( ResourceSlot::ROUGHNESS );
    g_engine->m_render->UnbindTexture( ResourceSlot::METALLIC );
    g_engine->m_render->UnbindTexture( ResourceSlot::AMBIENT_OCCLUSION );

    g_engine->m_render->BindDefaultShader();
}

//-----------------------------------------------------------------------------------------------
void Primitive::RenderShadow() const
{
    Mat44 modelToWorldTransform = GetModelToWorldTransform();
    g_engine->m_render->SetModelConstants( modelToWorldTransform, Rgba8::WHITE );
    g_engine->m_render->DrawIndexedVertexBuffer( m_vertexBuffer, m_indexBuffer, static_cast< unsigned int >( m_indices.size() ) );
}

//-----------------------------------------------------------------------------------------------
void Primitive::Update()
{
}

//-----------------------------------------------------------------------------------------------
Mat44 Primitive::GetModelToWorldTransform() const
{
    Mat44 modelToWorldTransform;

    Mat44 rotation = m_orientation.GetAsMatrix_IFwd_JLeft_KUp();
    rotation.Orthonormalize_XFwd_YLeft_ZUp();

    Mat44 scaleMatrix = Mat44::MakeNonUniformScale3D( m_scale );

    modelToWorldTransform.AppendTranslation3D( m_position );
    modelToWorldTransform.Append( rotation );
    modelToWorldTransform.Append( scaleMatrix );
    return modelToWorldTransform;
}

//-----------------------------------------------------------------------------------------------
void Primitive::CreateVertexBuffer()
{
    unsigned int vertexBufferStride = sizeof( Vertex );
    unsigned int vertexBufferSize   = static_cast< unsigned int >( m_vertices.size() ) * vertexBufferStride;

    m_vertexBuffer = g_engine->m_render->CreateVertexBuffer( vertexBufferSize, vertexBufferStride );
    g_engine->m_render->CopyCPUToGPU( m_vertices.data(), static_cast< unsigned int >( m_vertices.size() ) * sizeof( Vertex ), m_vertexBuffer );
}

//-----------------------------------------------------------------------------------------------
void Primitive::CreateIndexBuffer()
{
    unsigned int indexBufferStride = sizeof( unsigned int );
    unsigned int indexBufferSize   = static_cast< unsigned int >( m_indices.size() ) * indexBufferStride;

    m_indexBuffer = g_engine->m_render->CreateIndexBuffer( indexBufferSize, indexBufferStride );
    g_engine->m_render->CopyCPUToGPU( m_indices.data(), static_cast< unsigned int >( m_indices.size() ) * sizeof( unsigned int ), m_indexBuffer );
}

//-----------------------------------------------------------------------------------------------
void Primitive::SetUniformScale( float scale )
{
    m_scale = Vec3( scale, scale, scale );
}

//-----------------------------------------------------------------------------------------------
void Primitive::SetNonUniformScale( Vec3 const& scale )
{
    m_scale = Vec3( scale.x, scale.y, scale.z );
}