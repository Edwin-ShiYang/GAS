#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>

//-----------------------------------------------------------------------------------------------
class StaticMeshDefinition;

//-----------------------------------------------------------------------------------------------
class Weapon : public Actor
{
public:
    Weapon( Actor* owner, StaticMeshDefinition const& staticMeshDef );
    ~Weapon() = default;

    void  Update() override;
    void  Render() const override;
    Mat44 GetModelToWorldTransform() const override;

public:
    Mat44                       m_toEngineMatrix;
    Actor*                      m_owner = nullptr;

    StaticMeshDefinition const& m_staticMeshDef;
    Mat44                       m_rightHandNodeWorldTransform;
    std::string                 m_socketName;
};