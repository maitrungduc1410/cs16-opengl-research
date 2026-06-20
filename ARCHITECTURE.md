# Architecture — cs16-opengl-research

How the hack is structured and what happens at runtime. For usage instructions
see [README.md](./README.md). For build instructions see [BUILDING.md](./BUILDING.md).

---

## Table of Contents

- [Proxy DLL concept](#proxy-dll-concept)
- [High-level architecture](#high-level-architecture)
- [Per-frame pipeline](#per-frame-pipeline)
- [Feature internals](#feature-internals)
  - [Wallhack](#wallhack)
  - [Engine entity-list ESP](#engine-entity-list-esp)
  - [Radar](#radar)
  - [Own HUD (HP / Ammo arcs)](#own-hud-hp--ammo-arcs)
  - [Chams](#chams)
  - [Aimbot](#aimbot)
  - [Triggerbot](#triggerbot)
  - [Auto-fire](#auto-fire)
  - [No-recoil detour](#no-recoil-detour)
  - [No Flash / No Smoke / No Sky](#no-flash--no-smoke--no-sky)
  - [Hack menu and config persistence](#hack-menu-and-config-persistence)

---

## Proxy DLL concept

The game renders through `opengl32.dll`. Windows looks for a DLL in the
**application folder first**, before the system folder. Dropping a fake
`opengl32.dll` into the CS directory causes the game to load it instead of the
real one. The fake DLL re-exports every OpenGL function (via `opengl32.def`),
forwards them to the real DLL, and inserts cheat logic in between — invisible
to the game and engine.

---

## High-level architecture

```mermaid
flowchart LR
    A[CS 1.6 engine] -->|glBegin, glVertex3f,\nwglSwapBuffers ...| B[Fake opengl32.dll\nin CS folder]
    B --> C{sys_* wrapper}
    C -->|inject cheat logic| D[ESP / Aimbot / Radar\nHUD / Wallhack / ...]
    C -->|forward call| E[Real opengl32.dll\nin System32]
    D --> E
    E --> F[GPU draws the frame]
```

Every OpenGL call made by the game (`glBegin`, `glVertex3f`, `glViewport`,
`wglSwapBuffers`, …) passes through a matching `sys_*` wrapper in `opengl32.cpp`.
The wrapper runs the cheat logic, then calls the real function via a saved pointer.

---

## Per-frame pipeline

```mermaid
flowchart TD
    VPcount[sys_glViewport call count ≥ 5]
    VPcount -->|enabledraw = true| AIM[Engine aimbot:\nconsume eng_aim_have target\n→ mouse_event nudge]
    AIM --> TRIG[Triggerbot:\ncheck eng_trig_active + delay\n→ click inject]

    SWAP[sys_wglSwapBuffers\none call per frame]
    SWAP --> AF[UpdateAutofire]
    SWAP --> NR[EnsureNoRecoilHook]
    SWAP --> ESP[DrawEngineEsp\nradar + ESP + HUD\naimbot target pick]
    SWAP --> TOAST[DrawToast]
    SWAP --> UI[DrawOverlayUI\nhack menu + F11 panel]

    SHADEMODEL[sys_glShadeModel GL_SMOOTH\n= start of a player model]
    SHADEMODEL -->|player.get = true| CHAMS[Apply chams\ncolor / wireframe]
    POPMATRIX[sys_glPopMatrix\n= end of player model]
    POPMATRIX --> RESTORE[Restore polygon mode\n+ texture state]
```

Key timing facts:
- **`wglSwapBuffers`** is called exactly once per rendered frame — all overlay
  drawing (ESP, radar, menu, toast) happens here.
- **`glViewport`** is called many times per frame; the 5th call enables drawing
  and also runs the aimbot mouse nudge (one nudge per frame).
- **`glShadeModel(GL_SMOOTH)`** marks the beginning of a player model; the
  flag is cleared in `glPopMatrix`.

---

## Feature internals

### Wallhack

Implemented in `sys_glBegin` / `sys_glPopMatrix`. When `cvar.wall` is non-zero
and a player model is drawing (`bWall = true` set by `glPushMatrix`):

| Mode | Effect |
|------|--------|
| `wall 1` | `glDisable(GL_DEPTH_TEST)` on `GL_TRIANGLE_FAN/STRIP` — model paints over walls |
| `wall 2` | Depth off + `GL_SRC_ALPHA, GL_ONE` blend — additive glow |
| `wall 3` | Depth off + `GL_SRC_ALPHA_SATURATE` blend — saturate blend |

The depth test is restored in `sys_glPopMatrix` after each model.

---

### Engine entity-list ESP

Instead of guessing players from vertex counts, the ESP reads the GoldSrc
engine's own entity list at runtime.

**Finding the engine table (`cl_enginefunc_t`):** The engine hands a function
table to `client.dll`. We scan `client.dll`'s readable data pages for a run of
≥ 8 consecutive pointers that all land inside `hw.dll`, then verify slots 51
(`GetLocalPlayer`) and 53 (`GetEntityByIndex`). This gives a stable pointer to
the table without any signature scanning.

**Per-player data extracted each frame:**
- World origin (`ENT_ORIGIN` / `entity_state_t::origin`) for 3D → 2D projection
- Hull type (`usehull`) to compute standing vs. ducking height for box sizing
- Team via `g_PlayerExtraInfo` (scanned once via byte-pattern) or model name fallback
- Alive / stale via `current_position` update counter + 400 ms timeout

**WorldToScreen:** via `pTriAPI` slot 12 (`WorldToScreen`), the engine's own
projection — no manual matrix math needed.

**Visibility check (`esp_vischeck`):** `gluProject` maps the enemy chest to
screen space, then `glReadPixels(GL_DEPTH_COMPONENT)` reads the depth buffer at
that pixel. If the buffer value is ≥ the projected depth (within a small epsilon),
the target is unoccluded.

**Off-screen arrows (`esp_arrow`):** When both head and feet project outside the
viewport, the yaw from `pfnGetViewAngles` is used to compute a screen-edge
direction vector. A filled triangle is drawn at the rim pointing toward the enemy.

---

### Radar

Drawn in `DrawEngineEsp` using the same entity loop as the ESP. Each player's
XY offset from the local player is rotated into screen space (optionally following
view yaw with `radar_rotate`), scaled by `radar_zoom`, and clamped to the disc
radius. The radar center is freely positionable via `Move radar` in the menu
(move mode 3).

---

### Own HUD (HP / Ammo arcs)

GoldSrc sends HP, armor, and weapon clip to the client via **user messages**
(`Health`, `Battery`, `CurWeapon`, `DeathMsg`, `ResetHUD`). The engine keeps a
linked list of `usermsg_t` nodes (each holding a name string and a handler
function pointer). We scan private heap pages for nodes matching those names,
save the original handler, and overwrite the pointer with our own. Our handler
reads the value, updates our locals, then calls the original — so the vanilla HUD
keeps working.

Death tracking uses `DeathMsg` (victim index == our index → `me_dead = true`) and
`ResetHUD` (respawn → `me_dead = false`), independent of the HP value so
spectating a live teammate doesn't reset the dead state.

The HP and ammo arcs are drawn as 10-tick `GL_LINES` segments arranged in two
symmetric 96° arcs flanking the crosshair.

---

### Chams

`sys_glShadeModel(GL_SMOOTH)` fires at the start of every player model. When
`cvar.chams` is on:
- **Solid chams:** `glColor3f(1, 0.15, 0.95)` (magenta) overrides the texture
  color in every `sys_glVertex3f` call during the model.
- **Wireframe chams (`chams_wire`):** `glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)`
  draws only edges. Restored to `GL_FILL` in `sys_glPopMatrix`.

---

### Aimbot

Target selection happens inside `DrawEngineEsp` each frame — the best candidate
(closest screen-space distance to crosshair within `cvar.fov` px, alive, correct
team) is stored in `eng_aim_sx / eng_aim_sy`. On the next frame, `sys_glViewport`
(5th call) reads this target and emits a `mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE)`
nudge, with optional smoothing (`aim_smooth`). `cvar.aimthru` controls whether
a depth-buffer visibility check is required before locking.

---

### Triggerbot

`DrawEngineEsp` tests each frame whether the screen-space crosshair (viewport
center) falls inside any enemy's projected 2D bounding box. If yes,
`eng_trig_active = true` and `eng_trig_acq` records the timestamp. In
`sys_glViewport`, once `(now - eng_trig_acq) ≥ trigger_delay` ms and the
120 ms refractory period has passed, a `LEFTDOWN + LEFTUP` pair is injected.

---

### Auto-fire

A low-level mouse hook (`WH_MOUSE_LL`) on a dedicated thread tracks the physical
left-button state (ignoring injected clicks via `LLMHF_INJECTED`). When the
button is physically held and `cvar.autofire` is on, the hack alternates one
frame releasing (UP) and the next frame pressing (DOWN) at `autofire_rate` ms
intervals. GoldSrc fires one shot per 0→1 edge of `IN_ATTACK`, so this alternate
approach produces clean, reliable shots at the configured rate.

---

### No-recoil detour

`client.dll` exports `V_CalcRefdef(ref_params_s*)`, which adds
`pparams->punchangle` (the view kick) to the camera angles. We locate the export
via `GetProcAddress`, save the first 5 bytes, and overwrite them with a relative
`jmp` to `Hooked_VCalc`. Our hook zeroes `punchangle` **before** the original
runs, eliminating the visual kick. The detour uses unhook/call/re-hook (safe
because the render thread is single-threaded) to avoid needing a disassembler
trampoline.

---

### No Flash / No Smoke / No Sky

- **No Flash:** `sys_glBegin(GL_QUADS)` samples the current color. If it is
  pure white (1,1,1), `bFlash = true`. In `sys_glVertex2f`, when `bFlash` is
  set and the vertex `y` equals the viewport height (full-screen quad), the color
  alpha is set to 0.01 — making the flash nearly invisible.
- **No Smoke:** color components are equal, non-zero, non-one during smoke quads.
  `bSmoke = true` causes `sys_glVertex3fv` to silently return, skipping the
  geometry.
- **No Sky:** `bSky = true` during `GL_QUADS` primitives. When the engine later
  calls `glClear(GL_DEPTH_BUFFER_BIT)` while sky is active, we replace it with
  `GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT` — clearing sky color and preventing
  it from painting over the world.

---

### Hack menu and config persistence

The menu is data-driven: a static `mitem_t` array defines every row (label, type,
cvar pointer, range, dependency). The scroll window, animated highlight bar, and
fade-in/out all run from a single `UpdateMenuAnim()` call each frame.

**Two config files:**
- `oglconf.cfg` — shipped defaults + aim offsets. Loaded once on `F12`.
- `oglsave.cfg` — auto-written on every menu change via `SaveSettings()`.
  Loaded after `oglconf.cfg` on `F12` so user tweaks override defaults.
  Pressing `F10` resets everything back to `oglconf.cfg` defaults and deletes
  `oglsave.cfg`.

Panel positions (menu, F11, radar) are draggable with the **Move** entries and
persist in `oglsave.cfg`.
