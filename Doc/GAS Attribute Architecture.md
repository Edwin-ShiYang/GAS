---
title: GAS Attribute Architecture
tags:
  - gas
  - architecture
  - gameplay-ability-system
---

# GAS Attribute Architecture

## Core Ownership

The runtime ownership should be:

```text
Character / Actor
└── AbilitySystemComponent
    └── AttributeSet
```

`AbilitySystemComponent` manages the runtime `AttributeSet`. `AttributeSetDefinition` and `AbilitySystemComponentDefinition` are data templates loaded from XML, not runtime state.

## Definition vs Runtime

```text
AbilitySystemComponentDefinition
└── AttributeSetDefinition
    └── XML default attribute values

AbilitySystemComponent
└── AttributeSet
    └── runtime attribute values for one actor instance
```

Definitions are shared templates. Runtime `AttributeSet` data must be copied from the definition so each actor instance owns its own values.

## AttributeData

An attribute should not be stored as only one float. It should use an `AttributeData` structure:

```cpp
struct AttributeData
{
    float m_baseValue = 0.f;
    float m_currentValue = 0.f;
};
```

The attribute map can then be:

```cpp
using AttributeName = std::string;
using Attributes = std::map< AttributeName, AttributeData >;
```

## BaseValue vs CurrentValue

`BaseValue` is the attribute value before temporary modifiers.

`CurrentValue` is the actual value right now after damage, healing, buffs, debuffs, or other runtime changes.

```text
BaseValue = what the actor normally has
CurrentValue = what the actor currently has
```

## Initialization

When loading attributes from XML, initialize both values:

```cpp
AttributeData data;
data.m_baseValue = value;
data.m_currentValue = value;
```

Example:

```xml
<Attribute name="Health" value="100.0" />
```

becomes:

```text
Health.BaseValue = 100
Health.CurrentValue = 100
```

## When To Modify BaseValue

Modify `BaseValue` only for permanent, equipment, progression, or configuration changes:

- Level up increases max health.
- Equipping a weapon changes attack power.
- Equipping armor changes defense.
- Loading permanent stats from save data.
- Editing a character's base stats in debug tools.

When `BaseValue` changes, recalculate `CurrentValue` from base plus active modifiers.

## When Not To Modify BaseValue

Do not modify `BaseValue` for temporary or momentary gameplay changes:

- Taking damage.
- Healing.
- Poison or burn damage over time.
- Temporary movement speed buff.
- Temporary slow debuff.
- Shield damage.
- Stun state.

These should modify `CurrentValue`, add/remove modifiers, or add/remove gameplay tags.

## Correct Runtime Flow

Damage should be:

```text
GameplayEffect
-> AbilitySystemComponent
-> AttributeSet
-> Health.CurrentValue changes
```

Buffs should be:

```text
Add modifier
-> Recalculate CurrentValue
-> Remove modifier when expired
-> Recalculate CurrentValue
```

Permanent upgrades should be:

```text
Set BaseValue
-> Recalculate CurrentValue
```

## Rule

Do not use a vague `SetAttribute()` as the main API. Prefer explicit functions:

```cpp
InitializeAttribute()
SetBaseValue()
SetCurrentValue()
ApplyInstantCurrentChange()
AddModifier()
RemoveModifier()
RecalculateCurrentValue()
```

This keeps initialization, permanent stat changes, instant changes, and temporary modifiers from being mixed together.
