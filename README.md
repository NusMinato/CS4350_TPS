
# CS4350 Game Project README — Detailed Module Breakdown (10 Pillars)

> ⚠️ **Note: Communication is key!**  
> Please feel free to share any ideas in the group.  
> Especially when changing parts owned by others, make sure to **communicate in advance** to avoid conflicts or duplicated work.

This document details **ten pillars** of the project so that any contributor can pick up **two pillars** and deliver shippable work. Everything is written for `Unreal Engine 5.6.1`.

- 1) Player Controls & Combat System
- 2) Enemy AI (Minions & Boss)
- 3) UI & HUD
- 4) Level / Scene Design
- 5) Interaction System
- 6) Shaders / Rendering Style
- 7) Audio & Music
- 8) VFX / Particles
- 9) Save & Load System
- 10) Narrative / Story
 
> **Definition of Done (high level):** Runs in packaged build, stable FPS on target PC, zero blocker bugs, clear player goal from spawn to completion, with checkpoints and menu flow.

---

## 1) Player Controls & Combat System
**Goals**: Responsive and flexible gameplay with third‑person camera.

**Core Features**
- **Locomotion**: Walk, run, sprint, crouch, jump, vault/climb/ledge‑mantle (optional).
- **Camera**: Third‑person follow, shoulder switch, camera shake (damage, firing), optional lock‑on.
- **Input**: Enhanced Input mappings (keyboard/mouse or gamepad), remappable bindings.
- **Melee**: Light/Heavy chain, combo windows, hit‑pause, perfect parry/dodge i‑frames, execution/finisher when target is stunned.
- **Ranged**: Hip‑fire vs ADS, recoil/accuracy bloom, ammo & reload (interruptible), weapon sway, projectile vs line‑trace.
- **State & Transitions**: Animation Blueprint (ABP) with state machine and Montages; cancel rules (e.g., dodge cancels heavy), root motion where applicable.
- **Attributes**: Health, Stamina (for sprint/dodge), optional Resource (e.g., Sanity, Mana) for skills; regen & gating.
- **Skills/Abilities**: At least 1 actives + 1 ultimate (cooldowns, costs), gating by story.

**Data & Interfaces**
- Damage interface (ApplyDamage, hit result payload: instigator, causer, surface, multipliers).
- Weapon interface: StartFire/StopFire, Reload, GetAmmo/ClipSize, CanFire, spread & recoil parameters.
- Events to UI: Health/Stamina changed, Ammo changed, Ability ready, Status effects.
- Events to VFX/Audio: Muzzle flash, impacts, footsteps, hurt/heal, dodge, parry.

**Tools**
- CharacterMovementComponent, ABP/Montage, Control Rig, GAS (Game Ability System), DataTables for weapons & movesets.

**Acceptance Criteria**
- 60 FPS with firing + melee spam in a test arena.
- Clear hit feedback (sound, flash, hit‑pause) and readable dodge/parry windows.
- ADS + reload + weapon switch reliable; no animation dead‑locks.
- Mouse plays well (sensitivity/accel tunable).

---

## 2) Enemy AI (Minions & Boss)
**Goals**: Predictable rules with room for mastery; boss with multi‑phase pattern.

**Core Features**
- **Sensing**: AI Perception (sight/hearing), aggro, de‑aggro, investigate last known position.
- **Navigation**: NavMesh pathing, avoidance, off‑mesh links (jumps/climbs) if required.
- **Minions**: 2 archetypes minimum (melee rusher, ranged harasser). Flanking, strafe, retreat on low HP.
- **Combat**: Attack telegraph, cooldowns, friendly‑fire/line‑of‑sight checks, cover seeking (EQS optional).
- **Boss**: ≥2 phases with threshold triggers (HP/time). Unique mechanics (weak points, adds, arenas, AoE tells).
- **Stagger/Stun**: Unified debuff system (from player skills), execution window pipeline.
- **Spawn/Despawn**: Encounter script, waves, leashing, cleanup when out of combat.

**Data & Interfaces**
- Blackboard keys: TargetActor, LastSeenLocation, IsInAttackRange, Phase.
- Behavior Tree tasks: Patrol, Chase, Attack, FindCover, PhaseSwitch.
- Damage intake hooks, stun/CC tags; broadcast Boss HP to UI; spawn hooks for VFX/SFX.

**Tools**
- Behavior Tree & Blackboard, AIController, EQS (for cover/goal selection), DataTables for stats, Animation BP.

