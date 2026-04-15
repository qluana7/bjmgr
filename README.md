# bjmgr

Baekjoon source code manager with solved.ac tier

---

# This project is archived.
As Baekjoon online judge service ends, we decided to archive this project.

Thanks for baekjoon and shiftpsh to service BOJ and solved.ac

---

- Language: C++17 · Build: CMake · License: MIT  
- Integrations: solved.ac API v3, libcurl, nlohmann_json  
- Optional: less, Visual Studio Code (`code` CLI)

## Index

- [Overview](#overview)
- [Quick Start](#quick-start)
- [Requirements](#requirements)
- [Directory Structure](#directory-structure)
- [CLI Reference](#cli-reference)
- [Installation](#installation)
- [Troubleshooting](#troubleshooting)
- [Notes on Security and Portability](#notes-on-security-and-portability)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

## Overview

bjmgr is a CLI tool that organizes your Baekjoon Online Judge (BOJ) solutions by their solved.ac tier/level.

- Auto-create and place solution files under tier/level folders (e.g., `Gold/Gold 5/1000.cpp`)
- Summarize local inventory by tier; filter by tier range
- Patch/sync files to updated tiers from solved.ac
- Optional integration with VS Code (`--code`)
- Note: Inventory scan currently counts only `.cpp` files
- Today it is C++-centric, but broader language support is planned (see Roadmap)

## Quick Start

```bash
# Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Help
./build/bjmgr help

# Create a file in the correct tier folder
./build/bjmgr new 1000

# Summarize your solutions by tier
./build/bjmgr info

# Fetch a problem's tier/title/link
./build/bjmgr get 11440

# Move files to updated tiers from solved.ac (with logging)
./build/bjmgr patch -l ./log.txt
```

## Requirements

- C++17 compiler, CMake ≥ 3.21  
- libcurl, nlohmann_json (build-time)  
- Optional: `less` (patch list), VS Code `code` CLI (`--code`)  
- Network access for solved.ac API operations

## Directory Structure

```
<workspace>/
├── Bronze/
│   └── Bronze 3/
│       └── 1001.cpp
├── Silver/
│   └── Silver 2/
│       └── 2000.cpp
├── Gold/
│   └── Gold 5/
│       └── 1000.cpp
...
```

Note: The inventory scanner currently includes `.cpp` files only.

## CLI Reference

<details>
<summary>Click to expand</summary>

### help
- Show help text and list commands.
```bash
./bjmgr help
./bjmgr help new
```

### info
- Scan a directory and summarize inventory by tier/level.
- Options:
  - `--search, -s <tier-range>`: Filter by tier (e.g., `b3..s1`, `d`, `b3..`, `..p2`)
  - `--dir, -d <path>`: Working directory (default: `.`)
- Examples:
```bash
./bjmgr info
./bjmgr info --search s1
./bjmgr info -s b3..s1
./bjmgr info -s ..p2 -d ./solutions
```

### get
- Fetch problem info (tier, title, link) by problem ID.
```bash
./bjmgr get <problem-id>
# examples
./bjmgr get 1000
./bjmgr get 11440
```

### new
- Fetch the tier from solved.ac and create a file in the corresponding tier folder.
- Required: `<problem-id>` (numeric)
- Options:
  - `--dir, -d <path>`: Working directory (default: `.`)
  - `--tier, -t <tier>`: Force tier manually (skip solved.ac), e.g., `D3`
  - `--extension, -x <ext>`: File extension (default: `cpp`)
  - `--yes, -y`: Skip confirmation prompts
  - `--code, -c`: Open created file in VS Code (uses `system()`)
- Examples:
```bash
./bjmgr new 1000
./bjmgr new 3024 -d ../ -t D3 -x cpp
./bjmgr new 15829 -x cxx --code
```

### patch
- Fetch current tiers from solved.ac and move files to correct tier directories (writes a log; optional patch list via `less`).
- Options:
  - `--log, -l <path>`: Log output file (default: `./log.txt`)
  - `--dir, -d <path>`: Working directory (default: `.`)
  - `--yes, -y`: Skip interactive confirmations
- Examples:
```bash
./bjmgr patch
./bjmgr patch --log ./log.txt -d ./solutions
```

### update
- Fetch all solved problems for a solved.ac user and create any missing files (interactive).
- Options:
  - `--log, -l <path>`: Log file
  - `--dir, -d <path>`: Working directory
  - `--filter, -f <tier-range>`: Filter by tier range
  - `--extension, -x <ext>`: File extension (default: `cpp`)
  - `--yes, -y`: Skip confirmations
  - `--code, -c`: Open created files in VS Code (uses `system()`)
- Examples:
```bash
./bjmgr update solvedac
./bjmgr update solvedac -d ../
./bjmgr update solvedac --log ./log.txt -x cpp --code
```

</details>

## Installation

### Linux (Debian/Ubuntu)
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev
git clone https://github.com/qluana7/bjmgr
cd bjmgr
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### macOS (Homebrew)
```bash
brew install cmake curl nlohmann-json
git clone https://github.com/qluana7/bjmgr
cd bjmgr
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
# If packages are not found, try:
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix)"
```

### Windows
- Recommended: MSYS2 or WSL for a Unix-like environment
```bash
git clone https://github.com/qluana7/bjmgr
cd bjmgr
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Build-time option
Disable ANSI colors:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DDISABLE_ANSI=ON
cmake --build build --config Release
```

## Troubleshooting

- Build cannot find libcurl or nlohmann_json  
  - Ensure dev packages are installed (e.g., `libcurl4-openssl-dev`, `nlohmann-json3-dev`; `brew install curl nlohmann-json`)  
  - Provide CMake hints:
    ```bash
    cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix)"
    ```
- HTTP/curl errors or 429 (Too Many Requests)  
  - Check network connectivity; wait if 429 occurs
- `less` not found  
  - Install `less` or use `-y` to skip patch list viewing
- `--code` does nothing  
  - Ensure VS Code is installed and `code` CLI is in PATH
- Inventory misses files  
  - Current scanner counts `.cpp` only
- ANSI colors look broken  
  - Rebuild with `-DDISABLE_ANSI=ON`

## Notes on Security and Portability

- `system()` is used for optional `less` and `code` integrations.
  - This is convenient but may affect portability/security. Use `--code` only if you trust your environment.
- Network access is required for solved.ac API operations and is subject to rate limits/availability.
- Color output uses ANSI sequences (can be disabled at build-time).

## Roadmap

- Replace or refactor `system()` calls to improve portability and security.
- Support customizable directory structures (user-defined mapping).
- Enhance editor integrations beyond VS Code.
- Provide portable patch/diff viewing that does not depend on external tools.
- Improve inventory scanning to include more file extensions.
- Support for more programming languages beyond C++.

## Contributing

Issues and PRs are welcome. For major changes, please open an issue first to discuss your proposal.

## License

MIT — see [LICENSE](./LICENSE).

---

<p align="center">
  <a href="https://solved.ac/">solved.ac</a> | <a href="https://www.acmicpc.net/">Baekjoon Online Judge</a>
</p>
