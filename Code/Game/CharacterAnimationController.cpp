#include "Game/CharacterAnimationController.hpp"
#include "Game/Character.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Clock.hpp"

//-----------------------------------------------------------------------------------------------
CharacterAnimationController::CharacterAnimationController( Clock* parentClock, Character* owner )
    : AnimationController( parentClock )
    , m_owner( owner )
{
}

//-----------------------------------------------------------------------------------------------
void CharacterAnimationController::Update()
{
    float deltaSeconds = static_cast< float >( m_clock->GetDeltaSeconds() );
}

//-----------------------------------------------------------------------------------------------
void CharacterAnimationController::UpdatePose( Node* node, Mat44 parentTransform )
{
    SkeletonModel* skeletonModel  = m_owner->m_skeletonModel;
    int            jointIndex     = ModelImporter::GetBoneIndexByName( skeletonModel->m_skeleton, node->m_name );
    Mat44          worldTransform = parentTransform;
    Mat44          localTransform;
    if ( jointIndex >= 0 )
    {
        Joint& joint   = skeletonModel->m_skeleton.m_joints[ jointIndex ];
        localTransform = m_pose.m_localTransforms[ jointIndex ];
        worldTransform.Append( localTransform );

        joint.m_worldTransform = worldTransform;
        joint.m_skinMatrix     = worldTransform;
        joint.m_skinMatrix.Append( joint.m_inverseBindMatrix );
    }
    else
    {
        worldTransform.Append( node->m_localTransform );
    }

    for ( unsigned int i = 0; i < static_cast< int >( node->m_childrenIndices.size() ); ++i )
    {
        UpdatePose( &skeletonModel->m_nodes[ node->m_childrenIndices[ i ] ], worldTransform );
    }
}