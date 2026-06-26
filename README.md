# cs16-opengl-research

A study of a classic **OpenGL wrapper hack** for Counter-Strike 1.6 (GoldSrc engine),
based on the discontinued *panzerGL 2.2* multi-mod. The hack ships as a fake
`opengl32.dll` that the game loads instead of the real one, then intercepts OpenGL
draw calls to implement wallhack, ESP, aimbot, radar, and a set of visual tweaks
— all heavily expanded and modernized from the original.

<div align="center">
  <img src="./demo.jpg" />
</div>

<video src="https://github.com/user-attachments/assets/13b489dc-080a-48f3-9f50-91764662208b" controls loop muted></video>

> [!NOTE]
> **This repository is for technical research and educational purposes only.**
> The goal is to understand how render-layer game hacks work (DLL proxying,
> OpenGL interception, depth-buffer manipulation, engine entity-list reading).
> Do **not** use this on online/official servers. Using it against other players
> violates game terms of service and is not VAC-safe. Test only against bots or
> on a private/non-Steam server you control. Use at your own risk.

---

## Table of Contents

- [How to use](#how-to-use)
  - [Installation](#installation)
  - [In-game controls](#in-game-controls)
  - [Hack menu options](#hack-menu-options)
- [Screenshots](#screenshots)
- [FAQ](#faq)
- [Going deeper](#going-deeper)
- [Credits](#credits)

---

## How to use

> [!IMPORTANT]
> Your **Counter-Strike 1.6 client must be build 4554 or below**. Newer engine
> builds broke the proxy-DLL technique (and added VAC protections).

### Installation

1. Open the [`things-you-need-to-get-hack-works`](./things-you-need-to-get-hack-works) folder.
2. Copy **both files** into your Counter-Strike 1.6 main directory
   (the folder that contains `hl.exe` / `cstrike`):
   - `opengl32.dll` — the prebuilt hack
   - `oglconf.cfg` — default settings (cvars)
3. Make sure the game is running in **OpenGL** video mode.
4. Launch the game, then press `F12` to enable the hack.

> [!TIP]
> On most laptops (and Mac keyboards running Windows) F-keys require the `Fn`
> modifier, e.g. `Fn + F12`. The `Insert` key may be mapped to `Fn + Enter`.

> [!NOTE]
> If `F11` shows **"Could not load config file"** in red, `oglconf.cfg` is missing
> from the game directory. Make sure it's next to `opengl32.dll` and that the
> extension is not hidden (e.g. not `oglconf.cfg.txt`).

---

### In-game controls

| Key | Action |
|-----|--------|
| `F12` | Master switch — turn the hack **on / off** |
| `Insert` | Open / close the **hack menu** |
| `↑` / `↓` | Navigate menu rows (hold for auto-repeat) |
| `←` / `→` | Change the selected option's value or toggle it |
| `↑` / `↓` / `←` / `→` | Move the active panel (when in move mode) |
| `F11` | Toggle the **debug screen** (config paths, resolution, offsets) |
| `F10` | **Reset config to defaults** — reloads `oglconf.cfg` and deletes `oglsave.cfg` |

> Every change made in the menu is automatically saved to `oglsave.cfg` and
> restored the next time the hack loads. `F10` wipes that file and brings
> everything back to the shipped defaults.

---

### Hack menu options

Open the menu with `Insert`. Use `↑`/`↓` to scroll, `←`/`→` to change a value.
Sub-options (indented with `- `) are hidden until their parent feature is turned on.

#### Aimbot

| Option | Description |
|--------|-------------|
| **Aimbot** | Enable auto-aim. Snaps the mouse toward the nearest enemy head within FOV. |
| **- Aim smooth** | Smoothing strength (0 = instant snap, 1–10 = progressively slower follow). |
| **- Target** | Which team to aim at (Terrorists / Counter-Terrorists). |
| **- Shoot** | Auto-fire once aimed at a target. |
| **- Aimthru** | Aim through walls. Off = depth-buffer visibility check required. |
| **- FOV** | Screen-pixel radius around the crosshair to search for targets. |
| **- Head dot** | Draw a dot at the exact point the aimbot aims at (each target-team enemy). |
| **- Aim point** | Vertical aim offset from head center (world units; 0 = center of head, +up / −down). |
| **Triggerbot** | Auto-fire when the crosshair rests on an enemy. |
| **- Trigger delay** | Milliseconds to wait before firing (humanization). |
| **Auto-fire** | Spam clicks while holding Mouse1 (auto-pistol / auto-knife). |
| **- Auto-fire rate** | Milliseconds between injected clicks (lower = faster). |

#### Combat / rendering tweaks

| Option | Description |
|--------|-------------|
| **Recoil** | Mouse-down compensation per shot (0 = off, 1–5 = strength). |
| **No recoil** | Zeroes the engine's view-punch via a `V_CalcRefdef` detour — no screen kick. |
| **Wallhack** | 0 = off · 1 = basic (depth test off) · 2 = additive glow · 3 = saturate blend. |
| **No Sky** | Skip rendering the skybox. |
| **No Flash** | Reduce flashbang white to near-zero alpha. |
| **No Smoke** | Skip smoke-grenade geometry. |
| **Lambert** | Force white on all player vertices (fullbright — stay visible in dark areas). |
| **Crosshair** | Draw a custom static crosshair at the screen center. |

#### ESP Engine

The ESP reads the engine's own entity list for real player names, origins, and team data.

| Option | Description |
|--------|-------------|
| **ESP Engine** | Master toggle for the engine-based ESP. |
| **- Player name** | Draw the player's name above the box. |
| **- - Name size** | Font size: 1 = small (7 px) · 2 = normal (10 px) · 3 = large (13 px) · 4 = x-large (16 px). |
| **- - Name padding** | Vertical offset of the name above the box (negative = closer). |
| **- Box** | Draw a 2D bounding box around each player. |
| **- - Box padding** | Grow / shrink the box by this many pixels. |
| **- - Box radius** | Corner rounding (0 = sharp corners). |
| **- - Box width** | Stroke thickness in pixels. |
| **- Distance** | Draw the distance in metres below the box. |
| **- - Dist size** | Font size (same scale as Name size above). |
| **- - Dist padding** | Vertical offset below the box (negative = closer). |
| **- Snaplines** | Line from a screen anchor to each enemy: 0 = off · 1 = bottom · 2 = top · 3 = crosshair. |
| **- Vis check** | Dim ESP when the enemy is occluded (depth-buffer test). |
| **- Off-screen arrow** | Edge-of-screen arrow pointing at enemies outside the viewport. |
| **- Max distance** | Hide ESP beyond this distance in metres (0 = unlimited). |
| **- Distance fade** | Fade ESP alpha with distance (closer = fully opaque). |
| **- Show team** | 0 = both teams · 1 = CT only · 2 = T only. |
| **- Debug text** | Top-left readout showing player count and team-detection method. |

#### HUD

| Option | Description |
|--------|-------------|
| **HUD HP/Ammo** | Master toggle for the own-player HUD arcs around the crosshair. |
| **- HP** | Green arc (left) showing current health in 10% ticks. |
| **- Ammo** | Yellow arc (right) showing current clip in 10% ticks. |
| **- Show when die** | Keep the arcs visible while dead / spectating. |
| **- Padding** | Move the arcs closer to or further from the crosshair. |

#### Chams

| Option | Description |
|--------|-------------|
| **Chams** | Solid-color player models (magenta) visible through walls. |
| **- Chams Wire** | Wireframe mode instead of solid fill. |

#### Radar

| Option | Description |
|--------|-------------|
| **Radar** | 2D mini-map built from the engine entity list. |
| **- Move radar** | Enter move mode — arrow keys reposition the radar disc. |
| **- Dot shape** | 0 = circle · 1 = square. |
| **- Size** | Disc radius in pre-scale units (30–150). |
| **- Zoom (units)** | World units that map to the disc edge (smaller = zoomed in). |
| **- Rotate view** | Rotate the radar so local forward points up. |
| **- Names** | Show a short player name next to each dot. |
| **- Range rings** | Draw two concentric range rings at 1/3 and 2/3 of the zoom radius. |

#### Misc

| Option | Description |
|--------|-------------|
| **Notifications** | Toast pop-ups when toggling features in the menu. |
| **Detect log** | On-screen per-frame enemy / PVS detection counters (top-left). |
| **Move hack menu** | Reposition the hack menu with arrow keys (`Insert` = done). |
| **Move F11 panel** | Reposition the F11 debug panel with arrow keys. |
| **Reset positions** | Snap the menu, F11 panel, and radar back to their default positions. |

---

## Screenshots

| Original panzerGL 2.2 | This version |
|:---------------------:|:------------:|
| <img width="600" src="./demo_old.png" /> | <img width="800" src="./demo.jpg" /> |

---

## FAQ

### Why do I only see enemies when they get close on some servers?

Because the wallhack can only reveal what the server actually **sends** to your
client. The engine uses a **PVS (Potentially Visible Set)** table baked into each
map: from your position, only players in potentially visible areas are transmitted.
Enemies in non-visible areas are never sent — so the hack has nothing to draw.

Servers running a recompiled `de_dust2` **without VIS data** transmit all players
all the time, so you see everyone from anywhere. Properly compiled maps cull
aggressively. This is a hard limit of the network layer, not a weakness of the hack.

### Why does it only work on build 4554?

The engine struct offsets and function-table scanning are tuned for engine build
4554 (the most common non-Steam build). Later builds changed those offsets and
added protections. Earlier builds may also work but are untested.

---

## Going deeper

- **[ARCHITECTURE.md](./ARCHITECTURE.md)** — detailed internals: how each feature
  is implemented at the OpenGL / engine level.
- **[BUILDING.md](./BUILDING.md)** — how to compile the DLL yourself, including
  a step-by-step guide for creating the Visual Studio project from scratch.

---

## Credits

- Original *panzerGL 2.2* multi-mod by **james34602**:
  <https://github.com/james34602/panzerGL22>
- Original aimbot & model recognition: *Kenbabutz* (oC Hack source).
- Blank OpenGL wrapper: *Crusader* (Game-Deception).

This repository is a research fork with a modern Visual Studio project, extensive
new features, and documentation on how the hack is built and how it works.

