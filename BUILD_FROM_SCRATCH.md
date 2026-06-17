# Build from scratch (create the Visual Studio project yourself)

This guide is for when you only have the **source files** and want to build the
hack by creating a fresh Visual Studio project from an empty project — instead of
opening the provided `cs16OpenGL.slnx`.

It documents the exact path of menus, dialogs and options.

> [!NOTE]
> This is for research/educational purposes only. See the disclaimer in
> [README.md](./README.md).

---

## 0. Prerequisites

Install the right tool first — this is the #1 source of confusion:

| You need | You do NOT need |
|----------|-----------------|
| **Visual Studio** (the IDE) — Community edition is free | ❌ Visual Studio **Code** (it's a different, lightweight editor) |
| Workload: **Desktop development with C++** | ❌ Visual C++ **Redistributable** alone (that's just runtime libraries, no compiler) |

1. Download **Visual Studio Community** from
   <https://visualstudio.microsoft.com/downloads/>.
2. In the installer, on the **Workloads** screen, tick
   **"Desktop development with C++"**. The defaults are fine — just make sure
   **"MSVC ... build tools (x64/x86)"** and a **Windows SDK** are checked.
3. Install, then confirm you can launch **Visual Studio** (not VS Code).

The two things that actually matter for this build: the **MSVC compiler** and a
**Windows SDK**. Both are included by that workload.

---

## 1. Gather the source files

You need these files in one folder:

- `opengl32.cpp` — main hack logic
- `OtherOGL.cpp` — wrapper boilerplate (loads the real DLL, `__asm` trampolines)
- `opengl32.h` — function typedefs
- `vars.h` — global variables / cvars
- `opengl32.def` — module definition file (re-exports every OpenGL function)

> [!TIP]
> In this repo they live in [`cs16OpenGL/`](./cs16OpenGL/).

---

## 2. Create an **Empty Project**

In Visual Studio: **Create a new project**.

1. Filter the template list: Language **C++**, Platform **Windows**, Project type **Desktop**.
2. Choose **"Empty Project"** (description: *"Start from scratch with C++ for
   Windows. Provides no starting files."*) → **Next**.
3. Name it (e.g. `cs16OpenGL`), pick a location → **Create**.

> [!WARNING]
> Do **not** pick these templates:
> - ❌ **Dynamic-Link Library (DLL)** — it generates its own `dllmain.cpp` with a
>   `DllMain`, which collides with the `DllMain` already in `opengl32.cpp`
>   (duplicate symbol / linker error).
> - ❌ **Console App** — generates a `main()` you don't want.
>
> An **Empty Project** has no starter files, so nothing conflicts.

---

## 3. Add the source files to the project

1. In **Solution Explorer**, right-click the project → **Add → Existing Item…**
2. Select all five files (`opengl32.cpp`, `OtherOGL.cpp`, `opengl32.h`,
   `vars.h`, `opengl32.def`) → **Add**.

Both `.cpp` files should now appear under **Source Files**.

---

## 4. Select the configuration: **Release + x86**

On the top toolbar there are two dropdowns:

- Left dropdown → **Release**
- Right dropdown → **x86** (a.k.a. Win32)

> [!IMPORTANT]
> The platform **must be x86 (32-bit)**:
> - CS 1.6 is a 32-bit process, so the injected DLL must be 32-bit.
> - `OtherOGL.cpp` contains hundreds of `__asm { jmp ... }` inline-assembly
>   trampolines that MSVC only compiles when targeting x86. Building x64 fails
>   with *"inline assembler not supported on this target"*.
>
> If you only see `x64`, open the dropdown → **Configuration Manager** →
> Platform column → **New… → x86**.

---

## 5. Configure Project Properties

Right-click the project → **Properties**. At the top-left of the dialog set
**Configuration: Release** and **Platform: Win32** (or "All Configurations" if you
prefer to apply everywhere) before changing anything.

### 5.1 General

| Setting | Value |
|---------|-------|
| **Configuration Type** | **Dynamic Library (.dll)** |
| **Target Name** | **`opengl32`** (so the output is `opengl32.dll`) |

### 5.2 Advanced

| Setting | Value |
|---------|-------|
| **Character Set** | **Use Multi-Byte Character Set** |

> Why: the code calls ANSI Win32 APIs with `char*` (e.g. `GetCurrentDirectory`,
> `CreateFont`, `LoadLibrary`). The default **Unicode** makes those resolve to the
> wide (`...W`, `wchar_t*`) versions → many `cannot convert 'char*' to 'LPWSTR'`
> errors. Multi-Byte makes them resolve to the ANSI (`...A`) versions.

### 5.3 C/C++ → General

| Setting | Value |
|---------|-------|
| **SDL checks** | **No (/sdl-)** |
| **Treat Warnings As Errors** | **No (/WX-)** |

### 5.4 C/C++ → Preprocessor

Add to **Preprocessor Definitions**:

```
_CRT_SECURE_NO_WARNINGS
```

> Why: silences the `C4996` "this function is unsafe" errors for old CRT calls
> like `sscanf`, `strcpy`, `sprintf`, `fopen`.

### 5.5 C/C++ → Language

| Setting | Value |
|---------|-------|
| **Conformance mode** | **No (/permissive)** |

> Why: lets the legacy code assign string literals to `char*`
> (e.g. `char *timestring = "..."`, `LoadFile("oglconf.cfg", 0)`), which strict
> conformance mode rejects with `C2440`/`C2664`. If a string-literal error still
> appears, also add `/Zc:strictStrings-` under *C/C++ → Command Line → Additional Options*.

### 5.6 C/C++ → Advanced  ⭐ (the one that prevents a crash)

| Setting | Value |
|---------|-------|
| **Calling Convention** | **`__stdcall` (/Gz)** |

> [!CAUTION]
> This is the single most important setting. OpenGL functions are `__stdcall`,
> but the wrapper's function-pointer typedefs don't specify a convention — so they
> inherit the project default. If that default is `__cdecl` (the VS default), every
> call mismatches the real OpenGL functions → the stack gets corrupted → **the game
> compiles fine but crashes on launch**. Setting `/Gz` makes everything `__stdcall`.
> (Variadic functions like `sprintf`/`DrawText(...)` automatically stay `__cdecl`,
> so this is safe.)

### 5.7 C/C++ → Code Generation

| Setting | Value |
|---------|-------|
| **Runtime Library** | **Multi-threaded DLL (/MD)** *or* **Multi-threaded (/MT)** |

> - **/MD**: smaller DLL, but the target machine needs the matching
>   **Visual C++ Redistributable** installed.
> - **/MT**: larger, self-contained DLL with no external dependency. Choose this
>   if you'll run it on a clean Windows machine.

### 5.8 Linker → Input

| Setting | Value |
|---------|-------|
| **Module Definition File** | **`opengl32.def`** |
| **Additional Dependencies** | add `opengl32.lib;glu32.lib;gdi32.lib;user32.lib;winmm.lib` |

> Do **not** add `glaux.lib` — `glaux` was removed from modern Windows SDKs and
> isn't used by the code.

### 5.9 Linker → Advanced

| Setting | Value |
|---------|-------|
| **Entry Point** | **leave empty** |

> Why: leaving it blank uses the default `_DllMainCRTStartup`, which initializes
> the CRT and *then* calls the code's `DllMain`. The CRT must be initialized
> because the hack uses `fopen`, `sscanf`, `sprintf`, etc. (Don't force the entry
> point to `DllMain` — that skips CRT init.)

Click **OK**.

---

## 6. (Only if needed) remove `glaux`

`glaux.h` no longer ships with the Windows SDK. If your source still includes it,
comment out these lines (in this repo they are already commented out):

```cpp
// in opengl32.h and opengl32.cpp
//#include <gl/glaux.h>
```

Nothing in the code calls `aux*` functions, so removing it is harmless.

---

## 7. Build

**Build → Build Solution** (`Ctrl + Shift + B`).

Success looks like:

```
========== Build: 1 succeeded, 0 failed ... ==========
```

### Troubleshooting (error → cause → fix)

| Symptom | Cause | Fix |
|---------|-------|-----|
| `cannot convert 'char*' to 'LPWSTR'` (`C2664`/`E0167`) on `GetCurrentDirectoryW`, `CreateFontW`, `LoadLibraryW` | Project is Unicode | Step 5.2 — Multi-Byte Character Set |
| `'sscanf'/'strcpy'/... unsafe` (`C4996`) | Deprecation warnings escalated to errors | Step 5.4 (`_CRT_SECURE_NO_WARNINGS`) + Step 5.3 (SDL off, WX off) |
| `cannot convert 'const char[N]' to 'char*'` (`C2440`/`C2664`) | Strict conformance | Step 5.5 (Conformance mode No) |
| `inline assembler not supported on this target` | Building x64 | Step 4 — switch to x86 |
| `cannot open 'gl/glaux.h'` | glaux removed from SDK | Step 6 — comment the include |
| **Builds OK but the game crashes on launch** | Wrong calling convention | Step 5.6 — `__stdcall` (/Gz) |

---

## 8. Install & run

1. The DLL is at `<project>\Release\opengl32.dll`.
2. Copy it into your Counter-Strike 1.6 folder, together with `oglconf.cfg`
   (see [README.md → How to use](./README.md#how-to-use)).
3. In-game: `F12` to enable, `Insert` to open the menu.

> [!IMPORTANT]
> The hack only works on **CS 1.6 build 4554 or below**.