**Acceptance Criteria**
- AI does not get stuck on nav; predictable yet challenging.
- Boss phases switch reliably; boss bar syncs; telegraphs readable and counterable.
- 30+ simultaneous minions stress test without logic collapse.

---

## 3) UI & HUD
**Goals**: Inform without clutter; fast status readout; simple menus for testing/ship.

**Core Features**
- **HUD**: Health, Stamina/Resource, Ammo, Crosshair, Ability icons with cooldown overlays & timers.
- **Boss Bar**: Visible in boss arena; fades when boss despawns.
- **Damage & Status**: Hit markers, damage numbers (optional), low‑HP vignette, debuff icons.
- **Menus**: Main Menu (Start, Settings, Quit), Pause (Resume, Settings, Quit to Title), Results screen.
- **Settings**: Audio volumes, mouse sensitivity, display mode & resolution presets.
- **Inventory/Backpack**: Item list, equipment slots, tooltips.

**Data & Interfaces**
- Bindings to Player State (HP/Ammo/Resources), Ability System, Save data for settings.
- Events for boss start/end, pickup acquired, quest/objective updates.

**Tools**
- UMG/Widget Blueprints, CommonUI (optional), Retainer Box for effect passes, material‑driven UI FX.

**Acceptance Criteria**
- All HUD elements update within one frame of state changes.
- Menus navigable with mouse; settings persist via SaveGame.
- No UI element overlaps crucial screen regions in 16:9 and 16:10 at 1080p/1440p.

---

## 4) Level / Scene Design
**Goals**: A complete playable path from Spawn → Combat → Mid objective → Boss → Exit.

**Core Features**
- **Blockout → Art pass**: Start with graybox for metrics (jump gaps, vault heights), convert to art with modular kits.
- **Combat**: Teach/test/expand loop; enemy placements escalate complexity; arena variety.
- **Checkpoints**: Safe spawns, short backtracking, save spheres; boss antechamber checkpoint.
- **Traversal**: Shortcuts/unlocks, optional pickups, readable landmarks, lighting for guidance.
- **Optimization layout**: Culling portals, sub‑levels (streaming), HLOD volumes.

**Data & Interfaces**
- Encounter volumes (spawn waves), objective triggers, checkpoint actors, streaming level toggles.

**Tools**
- World Partition or Level Streaming, NavMesh, Trigger volumes, DataLayers, HLOD, Lumen/Nanite config.

**Acceptance Criteria**
- Zero geometry softlocks; stable FPS in all hotspots.

---

## 5) Interaction System
**Goals**: Unified way to interact with world, items, doors, switches, pickups, crafting (optional).

**Core Features**
- **Detection**: Line trace or overlap sphere from camera; focus & interaction prompts.
- **Interactables**: Doors, levers, terminals; pickups (ammo/health/keys), readable notes (optional).
- **Context Rules**: Distance, facing, key ownership, cooldowns.
- **Inventory Hooks**: Add/Remove items, equip weapons, use consumables; stacking and rarity.
- **Loot**: Enemy death loot tables; auto‑loot toggle; ground clutter despawn timer.

**Data & Interfaces**
- IInteractable interface (CanInteract, OnInteract), prompt text, icon ID. 
- Inventory component events for UI; save data persistence for inventory & world state.

**Tools**
- Blueprint Interface, Actor Components (Interaction, Inventory), DataTables for items/loot tables.

**Acceptance Criteria**
- Prompt appears/disappears correctly; no “ghost” prompts; items cannot be duplicated or lost.

---

## 6) Shaders / Rendering Style
**Goals**: Cohesive stylized look without tanking performance.

**Core Features**
- **Materials**: Master materials + instances (environment, character, weapon, FX). 
- **Style**: Hand‑painted/toon/oil‑brush look via post‑process & surface shaders; edge highlights; color grading.
- **Lighting**: Lumen (or hybrid baked); exposure control; time‑of‑day if needed.
- **Performance**: Instruction count budgets, texture resolution policy, Nanite usage for high‑poly assets.

**Data & Interfaces**
- Global parameters via Material Parameter Collections (e.g., “MadnessLevel” → drives hue shift).
- Post Process volumes per area; data layers toggle look if needed (e.g., dream state).

**Tools**
- Material Editor, Post‑Process materials, MPCs, Color Grading LUTs; Visualize → Shader Complexity.

**Acceptance Criteria**
- Style reads clearly in gameplay camera; shader complexity mostly green/cyan; no full‑screen expensive passes during combat.

---

## 7) Audio & Music
**Goals**: Make every action feel tactile; deliver mood shifts (explore → combat → boss).

