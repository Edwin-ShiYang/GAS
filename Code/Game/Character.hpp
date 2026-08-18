#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>

//-----------------------------------------------------------------------------------------------
class AbilitySystemComponent;
class AttributeSet;
class CharacterDefinition;
class SkeletalMeshDefinition;
class Game;
class PlayerController;
class Weapon;

//-----------------------------------------------------------------------------------------------
class Character : public Actor
{
public:
    Character( Game* game, std::string const& name );
    ~Character() override;

    void                    Update() override;
    void                    Render() const override;
    Mat44                   GetModelToWorldTransform() const override;

    void                    PossessedBy( PlayerController* playerController );

    AbilitySystemComponent* GetAbilitySystemComponent() const;
    AttributeSet*           GetAttributeSet() const;

public:
    Game*                         m_game             = nullptr;
    AbilitySystemComponent*       m_asc              = nullptr;
    CharacterDefinition const*    m_characterDef     = nullptr;
    SkeletalMeshDefinition const* m_skeletalMeshDef  = nullptr;
    PlayerController*             m_playerController = nullptr;

    Mat44                         m_toEngineMatrix;
    Vec3                          m_velocity       = Vec3::ZERO;
    Vec3                          m_mouseTargetPos = Vec3::ZERO;

    std::vector< Weapon* >        m_weapons;
};