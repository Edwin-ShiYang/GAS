#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/GameFramework/Actor.hpp"
#include "string"

//-----------------------------------------------------------------------------------------------
class AnimationClip;
class Clock;
class CharacterAnimationController;
class AbilitySystemComponent;
class AttributeSet;
class PropDefinition;
class Game;

//-----------------------------------------------------------------------------------------------
class Character : public Actor
{
public:
    Character( Game* game, std::string const& name );
    ~Character();

    void                    Update() override;
    void                    Render() const override;

    AbilitySystemComponent* GetAbilitySystemComponent() const;
    AttributeSet*           GetAttributeSet() const;
    void                    GrantDefaultAbilities();
    void                    PlayAbilityAnimation( std::string const& animationName );
    Mat44                   GetModelToWorldTransform() const override;

public:
    Mat44                         m_toEngineMatrix;
    CharacterAnimationController* m_animationController = nullptr;
    AbilitySystemComponent*       m_asc                 = nullptr;
    PropDefinition const*         m_actorDef            = nullptr;
    Game*                         m_game                = nullptr;
};