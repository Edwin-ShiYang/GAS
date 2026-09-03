#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include "CharacterDefinition.hpp"

//-----------------------------------------------------------------------------------------------
class StaticMeshDefinition;

//-----------------------------------------------------------------------------------------------
class Weapon : public Actor
{
public:
    Weapon( Actor* owner, WeaponDefinition const& weaponDef );
    ~Weapon() = default;

    void  Update() override;
    void  Render() const override;
    void  RenderShadow() const;
    Mat44 GetModelToWorldTransform() const override;

public:
    Mat44                       m_toEngineMatrix;
    Actor*                      m_owner = nullptr;

    WeaponDefinition const&     m_weaponDef;
    StaticMeshDefinition const& m_staticMeshDef;
    Mat44                       m_rightHandNodeWorldTransform;
    std::string                 m_socketName;
};