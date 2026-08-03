---
title: GAS Gameplay Ability Architecture
tags:
  - gas
  - architecture
  - gameplay-ability
  - thesis
---

# GAS Gameplay Ability Architecture

## Core Idea

The ability system should be **code-backed data-driven**, not pure XML and not one C++ class per concrete skill.

The correct split is:

```text
Engine GAS
  Provides the framework and lifecycle.

Game Ability Types
  Provide reusable gameplay behavior patterns.

XML Definitions
  Provide concrete skill data.
```

This keeps the engine reusable while still allowing real combat behavior such as combo attacks, projectiles, AOE, dashes, channeling, and animation-timed hit frames.

## Engine Responsibility

Engine code owns the generic GAS framework:

```text
AbilitySystemComponent
GameplayAbility base class
GameplayEffect
GameplayEffectDefinition
AttributeSet
GameplayModifier
ActiveGameplayEffect
GameplayEvent base data
AbilityTask base lifecycle
```

Engine GAS controls:

```text
granting abilities
removing abilities
TryActivateAbility
CanActivateAbility entry point
ActivateAbility dispatch
cost checks
cooldown checks
tag checks
effect application
attribute modification
active effect lifetime
source/target ASC context
```

Engine GAS should not know concrete game behavior:

```text
how BasicAttack finds a target
which animation DarkLord plays
how Fireball projectile moves
how a combo input window works
how an AOE target point is selected
```

Those are Game-layer ability behaviors.

## Game Ability Type Responsibility

Game code should define reusable ability behavior types:

```text
ApplyEffectToSelfAbility
ApplyEffectToTargetAbility
MeleeComboAbility
AreaEffectAbility
ProjectileAbility
DashAbility
SpawnActorAbility
SummonAbility
ChannelAbility
```

These are not one class per skill. They are one class per reusable behavior pattern.

Example:

```text
MeleeComboAbility
  Can power SwordCombo, AxeCombo, DarkLordCombo, SkeletonCombo.

AreaEffectAbility
  Can power FireNova, IceNova, HealingCircle, DarkExplosion.

ProjectileAbility
  Can power Fireball, IceBolt, PoisonShot.
```

The Game ability type is allowed to know Game-layer systems:

```text
Character
Animation
TargetingComponent
Projectile Actor
Team / Ally / Enemy rules
Weapon sockets
VFX / SFX
```

It must still use Engine GAS for attribute changes:

```cpp
m_ownerASC->ApplyGameplayEffectToTarget( effect, targetASC );
```

It should not directly change health or other attributes.

## XML Definition Responsibility

XML defines concrete skills by selecting a behavior type and filling parameters.

Example instant self effect:

```xml
<GameplayAbilityDefinition
    name="Recover"
    type="ApplyEffectToSelf"
    effect="Heal" />
```

Example target effect:

```xml
<GameplayAbilityDefinition
    name="BasicDamage"
    type="ApplyEffectToTarget"
    effect="PhysicalDamage" />
```

Example combo:

```xml
<GameplayAbilityDefinition name="SwordCombo" type="MeleeCombo">
    <ComboStep
        animation="Attack_01"
        effect="PhysicalDamage_Light"
        hitFrame="12"
        inputWindowStart="18"
        inputWindowEnd="28"
        nextStep="1" />

    <ComboStep
        animation="Attack_02"
        effect="PhysicalDamage_Light"
        hitFrame="10"
        inputWindowStart="16"
        inputWindowEnd="26"
        nextStep="2" />

    <ComboStep
        animation="Attack_03"
        effect="PhysicalDamage_Heavy"
        hitFrame="15"
        nextStep="-1" />
</GameplayAbilityDefinition>
```

Example AOE:

```xml
<GameplayAbilityDefinition
    name="FireNova"
    type="AreaEffect"
    effect="FireDamage"
    targetPolicy="AroundSelf"
    targetFilter="Enemy"
    radius="4.0"
    delay="0.5" />
```

This is data-driven because new concrete skills can be created by XML as long as they fit an existing behavior type.

## Why Not Pure XML

Pure XML cannot naturally express real gameplay flow:

```text
play animation
wait until hit frame
perform trace
spawn projectile
wait until projectile hit
open combo input window
cancel ability
channel over time
select target point
apply effect to every enemy in radius
```

These are processes, not simple attribute modifiers.

`GameplayEffect` describes attribute result:

```text
Health Add -20
MoveSpeed Multiply 0.5
Mana Add 20
```

`GameplayAbility` describes process:

```text
when the effect happens
who receives it
how the target is found
whether animation/event/projectile/delay is involved
when the ability ends
```

Therefore:

```text
GameplayEffect = result
GameplayAbility = process
```

## Why Not One Class Per Skill

One class per skill does not scale:

```text
FireNovaAbility
IceNovaAbility
PoisonNovaAbility
HealNovaAbility
```

