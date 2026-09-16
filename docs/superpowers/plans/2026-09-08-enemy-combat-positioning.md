# Enemy Combat Positioning Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the unstable per-enemy obstacle lock with stable melee/ranged positions around the player and acceleration-limited movement.

**Architecture:** `AIController` continues to decide whether an enemy should chase or attack, but chases a deterministic combat slot instead of the player center. A small pure `CombatPositioning` module calculates slots and acceleration-limited velocity; orientation and animation consume the final velocity only. This first stage intentionally omits general crowd avoidance and keeps collision correction out of normal movement.

**Tech Stack:** C++17, existing `Vec2`/`Vec3`/`MathUtils`, Visual Studio 2022, existing GAS Debug x64 target.

## Global Constraints

- Preserve all unrelated user changes in the dirty working tree.
- Do not use `CylinderComponent` as collision data; it is debug rendering only.
- Use `CharacterDefinition::m_physicsRadius` for gameplay spacing.
- Do not reintroduce enemy-to-enemy `PushZCylinderOutofEachOther3D` as normal movement.
- Keep slot assignment deterministic for the lifetime of each enemy.
- Do not add an external navigation or crowd dependency in this stage.

---

### Task 1: Add deterministic combat-slot calculation

**Files:**
- Create: `Code/Game/CombatPositioning.hpp`
- Create: `Code/Game/CombatPositioning.cpp`
- Modify: `Code/Game/Game.vcxproj`
- Modify: `Code/Game/Game.vcxproj.filters`
- Test: `Code/Game/CombatPositioningTests.cpp`

**Interfaces:**
- Produces: `Vec2 ComputeCombatSlotPosition(Vec2 const& playerPosition, float preferredRadius, float spacing, unsigned int slotOrdinal)`.
- Produces: `void RunCombatPositioningTests()` in `_DEBUG` builds.

- [ ] **Step 1: Write the failing deterministic tests**

Create `Code/Game/CombatPositioningTests.cpp` with debug assertions covering distinct slots, deterministic output, and automatic outer rings:

```cpp
#include "Game/CombatPositioning.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#if defined(_DEBUG)
void RunCombatPositioningTests()
{
    Vec2 const player( 0.f, 0.f );
    Vec2 const slot0 = ComputeCombatSlotPosition( player, 2.f, 2.f, 0u );
    Vec2 const slot1 = ComputeCombatSlotPosition( player, 2.f, 2.f, 1u );
    Vec2 const slot7 = ComputeCombatSlotPosition( player, 2.f, 2.f, 7u );

    GUARANTEE_OR_DIE( slot0 != slot1, "Combat slots must be distinct" );
    GUARANTEE_OR_DIE( slot0 == ComputeCombatSlotPosition( player, 2.f, 2.f, 0u ), "Combat slots must be deterministic" );
    GUARANTEE_OR_DIE( ( slot7 - player ).GetLength() > ( slot0 - player ).GetLength(), "Overflow slots must use an outer ring" );
}
#endif
```

- [ ] **Step 2: Add the test declaration and invoke it once in Debug**

Add this declaration near the top of `Game.cpp` and call it at the beginning of `Game::Game()`:

```cpp
#if defined(_DEBUG)
void RunCombatPositioningTests();
#endif
```

```cpp
#if defined(_DEBUG)
    RunCombatPositioningTests();
#endif
```

- [ ] **Step 3: Build to verify the test fails because the slot function is missing**

Run:

```powershell
msbuild GAS.sln /p:Configuration=Debug /p:Platform=x64
```

Expected: compilation or link failure naming `ComputeCombatSlotPosition`.

- [ ] **Step 4: Implement the slot calculator**

Create `Code/Game/CombatPositioning.hpp`:

```cpp
#pragma once

#include "Engine/Math/Vec2.hpp"

Vec2 ComputeCombatSlotPosition(
    Vec2 const& playerPosition,
    float       preferredRadius,
    float       spacing,
    unsigned int slotOrdinal );
```

Create `Code/Game/CombatPositioning.cpp`:

