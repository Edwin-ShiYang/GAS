#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "Engine/AbilitySystem/AbilitySystemComponent.hpp"

//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Engine/Core/Core.hpp"
#include "ProjectileSpawnPoint.hpp"
#include "GameCommon.hpp"

//-----------------------------------------------------------------------------------------------
class AttributeSet;
class CharacterDefinition;
class SkeletalMeshDefinition;
class Game;
class PlayerController;
class Controller;
class Equipment;

//-----------------------------------------------------------------------------------------------
class Character : public Actor
{
public:
    Character( Game* game, CharacterDefinition const& characterDef );
    ~Character() override;

    void Update() override;
    void Render() const override;
    void RenderIndicator() const;

    void RenderShadow() const;
    void PossessedBy( Controller* playerController );
    Vec2 GetPositionXY();
    void SetPositionXY( Vec2 positionXY );
    bool IsDead() const;

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
    Controller*                   m_controller = nullptr;
    std::vector< Equipment* >     m_equipments;
    ProjectileSpawnPoint          m_projectileSpawnPoint;
    Faction                       m_faction = Faction::UNKNOWN;
};