These should be one reusable behavior type:

```text
AreaEffectAbility
```

with different XML definitions.

The rule is:

```text
one class per behavior pattern
one XML definition per concrete skill
```

## Unreal Mapping

Unreal GAS follows the same architectural idea:

```text
UAbilitySystemComponent
  Generic system component.

UGameplayAbility
  Base class for ability lifecycle and custom ability logic.

UGameplayEffect
  Data asset describing attribute/tag modifications.

Game-specific GameplayAbility Blueprints/C++ classes
  Implement combat behavior such as melee, projectile, dash, or channel.
```

Unreal does not put every concrete ability behavior inside the engine core. The system provides the framework; the game implements abilities on top of it.

## Current Project Direction

The current project should use this split:

```text
Engine:
  GameplayAbility base
  AbilitySystemComponent
  GameplayEffect
  AttributeSet

Game:
  reusable Ability Types
  target selection
  animation timing
  projectile spawning
  combo rules

XML:
  concrete abilities
  effects
  attributes
  default ASC loadout
```

For this thesis, this is still a valid Gameplay Ability System. The thesis claim should not be "everything is pure XML." A stronger and more accurate claim is:

```text
The system separates reusable gameplay behavior from concrete skill data through a code-backed data-driven Gameplay Ability System.
```

This supports a defensible architecture:

```text
Engine framework
reusable Game behavior types
XML-authored concrete content
```

## First Full Ability Line

The first complete gameplay line should be:

```text
Input
-> ASC::TryActivateAbility(BasicAttack)
-> GameplayAbility::CanActivateAbility
-> MeleeAttackAbility::ActivateAbility
-> play attack animation
-> reach hit frame
-> perform melee collision / trace
-> find hit Actor
-> get target ASC from hit Actor
-> apply PhysicalDamage GameplayEffect
-> target Health changes
-> animation ends
-> EndAbility
```

This line proves the core system works end to end:

```text
Ability activation
animation-timed gameplay
collision-based targeting
GameplayEffect application
Attribute mutation
ability ending
```

Do not start with combo, AOE, projectile, cooldown, cost, or tags. The first full line should be a single melee attack that uses animation timing and collision to apply one instant damage effect.

## First Game Ability Type

The first Game-layer ability type should be:

```text
MeleeAttackAbility : GameplayAbility
```

It is Game code, not Engine core code.

It owns this behavior:

```text
play attack animation
track active attack time
wait for hit frame
run melee collision / trace
resolve hit actor
resolve target ASC
apply configured GameplayEffect
end the ability
```

It should still use Engine GAS to change attributes:

```cpp
m_ownerASC->ApplyGameplayEffectToTarget( effect, targetASC );
```

It should not directly edit `Health`.

## MeleeAttack XML Shape

The concrete `BasicAttack` should be data on top of `MeleeAttackAbility`:

```xml
<GameplayAbilityDefinition
    name="BasicAttack"
    type="MeleeAttack"
    gameplayEffect="PhysicalDamage"
    animation="Attack"
    hitTime="0.35"
    range="1.5"
    radius="0.5" />
```

The damage effect stays separate:

```xml
<GameplayEffectDefinition name="PhysicalDamage" durationPolicy="Instant">
    <Modifier attribute="Health" operation="Add" value="-20.0" />
</GameplayEffectDefinition>
```

This means:

```text
BasicAttack
  process = MeleeAttackAbility
  result = PhysicalDamage
```

## MeleeAttack Runtime State

`MeleeAttackAbility` needs runtime state because the effect is not applied immediately on activation:

```cpp
float m_elapsedSeconds = 0.f;
bool  m_hasAppliedHit = false;
bool  m_isActive = false;
```

The ability flow is:

```text
ActivateAbility
  set active state
  reset elapsed time
  reset hit-applied flag
  play animation

Update
  advance elapsed time
  if elapsed >= hitTime and hit not applied:
      ExecuteHit
  if animation/ability duration finished:
      EndAbility

ExecuteHit
  trace / overlap in front of owner
  find hit actor
  get target ASC
  create GameplayEffect from definition
  apply effect to target
```

This is why `BasicAttack` cannot be only "activate -> apply effect immediately". The effect must happen at the animation hit frame after collision confirms a target.

## Ability Types To Add Later

After the melee attack line works, add more Game-layer ability types only when needed:

```text
ApplyEffectToSelfAbility
  Recover / Heal / SelfBuff

MeleeComboAbility
  SwordCombo / AxeCombo / BossCombo

ProjectileAbility
  Fireball / Arrow / IceBolt

AreaEffectAbility
  FireNova / HealingCircle / DarkExplosion

DashAbility
  Dodge / Roll / Charge

ChannelAbility
  Beam / Hold-to-cast / Healing channel
```

The rule remains:

```text
one class per reusable behavior type
one XML definition per concrete skill
```