```cpp
#include "Game/CombatPositioning.hpp"
#include "Engine/Math/MathUtils.hpp"

#include <cmath>

Vec2 ComputeCombatSlotPosition(
    Vec2 const& playerPosition,
    float       preferredRadius,
    float       spacing,
    unsigned int slotOrdinal )
{
    float const safeRadius = preferredRadius > 0.f ? preferredRadius : 1.f;
    float const safeSpacing = spacing > 0.f ? spacing : 1.f;

    unsigned int remainingOrdinal = slotOrdinal;
    unsigned int ringIndex = 0u;

    for ( ;; )
    {
        float const ringRadius = safeRadius + static_cast<float>( ringIndex ) * safeSpacing;
        unsigned int const capacity = static_cast<unsigned int>(
            std::floor( 2.f * PI * ringRadius / safeSpacing ) );
        unsigned int const safeCapacity = capacity > 0u ? capacity : 1u;

        if ( remainingOrdinal < safeCapacity )
        {
            float const phaseOffset = ( ringIndex & 1u ) != 0u ? 0.5f : 0.f;
            float const angleDegrees =
                360.f * ( static_cast<float>( remainingOrdinal ) + phaseOffset ) /
                static_cast<float>( safeCapacity );

            return playerPosition + Vec2::MakeFromPolarDegrees( angleDegrees, ringRadius );
        }

        remainingOrdinal -= safeCapacity;
        ++ringIndex;
    }
}
```

- [ ] **Step 5: Register all three new files in the Visual Studio project and filters**

Add both `.cpp` files as `ClCompile` items and the header as a `ClInclude` item. Put them in the existing Game source/header filters.

- [ ] **Step 6: Build and run Debug once**

Run the same `msbuild` command. Expected: build succeeds; launching `Run/GAS_Debug_x64.exe` does not trigger a combat-positioning guarantee.

- [ ] **Step 7: Commit only the slot module and its tests**

```powershell
git add Code/Game/CombatPositioning.hpp Code/Game/CombatPositioning.cpp Code/Game/CombatPositioningTests.cpp Code/Game/Game.cpp Code/Game/Game.vcxproj Code/Game/Game.vcxproj.filters
git commit -m "feat: add deterministic enemy combat slots"
```

---

### Task 2: Replace obstacle locking with stable slot targets

**Files:**
- Modify: `Code/Game/AIController.hpp`
- Modify: `Code/Game/AIController.cpp`
- Modify: `Code/Game/Game.cpp`

**Interfaces:**
- Consumes: `ComputeCombatSlotPosition(...)` from Task 1.
- Produces: `unsigned int AIController::m_combatSlotOrdinal` assigned once by `Game::CreateAIController`.
- Produces: `Vec3 AIController::ComputeDesiredVelocity(Character const&) const`.

- [ ] **Step 1: Remove the experimental avoidance state**

Delete `AvoidOtherEnemies`, `m_avoidanceObstacle`, and `m_avoidanceSide` from `AIController.hpp` and `AIController.cpp`.

- [ ] **Step 2: Add a stable slot ordinal to each AI controller**

Change the constructor to:

```cpp
AIController( Game* game, ActorHandle const& actorHandle, unsigned int combatSlotOrdinal );
```

Add:

```cpp
unsigned int m_combatSlotOrdinal = 0u;
```

Initialize it in the constructor initializer list.

- [ ] **Step 3: Assign the ordinal once when creating controllers**

In `Game::CreateAIController`, use the current AI-controller count before insertion:

```cpp
unsigned int const slotOrdinal = static_cast<unsigned int>( m_aiControllers.size() );
AIController* aiController = new AIController( this, character.m_handle, slotOrdinal );
```

- [ ] **Step 4: Compute melee and ranged slot radii**

In `AIController.cpp`, include `CombatPositioning.hpp`. Replace direct player-center pursuit with:

```cpp
Vec2 const playerPosition(
    m_game->m_playerCharacer->m_position.x,
    m_game->m_playerCharacer->m_position.y );

float const spacing = character.m_characterDef.m_physicsRadius * 2.f + 0.25f;
float const preferredRadius = character.m_characterDef.m_attackRange;
Vec2 const slotPosition = ComputeCombatSlotPosition(
    playerPosition,
    preferredRadius,
    spacing,
    m_combatSlotOrdinal );

Vec2 const characterPosition( character.m_position.x, character.m_position.y );
Vec2 toSlot = slotPosition - characterPosition;
float const distanceToSlot = toSlot.GetLength();

if ( distanceToSlot > 0.05f )
{
    toSlot.Normalize();
    movementComponent->m_velocity = Vec3(
        toSlot.x * character.m_characterDef.m_runSpeed,
        toSlot.y * character.m_characterDef.m_runSpeed,
        0.f );
}
else
{
    movementComponent->m_velocity = Vec3::ZERO;
}
```

Keep attack eligibility based on distance to the player, not distance to the slot.

- [ ] **Step 5: Build and manually verify stable ownership**

Expected behavior: every enemy returns to the same relative position after the player moves; enemies do not swap slots frame-to-frame; ranged enemies remain on a larger ring because their `attackRange` is larger.

- [ ] **Step 6: Commit the integration**

