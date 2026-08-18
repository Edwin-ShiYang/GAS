#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <vector>
#include <memory>

//-----------------------------------------------------------------------------------------------
class Texture;
class Game;
class VertexBuffer;
class IndexBuffer;
struct Rgba8;

//-----------------------------------------------------------------------------------------------
class Primitive
{
public:
    Primitive( Game* owner );
    virtual ~Primitive();

    virtual void Update();
    virtual void Render() const;
    void         RenderShadow() const;
    void         SetUniformScale( float scale );
    void         SetNonUniformScale( Vec3 const& scale );

protected:
    Mat44                           GetModelToWorldTransform() const;
    void                            CreateVertexBuffer();
    void                            CreateIndexBuffer();

    Texture*                        m_diffuse      = nullptr;
    std::unique_ptr< VertexBuffer > m_vertexBuffer = nullptr;
    std::unique_ptr< IndexBuffer >  m_indexBuffer  = nullptr;
    Game*                           m_game         = nullptr;
    std::vector< Vertex >           m_vertices;
    std::vector< unsigned int >     m_indices;

public:
    Vec3        m_position;
    Vec3        m_scale       = Vec3( 1.0f, 1.0f, 1.0f );
    EulerAngles m_orientation = EulerAngles ::ZERO;
};