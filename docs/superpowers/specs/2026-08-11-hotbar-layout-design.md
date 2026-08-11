# Hotbar Layout Design

## Goal

Recreate the supplied dark-fantasy HUD composition as a centered, bottom-aligned hotbar using the existing shield, wing, orb-frame, and sheen textures.

## Structure

- Keep `Hotbar`, `HealthOrb`, and the future `ManaOrb` as separate `Widget` objects.
- Every widget derives its horizontal placement from the current client width and the same `centerX` anchor.
- Keep the screen camera mapped to client bounds so UI coordinates remain pixel based.

## Layout

- Draw the paired wing texture first in `AABB2(centerX - 600, 0, centerX + 600, 220)`.
- Draw the left and right shield textures above it, spanning a combined width of `1000` and height of `200`.
- Draw square `200 x 200` orb frames last so they cover the shield and wing seams.
- Center the health orb at `centerX - 400` and the mana orb at `centerX + 400`.
- Reserve the middle `600` pixels for later ability-slot rendering.

## Rendering Order

1. Wings
2. Left and right shield halves
3. Health and mana orb contents
4. Orb sheen and frames
5. Future ability slots and key labels

Use alpha blending and bilinear-clamp sampling for these raster UI textures.

## Validation

- The complete composition remains centered when the client width changes.
- Both orbs are symmetric around the screen center.
- Wings appear behind the shields and orbs.
- The shield-and-orb composition is `1000` pixels wide; the background wings extend the full layout to `1200 x 220` pixels.
