# OCIC — Minimal OCI Runtime

## Toolchain
- **Compiler:** GCC 13.3.0
- **C Standard:** `-std=gnu17` (C17 + GNU extensions)
- **Platform:** Linux x86_64, Ubuntu 24.04.3 LTS
- **Build:** `-static` (fully static binary, no dynamic linking)

## Build & Run
```sh
gcc -std=gnu17 -Wall -Werror -O2 -static -o main main.c
sudo ./main
```

### GCC Flags
| Flag | Purpose |
|------|---------|
| `-std=gnu17` | C17 standard with GNU extensions |
| `-Wall` | Enable all warnings |
| `-Werror` | Treat warnings as errors |
| `-O2` | Optimization level 2 (speed/size) |
| `-static` | Static linking — no dynamic libc, fully portable ELF |
| `-o` | Output binary name |

## Editor Setup (VS Code)

### 1. Install Extensions
- **C/C++** (Microsoft)
- **clang-format** (xaver.clang-format)

### 2. Install clang-format
The extension requires the system binary. Install via apt:
```sh
sudo apt install clang-format
```
This installs to `/usr/bin/clang-format`. The extension looks for it in PATH, or use the explicit path configured in settings.json below.

### 3. Configure VS Code
Create `.vscode/settings.json`:
```json
{
    "C_Cpp.default.cStandard": "gnu17",
    "[c]": {
        "editor.formatOnSave": true,
        "editor.defaultFormatter": "xaver.clang-format"
    },
    "editor.tabSize": 4,
    "editor.useTabStops": false,
    "clang-format.executable": "/usr/bin/clang-format"
}
```

Create `.vscode/c_cpp_properties.json`:
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": ["${workspaceFolder}/**"],
            "defines": ["_GNU_SOURCE"],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "gnu17"
        }
    ],
    "version": 4
}
```

### 4. Format on Save
Enabled in settings above. `.clang-format` config is in the project root.

Reload VS Code window after setup.

## Namespaces Implemented
| # | Namespace | Status |
|---|-----------|--------|
| 1 | UTS       | done |
| 2 | PID       | — |
| 3 | Mount     | — |
| 4 | User      | — |
| 5 | IPC       | — |
| 6 | Network   | — |
