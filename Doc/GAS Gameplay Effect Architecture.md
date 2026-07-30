---
title: GAS Gameplay Effect Architecture
tags:
  - gas
  - architecture
  - gameplay-effect
---

# GAS Gameplay Effect Architecture

## Core Idea

Use three separate concepts:

```text
GameplayEffectDefinition
GameplayEffect
ActiveGameplayEffect
```

Do not use one vague `GameplayEffect` class for every purpose. The template, one-time application data, and active runtime state are different things.

## Unreal Mapping

The simplified mapping to Unreal GAS is:

```text
Unreal UGameplayEffect       -> GameplayEffectDefinition
Unreal FGameplayEffectSpec   -> GameplayEffect
Unreal FActiveGameplayEffect -> ActiveGameplayEffect
```

## GameplayEffectDefinition

`GameplayEffectDefinition` is the data template loaded from XML.

It describes what the effect does:

```text
name
duration policy
duration seconds
period seconds
modifiers
```

Example:

```xml
<GameplayEffectDefinition name="Damage20" durationPolicy="Instant">
    <Modifier attribute="Health" operation="Add" value="-20.0" />
</GameplayEffectDefinition>

<GameplayEffectDefinition name="Burning" durationPolicy="HasDuration" duration="4.0" period="1.0">
    <Modifier attribute="Health" operation="Add" value="-5.0" />
</GameplayEffectDefinition>
```

This is shared data. It should not store per-actor runtime state.

## GameplayEffect

`GameplayEffect` is one created effect instance prepared for application.

It points to a definition and stores runtime context for this application:

```cpp
class GameplayEffect
{
public:
    GameplayEffectDefinition const* m_definition = nullptr;

    AbilitySystemComponent* m_sourceASC = nullptr;
    AbilitySystemComponent* m_targetASC = nullptr;

    float m_level = 1.f;
};
```

Example meaning:

```text
This Damage20 came from attacker ASC,
targets DarkLord ASC,
and is being applied at level 1.
```

This is not loaded directly from XML. It is created from a `GameplayEffectDefinition`.

## ActiveGameplayEffect

`ActiveGameplayEffect` is a gameplay effect that has already been applied and is still active on an ASC.

It owns time/stack runtime state:

```cpp
struct ActiveGameplayEffect
{
    GameplayEffect m_effect;

    float m_remainingSeconds = 0.f;
    float m_timeUntilNextTick = 0.f;
    int   m_stackCount = 1;
};
```

Only duration or infinite effects should become active effects.

Instant effects execute immediately and do not enter the active effect list.

`ActiveGameplayEffect` should stay separate from `GameplayEffect`.

`GameplayEffect` represents one application/spec:

```text
which definition
source ASC
target ASC
level
```

`ActiveGameplayEffect` represents ASC-owned active runtime state:

```text
remaining time
time until next periodic tick
stack count
```

Do not put active lifetime state directly into `GameplayEffect`. That would mix apply-time data with ASC-owned duration state. Instant effects would also carry duration fields they do not need.

It is fine to skip implementing `ActiveGameplayEffect` while only instant effects exist. Add it when duration, infinite, periodic, stacking, or removable effects are introduced.

## ASC Ownership

`AbilitySystemComponent` should own active gameplay effects:

```cpp
std::vector< ActiveGameplayEffect > m_activeGameplayEffects;
```

Runtime flow:

```text
GameplayEffectDefinition
-> create GameplayEffect
-> ASC ApplyGameplayEffect
-> if Instant: execute immediately
-> if HasDuration or Infinite: create ActiveGameplayEffect
```

## Apply Flow

Conceptual flow:

```cpp
GameplayEffectDefinition const* def =
    GameplayEffectDefinition::GetDefinitionByName( "Damage20" );

GameplayEffect effect;
effect.m_definition = def;
effect.m_sourceASC = attackerASC;
effect.m_targetASC = targetASC;
effect.m_level = 1.f;

targetASC->ApplyGameplayEffect( effect );
```

Inside ASC:

```cpp
void AbilitySystemComponent::ApplyGameplayEffect( GameplayEffect const& effect )
{
    GameplayEffectDefinition const* def = effect.m_definition;

    if ( def->m_durationPolicy == GameplayEffectDurationType::Instant )
    {
        ExecuteGameplayEffect( effect );
    }
    else
    {
        ActiveGameplayEffect activeEffect;
        activeEffect.m_effect = effect;
        activeEffect.m_remainingSeconds = def->m_durationSeconds;
        activeEffect.m_timeUntilNextTick = def->m_periodSeconds;

        m_activeGameplayEffects.push_back( activeEffect );
    }
}
```

## Apply vs Execute

`ApplyGameplayEffect` is the public entry point.

It receives a `GameplayEffect`, checks the definition, decides what kind of effect it is, and routes it:

```text
Instant
    -> execute immediately

HasDuration / Infinite
    -> create ActiveGameplayEffect
    -> store it in m_activeGameplayEffects
```

`ExecuteInstantGameplayEffect` is the numeric execution step.

It applies the definition's modifiers to the target ASC's `AttributeSet`:

```cpp
void AbilitySystemComponent::ExecuteInstantGameplayEffect( GameplayEffect const& effect )
{
    GameplayEffectDefinition const* def = effect.m_definition;
    AbilitySystemComponent* targetASC = effect.m_targetASC;

    for ( GameplayModifierDefinition const* modifier : def->m_modifiers )
    {
        float currentValue = targetASC->GetCurrentValue( modifier->m_attributeName );

        if ( modifier->m_operation == GameplayModifierOperation::Add )
        {
            currentValue += modifier->m_magnitude;
        }

        targetASC->SetCurrentValue( modifier->m_attributeName, currentValue );
    }
}
```

The split is:

```text
ApplyGameplayEffect = validation, routing, lifecycle decision
ExecuteInstantGameplayEffect = modifier math and attribute mutation
```

Keep them separate so duration effects, periodic effects, stacking, and instant effects do not all get mixed into one function.

## Naming Rule

Use these names with these meanings:

```text
GameplayEffectDefinition = template from XML
GameplayEffect = one effect application/spec
ActiveGameplayEffect = active runtime effect on an ASC
```

This keeps the system from mixing shared data, apply-time context, and active duration state.
