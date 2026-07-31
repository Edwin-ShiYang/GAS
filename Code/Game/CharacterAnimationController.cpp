#include "Game/CharacterAnimationController.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/AnimationSetDefinition.hpp"
#include "Game/Character.hpp"

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Clock.hpp"
#include "Engine/Animation/AnimationClip.hpp"
#include "Engine/Math/MathUtils.hpp"

//-----------------------------------------------------------------------------------------------
CharacterAnimationController::CharacterAnimationController( Clock* parentClock, Character* owner )
    : AnimationController( parentClock )
    , m_owner( owner )
{
    int jointCount = static_cast< int >( m_owner->m_skeletonModel->m_skeleton.m_joints.size() );

    m_pose.m_localTransforms.resize( jointCount );
    m_pose.m_translations.resize( jointCount );
    m_pose.m_rotations.resize( jointCount );
    m_pose.m_scales.resize( jointCount );

    m_previousPose.m_localTransforms.resize( jointCount );
    m_previousPose.m_translations.resize( jointCount );
    m_previousPose.m_rotations.resize( jointCount );
    m_previousPose.m_scales.resize( jointCount );

    m_currentPose.m_localTransforms.resize( jointCount );
    m_currentPose.m_translations.resize( jointCount );
    m_currentPose.m_rotations.resize( jointCount );
    m_currentPose.m_scales.resize( jointCount );

    SkeletonModel* model = m_owner->m_skeletonModel;

    for ( int jointIndex = 0; jointIndex < jointCount; ++jointIndex )
    {
        Joint& joint = model->m_skeleton.m_joints[ jointIndex ];

        for ( Node const& node : model->m_nodes )
        {
            if ( node.m_name != joint.m_name )
            {
                continue;
            }

            joint.m_localTransform                 = node.m_localTransform;
            m_pose.m_localTransforms[ jointIndex ] = node.m_localTransform;
            break;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void CharacterAnimationController::Update()
{
    float          deltaSeconds      = static_cast< float >( m_clock->GetDeltaSeconds() );
    AnimationClip* clip              = m_owner->m_actorDef->m_animSetDef->m_animClips[ "Idle" ];
    float          animationDuration = clip->m_duration / clip->m_ticksPerSecond;
    m_currentAnimTimeSeconds += deltaSeconds;
    m_currentAnimTimeSeconds = fmodf( m_currentAnimTimeSeconds, animationDuration );

    SamplePose( m_pose, clip, m_currentAnimTimeSeconds );
    UpdatePose( &m_owner->m_skeletonModel->m_nodes[ 0 ], m_owner->m_toEngineMatrix );
}

//-----------------------------------------------------------------------------------------------
Mat44 CharacterAnimationController::CreateRotationMatrixFromQuat( Vec4 const& quat ) const
{
    Vec4 q = quat;
    q.Normalize();

    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;

    float xy = x * y;
    float xz = x * z;
    float yz = y * z;

    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    Mat44 m;
    auto  iBasis = Vec4( 1.f - 2.f * ( yy + zz ), 2.f * ( xy + wz ), 2.f * ( xz - wy ), 0.f );
    auto  jBasis = Vec4( 2.f * ( xy - wz ), 1.f - 2.f * ( xx + zz ), 2.f * ( yz + wx ), 0.f );
    auto  kBasis = Vec4( 2.f * ( xz + wy ), 2.f * ( yz - wx ), 1.f - 2.f * ( xx + yy ), 0.f );
    auto  tBasis = Vec4( 0.f, 0.f, 0.f, 1.f );
    m.SetIJKT4D( iBasis, jBasis, kBasis, tBasis );

    return m;
}

//-----------------------------------------------------------------------------------------------
void CharacterAnimationController::SamplePose( Pose& pose, AnimationClip* animClip, float sampleTime ) const
{
    std::vector< AnimationTrack > const& tracks = animClip->m_tracks;
    for ( int trackIndex = 0; trackIndex < static_cast< int >( tracks.size() ); ++trackIndex )
    {
        AnimationTrack const& track       = tracks[ trackIndex ];
        Vec3                  translation = track.m_translationKeyFrames[ 0 ].m_value;
        Vec4                  rotation    = track.m_rotationKeyFrames[ 0 ].m_value;
        Vec3                  scale       = track.m_scaleKeyFrames[ 0 ].m_value;

        for ( int translationKeyIndex = 0; translationKeyIndex < static_cast< int >( track.m_translationKeyFrames.size() - 1 ); ++translationKeyIndex )
        {
            if ( track.m_translationKeyFrames[ translationKeyIndex ].m_time <= sampleTime && sampleTime <= track.m_translationKeyFrames[ translationKeyIndex + 1 ].m_time )
            {
                translation = animClip->GetSampleTranslation( track.m_translationKeyFrames[ translationKeyIndex ].m_time, track.m_translationKeyFrames[ translationKeyIndex + 1 ].m_time, track.m_translationKeyFrames[ translationKeyIndex ].m_value, track.m_translationKeyFrames[ translationKeyIndex + 1 ].m_value, sampleTime );
                break;
            }
        }

        for ( int i = 0; i < static_cast< int >( track.m_rotationKeyFrames.size() - 1 ); ++i )
        {
            if ( track.m_rotationKeyFrames[ i ].m_time <= sampleTime && sampleTime <= track.m_rotationKeyFrames[ i + 1 ].m_time )
            {
                Vec4  q0  = track.m_rotationKeyFrames[ i ].m_value;
                Vec4  q1  = track.m_rotationKeyFrames[ i + 1 ].m_value;
                float dot = DotProduct4D( q0, q1 );
                if ( dot < 0.f )
                {
                    q1 = -q1;
                }

                rotation = animClip->GetSampleRotation( track.m_rotationKeyFrames[ i ].m_time, track.m_rotationKeyFrames[ i + 1 ].m_time, q0, q1, sampleTime );
                rotation.Normalize();
                break;
            }
        }

        for ( int i = 0; i < static_cast< int >( track.m_scaleKeyFrames.size() - 1 ); ++i )
        {
            if ( track.m_scaleKeyFrames[ i ].m_time <= sampleTime && sampleTime <= track.m_scaleKeyFrames[ i + 1 ].m_time )
            {
                scale = animClip->GetSampleScale( track.m_scaleKeyFrames[ i ].m_time, track.m_scaleKeyFrames[ i + 1 ].m_time, track.m_scaleKeyFrames[ i ].m_value, track.m_scaleKeyFrames[ i + 1 ].m_value, sampleTime );
                break;
            }
        }

        Mat44 localTransform = Mat44();
        Mat44 rotationMatrix = CreateRotationMatrixFromQuat( rotation );
        localTransform.AppendTranslation3D( translation );
        localTransform.Append( rotationMatrix );
        localTransform.AppendScaleNonUniform3D( scale );

        int boneIndex = ModelImporter::GetBoneIndexByName( m_owner->m_skeletonModel->m_skeleton, track.m_boneName );

        if ( boneIndex < 0 )
        {
            continue;
        }

        pose.m_translations[ boneIndex ] = translation;
        pose.m_rotations[ boneIndex ]    = rotation;
        pose.m_scales[ boneIndex ]       = scale;

        pose.m_localTransforms[ boneIndex ] = localTransform;
    }
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

        joint.m_globalTransform = worldTransform;

        m_owner->m_skinMatrices[ jointIndex ] = worldTransform;
        m_owner->m_skinMatrices[ jointIndex ].Append( joint.m_inverseBindMatrix );

        // joint.m_skinMatrix                    = worldTransform;
        //joint.m_skinMatrix.Append( joint.m_inverseBindMatrix );
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