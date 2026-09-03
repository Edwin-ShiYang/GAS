#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Engine/Core/Core.hpp"

//-----------------------------------------------------------------------------------------------
class AttributeSet;
class CharacterDefinition;
class SkeletalMeshDefinition;
class Game;
class PlayerController;
class Controller;
class Weapon;

//-----------------------------------------------------------------------------------------------
class Character : public Actor
{
public:
    Character( Game* game, CharacterDefinition const& characterDef );
    ~Character() override;

    void Update() override;
    void Render() const override;
    void RenderShadow() const;
    void PossessedBy( Controller* playerController );

    // clang-format off
    Mat44                               GetModelToWorldTransform()  const override;
    CORE_INLINE AbilitySystemComponent* GetAbilitySystemComponent() const { return m_asc; };
    CORE_INLINE AttributeSet*           GetAttributeSet()           const { return m_asc->m_attributeSet; };
    // clang-format on

public:
    Game*                         m_game = nullptr;
    AbilitySystemComponent*       m_asc  = nullptr;
    CharacterDefinition const&    m_characterDef;
    SkeletalMeshDefinition const* m_skeletalMeshDef = nullptr;

    Mat44                         m_toEngineMatrix;
    Vec3                          m_velocity   = Vec3::ZERO;
    Controller*                   m_controller = nullptr;
    std::vector< Weapon* >        m_weapons;
};