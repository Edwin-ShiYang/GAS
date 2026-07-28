#include "Game/Actor.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Model/ModelImporter.hpp"
#include "ActorDefinition.hpp"
#include "StaticMeshDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//-----------------------------------------------------------------------------------------------
Actor::Actor( Game* game, std::string name )
    : m_game( game )
{
    m_actorDef = ActorDefinition::GetDefinitionByName( name );
    GUARANTEE_OR_DIE( m_actorDef, Stringf( "[Actor::Actor] actorDef is not found" ) )

    m_position = m_actorDef->m_spawnPosition;
}

//-----------------------------------------------------------------------------------------------
Actor::~Actor()
{
}

//-----------------------------------------------------------------------------------------------
Mat44 Actor::GetModelToWorldTransform() const
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
void Actor::SetScale( Vec3 const& scale )
{
    m_scale = Vec3( scale.x, scale.y, scale.z );
}