#pragma once
#include "SkeletalMeshActor.hpp"

//-----------------------------------------------------------------------------------------------
class AnimationClip;
class Clock;
class Node;

//-----------------------------------------------------------------------------------------------
class Character : public SkeletalMeshActor
{
public:
    Character( Game* game, std::string const& name );
    ~Character();

    void Update() override;
    void Render() const override;
};