**Core Features**
- **SFX**: Footsteps, jumps, landings, weapon fire, reload, melee swings & hits, UI clicks, pickups, doors.
- **Barks/VO** (optional): Enemy shouts, boss phase calls, player grunts.
- **Music**: Exploration loop, combat loop, boss tracks, victory/defeat stingers; adaptive transitions (side‑chain ducking SFX vs BGM).
- **Spatialization**: 3D attenuation, reverb zones; occlusion for walls.
- **Mixing**: Buses (SFX, Music, VO, UI), master limiter, loudness targets; settings screen volume sliders.

**Data & Interfaces**
- Cue sheets, concurrency rules, random containers, RTPCs (intensity, HP, phase).
- Hooks from gameplay events (hit, headshot, execute, boss phase).

**Tools**
- Unreal Audio Mixer, Sound Cue/MetaSounds, Submixes, Reverb/Ambisonics (if needed).

**Acceptance Criteria**
- No clipping; ducking works on explosions; clear telegraph sounds; music transitions within 250 ms.

---

## 8) VFX / Particles
**Goals**: Reinforce readability and impact without obscuring gameplay.

**Core Features**
- **Combat FX**: Muzzle flashes, tracers, bullet impacts by surface, blood/hit sparks, parry flashes, dodge trails.
- **Ability FX**: Charge‑ups, AoEs, debuffs (stun/freeze/slow), ultimate signature effects.
- **Environmental FX**: Fog, embers, dust motes, rain, god‑rays (balanced cost).
- **Boss FX**: Phase transitions, telegraph decals, arena hazards.
- **Performance**: LODs, fixed bounds, GPU particles only where justified; cull distances.

**Data & Interfaces**
- Niagara parameters from gameplay (damage, charge level, sanity); surface type → impact FX mapping.

**Tools**
- Niagara, Decals, Material FX, Ribbon trails; Profilers (GPU Visualizer).

**Acceptance Criteria**
- FX visible but not blinding; < 3 ms GPU cost in heavy scenes on target PC; consistent style with shaders.

---

## 9) Save & Load System
**Goals**: Robust persistence for settings, progress, inventory, checkpoints.

**Core Features**
- **What to Save**: Player stats (HP/resources), inventory & equipment, progression flags, current level & checkpoint, settings (audio/controls/video), boss defeat flags.
- **When to Save**: Auto‑save at checkpoints, manual save in menu, quick‑save (optional), on quit.
- **World State**: Pickups consumed, doors opened, puzzles solved, enemy cleared flags.
- **Versioning**: Save version number; migration on schema change; safe fallback for incompatible saves.

**Data & Interfaces**
- USaveGame subclass with structs for player/world/settings; compression & encryption (lightweight).
- Serialization for custom actors (unique IDs); spawn tables rebuilt on load.

**Tools**
- USaveGame, GameInstance, Level streaming rehydration, Async IO.

**Acceptance Criteria**
- Save/Load survives level transitions & app restarts; no dupes; no softlocks; settings persist across sessions.

---

## 10) Narrative / Story
**Goals**: Deliver a minimal but coherent story that motivates the player and frames gameplay beats.

**Core Features**
- **Premise & Arc**: 1‑page overview (inciting incident → mid‑twist → boss confrontation → resolution).
- **Story Delivery**: Environmental storytelling (props/decals), collectible notes/logs, short in‑engine cut‑ins (Sequencer), radio/VO (optional).
- **Objectives**: Clear goals per beat (UI tracker, on‑screen prompts); failure states & retries.
- **Dialogue** (optional): Simple conversation widgets; skip/auto‑advance.
- **Cinematic Hooks**: Entrance stingers, boss intro/outro, victory/defeat stingers; music sync.

**Data & Interfaces**
- Objective system: begin/complete/fail events; UI hooks; Save flags.
- Narrative triggers (overlaps, switches, boss phase changes).

**Tools**
- Sequencer, Level Blueprint, Objective Manager (component), DataTables for text & localization.

**Acceptance Criteria**
- Player always knows “what to do next”; cinematics skippable; subtitles legible; narrative state saved/restored.

---

## Cross‑Cut Policies
- **Performance**: Target 60 FPS 1080p dev PC; weekly profiling; shader/FX budgets defined per arena.
- **Content Pipeline**: Naming conventions, folder layout, LFS for binary assets, code style, peer review.
- **Testing**: Nightly smoke test, encounter checklist, boss script reliability, save/load torture tests.
- **Accessibility**: Remappable keys, subtitles, volume sliders.