```powershell
git add Code/Game/AIController.hpp Code/Game/AIController.cpp Code/Game/Game.cpp
git commit -m "feat: make enemies chase stable combat slots"
```

---

### Task 3: Add acceleration-limited actual velocity

**Files:**
- Modify: `Code/Game/AIController.hpp`
- Modify: `Code/Game/AIController.cpp`
- Modify: `Run/Data/Definitions/CharacterDefinitions.xml`
- Modify: `Code/Game/CharacterDefinition.hpp`
- Modify: `Code/Game/CharacterDefinition.cpp`

**Interfaces:**
- Produces: `CharacterDefinition::m_maxAcceleration` loaded from `maxAcceleration`.
- Produces: `Vec3 MoveVelocityToward(Vec3 const& current, Vec3 const& desired, float maxDelta)` as a private AI helper.

- [ ] **Step 1: Add and load `maxAcceleration`**

Add `float m_maxAcceleration = 8.f;` beside `m_runSpeed`, parse it from the `<Physics>` element, and set `maxAcceleration="8.0"` for every enemy definition.

- [ ] **Step 2: Implement the velocity limiter**

```cpp
Vec3 AIController::MoveVelocityToward(
    Vec3 const& current,
    Vec3 const& desired,
    float       maxDelta ) const
{
    Vec3 delta = desired - current;
    float const deltaLength = delta.GetLength();

    if ( deltaLength <= maxDelta || deltaLength <= 0.f )
    {
        return desired;
    }

    return current + delta * ( maxDelta / deltaLength );
}
```

- [ ] **Step 3: Apply the limiter after calculating the slot velocity**

Save the current movement velocity before calculating the desired velocity, then apply:

```cpp
float const maxVelocityDelta =
    character.m_characterDef.m_maxAcceleration *
    static_cast<float>( Clock::GetSystemClock().GetDeltaSeconds() );

movementComponent->m_velocity = MoveVelocityToward(
    currentVelocity,
    desiredVelocity,
    maxVelocityDelta );
```

- [ ] **Step 4: Make orientation and animation consume only final velocity**

Keep one `SetFloat("Speed", ...)` call after velocity limiting. In `TurnTowardDirection`, only face velocity when squared speed is greater than `0.04f`; otherwise face the player without changing position.

- [ ] **Step 5: Build and verify direction continuity**

Expected: no one-frame 90-degree velocity changes; movement ramps up and down; bodies turn steadily toward the actual motion direction.

- [ ] **Step 6: Commit acceleration smoothing**

```powershell
git add Code/Game/AIController.hpp Code/Game/AIController.cpp Code/Game/CharacterDefinition.hpp Code/Game/CharacterDefinition.cpp Run/Data/Definitions/CharacterDefinitions.xml
git commit -m "feat: smooth enemy velocity with acceleration limits"
```

---

### Task 4: Add debug evidence and conduct the gameplay check

**Files:**
- Modify: `Code/Game/AIController.cpp`

**Interfaces:**
- Consumes: stable slot position and final movement velocity from Tasks 2 and 3.
- Produces: debug markers for assigned slot and final velocity.

- [ ] **Step 1: Draw the assigned slot in `RenderDebug`**

When AI debug rendering is enabled, draw a small disc at the current world-space slot position and a line from the enemy to the slot. Use the existing default shader and debug vertex helpers already used in `RenderDebug`.

- [ ] **Step 2: Run the acceptance scenario**

Launch `Run/GAS_Debug_x64.exe` and verify all of the following:

- Four enemies approach four distinct positions.
- Moving the player causes the positions to follow without enemies swapping ownership.
- Skeleton and HornedDemon use the inner ring.
- SkeletonArcher and GoatDemon remain on their ranged ring.
- No enemy circles away from the player because of a stale obstacle pointer.
- Enemy bodies do not oscillate left/right while approaching a slot.
- Idle/Walk transitions follow the final smoothed velocity.

- [ ] **Step 3: Run the Debug x64 build once more**

```powershell
msbuild GAS.sln /p:Configuration=Debug /p:Platform=x64
```

Expected: successful build with no new warnings from the touched files.

- [ ] **Step 4: Commit debug visualization**

```powershell
git add Code/Game/AIController.cpp
git commit -m "debug: visualize enemy combat slots"
```

---

## Deferred Follow-up

A separate plan should add a centralized crowd solver only if enemies still collide while travelling to distinct slots. That follow-up must snapshot all positions and desired velocities, calculate relative avoidance for every enemy from the same snapshot, and apply all final velocities afterward. Do not reintroduce per-controller persistent obstacle pointers.
