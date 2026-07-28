#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AnimationController.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec4.hpp"

//-----------------------------------------------------------------------------------------------
class Clock;
class Character;
class AnimationClip;
struct Node;

//-----------------------------------------------------------------------------------------------
class CharacterAnimationController : public AnimationController
{
public:
    CharacterAnimationController( Clock* parentClock, Character* owner );
    ~CharacterAnimationController() = default;

    void  Update() override;

    Mat44 CreateRotationMatrixFromQuat( Vec4 const& quat ) const;
    void  SamplePose( Pose& pose, AnimationClip* animClip, float sampleTime ) const;
    void  UpdatePose( Node* node, Mat44 parentTransform );

public:
    Character* m_owner                   = nullptr;
    float      m_currentAnimTimeSeconds  = 0.f;
    float      m_previousAnimTimeSeconds = 0.f;
    float      m_blendElapsedSeconds     = 0.f;
};