# Building cs16-opengl-research

> [!NOTE]
> For research / educational purposes only. See the disclaimer in [README.md](./README.md).

---

## Table of Contents

- [Quick build (you already have the `.vcxproj`)](#quick-build)
- [Build from scratch (create the VS project yourself)](#build-from-scratch)
  - [0. Prerequisites](#0-prerequisites)
  - [1. Gather the source files](#1-gather-the-source-files)
  - [2. Create an Empty Project](#2-create-an-empty-project)
  - [3. Add the source files](#3-add-the-source-files)
  - [4. Select Release + x86](#4-select-the-configuration-release--x86)
  - [5. Configure Project Properties](#5-configure-project-properties)
  - [6. Remove glaux (if needed)](#6-only-if-needed-remove-glaux)
  - [7. Build](#7-build)

---

## Quick build

The Visual Studio solution is `cs16OpenGL.slnx` (project: `cs16OpenGL/cs16OpenGL.vcxproj`).

### Requirements

- **Visual Studio 2017 or newer** (any edition, including Community) with the
  **"Desktop development with C++"** workload (MSVC toolset + Windows SDK).
- You must use the **MSVC** compiler — the code contains `__asm` inline assembly
  trampolines that only build with MSVC targeting **x86**.

### Steps

1. Open `cs16OpenGL.slnx` in Visual Studio.
2. Set the active configuration to **`Release`** and platform to **`x86` (Win32)**.
   > CS 1.6 is a 32-bit process, and the inline assembly only compiles for x86.
   > All project settings are stored under `Release | Win32`; selecting another
   > configuration will show empty settings.
3. **Build → Build Solution** (`Ctrl + Shift + B`). Output: `Release/opengl32.dll`.

The project already has all required settings for `Release | Win32`:

| Setting | Value | Why |
|---------|-------|-----|
| Configuration Type | Dynamic Library (`.dll`) | It's a DLL |
| Target Name | `opengl32` | Must be named `opengl32.dll` |
| Platform | x86 / Win32 | 32-bit + inline asm |
| Calling Convention | `__stdcall` (`/Gz`) | OpenGL uses `__stdcall`; wrong convention = crash |
| Character Set | Multi-Byte | Code uses ANSI `char*` Win32 APIs |
| Conformance mode | No (`/permissive`) | Allows legacy string-literal → `char*` |
| Preprocessor | `_CRT_SECURE_NO_WARNINGS` | Old CRT functions (`sscanf`, `strcpy`, …) |
| Module Definition File | `opengl32.def` | Re-exports all OpenGL functions (proxy) |
| Additional Dependencies | `opengl32.lib; glu32.lib; gdi32.lib; user32.lib; winmm.lib` | Required libs |

### Runtime Library note

> [!IMPORTANT]
> This project builds with **Multi-threaded DLL (`/MD`)** by default. The CRT is
> linked dynamically, so the target machine must have the matching
> **Microsoft Visual C++ Redistributable** installed (any machine with Visual
> Studio already has it).
>
> For a **self-contained** DLL with no redistributable dependency, switch to
> **Multi-threaded (`/MT`)**: *Project Properties → C/C++ → Code Generation →
> Runtime Library → `/MT`*. The DLL will be larger but runs on a clean Windows
> machine.

---

## Build from scratch

This section is for when you only have the **source files** and want to build by
creating a fresh Visual Studio project from an empty project — instead of opening
the provided `cs16OpenGL.slnx`. It documents the exact menus, dialogs, and options.

---

### 0. Prerequisites

| You need | You do NOT need |
|----------|-----------------|
| **Visual Studio** (the IDE) — Community edition is free | ❌ Visual Studio **Code** (different, lightweight editor) |
| Workload: **Desktop development with C++** | ❌ Visual C++ **Redistributable** alone (just runtime libs, no compiler) |

1. Download **Visual Studio Community** from <https://visualstudio.microsoft.com/downloads/>.
2. In the installer, on the **Workloads** screen, tick **"Desktop development with C++"**.
   The defaults are fine — just make sure **"MSVC … build tools (x64/x86)"** and a
   **Windows SDK** are checked.
3. Install, then confirm you can launch **Visual Studio** (not VS Code).

---

### 1. Gather the source files

You need these files in one folder:

- `opengl32.cpp` — main hack logic
- `OtherOGL.cpp` — wrapper boilerplate (loads the real DLL, `__asm` trampolines)
- `opengl32.h` — function typedefs
- `vars.h` — global variables / cvars
- `opengl32.def` — module definition file (re-exports every OpenGL function)

> [!TIP]
> In this repo they live in [`cs16OpenGL/`](./cs16OpenGL/).

---

### 2. Create an Empty Project

In Visual Studio: **Create a new project**.

1. Filter the template list: Language **C++**, Platform **Windows**, Project type **Desktop**.
2. Choose **"Empty Project"** → **Next**.
3. Name it (e.g. `cs16OpenGL`), pick a location → **Create**.

> [!WARNING]
> Do **not** pick these templates:
> - ❌ **Dynamic-Link Library (DLL)** — generates its own `dllmain.cpp` that
>   collides with the `DllMain` already in `opengl32.cpp`.
> - ❌ **Console App** — generates a `main()` you don't want.

---

### 3. Add the source files

1. In **Solution Explorer**, right-click the project → **Add → Existing Item…**
2. Select all five files (`opengl32.cpp`, `OtherOGL.cpp`, `opengl32.h`,
   `vars.h`, `opengl32.def`) → **Add**.

---

### 4. Select the configuration: Release + x86

On the top toolbar:

- Left dropdown → **Release**
- Right dropdown → **x86** (a.k.a. Win32)

> [!IMPORTANT]
> The platform **must be x86 (32-bit)**:
> - CS 1.6 is a 32-bit process.
> - `OtherOGL.cpp` contains `__asm { jmp ... }` trampolines that MSVC only
>   compiles for x86. Building x64 fails with *"inline assembler not supported
>   on this target"*.
>
> If you only see `x64`, open the dropdown → **Configuration Manager** →
> Platform column → **New… → x86**.

---

### 5. Configure Project Properties

Right-click the project → **Properties**. At the top-left set
**Configuration: Release** and **Platform: Win32** before changing anything.

#### 5.1 General

| Setting | Value |
|---------|-------|
| **Configuration Type** | **Dynamic Library (.dll)** |
| **Target Name** | **`opengl32`** |

#### 5.2 Advanced

| Setting | Value |
|---------|-------|
| **Character Set** | **Use Multi-Byte Character Set** |

> The code calls ANSI Win32 APIs with `char*`. The default Unicode makes those
> resolve to wide (`...W`) versions → many `cannot convert 'char*' to 'LPWSTR'` errors.

#### 5.3 C/C++ → General

| Setting | Value |
|---------|-------|
| **SDL checks** | **No (/sdl-)** |
| **Treat Warnings As Errors** | **No (/WX-)** |

#### 5.4 C/C++ → Preprocessor

Add to **Preprocessor Definitions**:

```
_CRT_SECURE_NO_WARNINGS
```

#### 5.5 C/C++ → Language

| Setting | Value |
|---------|-------|
| **Conformance mode** | **No (/permissive)** |

> Lets legacy code assign string literals to `char*`, which strict conformance mode rejects.

#### 5.6 C/C++ → Advanced ⭐ (prevents a crash)

| Setting | Value |
|---------|-------|
| **Calling Convention** | **`__stdcall` (/Gz)** |

> [!CAUTION]
> This is the single most important setting. OpenGL functions are `__stdcall`, but
> the wrapper's function-pointer typedefs don't specify a convention — they inherit
> the project default. If the default is `__cdecl` (the VS default), every call
> mismatches the real OpenGL → stack corruption → **game crashes on launch**.
> Setting `/Gz` fixes this. Variadic functions like `sprintf` automatically stay
> `__cdecl`, so this is safe.

#### 5.7 C/C++ → Code Generation

| Setting | Value |
|---------|-------|
| **Runtime Library** | **Multi-threaded DLL (/MD)** or **Multi-threaded (/MT)** |

> `/MD` = smaller DLL, requires the Visual C++ Redistributable on target.
> `/MT` = larger, fully self-contained, no external dependency.

#### 5.8 Linker → Input

| Setting | Value |
|---------|-------|
| **Module Definition File** | **`opengl32.def`** |
| **Additional Dependencies** | `opengl32.lib;glu32.lib;gdi32.lib;user32.lib;winmm.lib` |

> Do **not** add `glaux.lib` — it was removed from modern Windows SDKs.

#### 5.9 Linker → Advanced

| Setting | Value |
|---------|-------|
| **Entry Point** | **leave empty** |

> Leaving it blank uses `_DllMainCRTStartup`, which initializes the CRT and then
> calls `DllMain`. CRT must be initialized because the hack uses `fopen`, `sscanf`,
> `sprintf`, etc.

Click **OK**.

---

### 6. (Only if needed) remove `glaux`

`glaux.h` no longer ships with the Windows SDK. In this repo the include is already
commented out. If your copy still has it, comment these lines:

```cpp
//#include <gl/glaux.h>
```

---

### 7. Build

**Build → Build Solution** (`Ctrl + Shift + B`).

Success looks like:

```
========== Build: 1 succeeded, 0 failed ... ==========
```

#### Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| `cannot convert 'char*' to 'LPWSTR'` (`C2664`) | Unicode character set | Step 5.2 — Multi-Byte |
| `'sscanf'/'strcpy'/... unsafe` (`C4996`) | Deprecation warnings as errors | Step 5.4 + Step 5.3 |
| `cannot convert 'const char[N]' to 'char*'` | Strict conformance | Step 5.5 — Conformance No |
| `inline assembler not supported on this target` | Building x64 | Step 4 — switch to x86 |
| `cannot open 'gl/glaux.h'` | glaux removed from SDK | Step 6 — comment the include |
| **Builds OK but game crashes on launch** | Wrong calling convention | Step 5.6 — `__stdcall` (/Gz) |

#### Install & run

1. The DLL is at `<project>\Release\opengl32.dll`.
2. Copy it into your Counter-Strike 1.6 folder together with `oglconf.cfg`.
3. In-game: `F12` to enable, `Insert` to open the menu.

> [!IMPORTANT]
> The hack only works on **CS 1.6 build 4554 or below**.
