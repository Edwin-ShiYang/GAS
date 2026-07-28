#pragma once

//-----------------------------------------------------------------------------------------------
#include "Game/Actor.hpp"
#include "Engine/Math/Mat44.hpp"
#include <vector>

//-----------------------------------------------------------------------------------------------
struct SkeletonModel;

//-----------------------------------------------------------------------------------------------
class SkeletalMeshActor : public Actor
{
public:
    SkeletalMeshActor( Game* game, std::string const& name );
    ~SkeletalMeshActor();

    void Update() override;
    void Render() const override;

protected:
    void RenderMesh() const;

public:
    SkeletonModel*       m_skeletonModel = nullptr;
    std::vector< Mat44 > m_skinMatrices;
};