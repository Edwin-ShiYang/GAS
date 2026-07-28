#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/AnimationController.hpp"
#include "Engine/Math/Mat44.hpp"

//-----------------------------------------------------------------------------------------------
class Clock;
class Character;
struct Node;

//-----------------------------------------------------------------------------------------------
class CharacterAnimationController : public AnimationController
{
public:
    CharacterAnimationController( Clock* parentClock, Character* owner );
    ~CharacterAnimationController() = default;

    void Update() override;
    void UpdatePose( Node* node, Mat44 parentTransform );

public:
    Character* m_owner                   = nullptr;
    float      m_currentAnimTimeSeconds  = 0.f;
    float      m_previousAnimTimeSeconds = 0.f;
    float      m_blendElapsedSeconds     = 0.f;
};