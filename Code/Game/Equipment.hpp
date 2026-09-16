#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include "CharacterDefinition.hpp"

//-----------------------------------------------------------------------------------------------
class StaticMeshDefinition;

//-----------------------------------------------------------------------------------------------
class Equipment : public Actor
{
public:
    Equipment( Actor* owner, EquipmentDefinition const& weaponDef );
    ~Equipment() = default;

    void  Update() override;
    void  Render() const override;
    void  RenderShadow() const;
    Mat44 GetModelToWorldTransform() const override;

public:
    Mat44                       m_toEngineMatrix;
    Actor*                      m_owner = nullptr;

    EquipmentDefinition const&  m_weaponDef;
    StaticMeshDefinition const& m_staticMeshDef;
    Mat44                       m_rightHandNodeWorldTransform;
    std::string                 m_socketName;
};