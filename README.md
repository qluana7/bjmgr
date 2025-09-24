<div id="toc">
  <ul style="list-style: none;" align="center">
    <summary>
      <h1> bjmgr </h1>
    </summary>
  </ul>
</div>

<p align="center"><i>Baekjoon source code manager with solved.ac tier</i></p>

## Overview

**bjmgr** is a utility for managing your Baekjoon Online Judge (BOJ) solutions, organizing them according to their solved.ac tier. It helps keep your codebase neat and allows you to easily track your progress and performance across different problem difficulties.

Written primarily in C++, bjmgr is designed for users who regularly solve BOJ problems and want a streamlined way to store and access their source code.

## Features

- **Tier-based organization**: Solutions are stored in directories named after solved.ac tiers (e.g., Bronze, Silver, Gold), with subdirectories for each tier level (e.g., Gold 5).
- **Automatic file placement**: When managing code, files are placed in the correct tier and level directory.
- **View patch files**: Uses the `less` pager (if installed) via the `system()` method for viewing diffs and patches.
- **VSCode integration**: Some commands support the `code` operation to open or create files directly in Visual Studio Code (requires VSCode and its `code` command available in your PATH).
- **Help command**: Lists all available commands and usage information.
- **C++ support**: Primarily intended for C++ solutions.

## System and External Dependencies

- **C++ compiler:** A C++ compiler such as g++ or clang++ is required to build and use bjmgr.
- **CMake:** Needed for building the project.
- **External utilities:** Some features depend on external commands like `less` for viewing patch files. Make sure `less` is installed if you want to use related features.
- **Visual Studio Code (Optional):** Commands that support the `code` operation require [Visual Studio Code](https://code.visualstudio.com/) and the `code` command available in your system's PATH. If VSCode or the `code` command is not installed, these features will not work, but the core functionality of bjmgr remains unaffected.
- **system() method:** This project uses the standard C++ `system()` method to call external commands. Be aware of portability and security considerations when running bjmgr on different environments.

## Getting Started

### Prerequisites

See the System and External Dependencies section.

### Build

```bash
git clone https://github.com/qluana7/bjmgr
cd bjmgr

# Use default Makefile generator
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
make -C build

# Use ninja for faster builds (optional)
cmake -S . -B build -Gninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

## Usage

To see available commands and usage instructions, run:

```bash
./bjmgr help
```

This command will display all supported features and their usage.

## Directory Structure Example

```
bjmgr/
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

## Roadmap / TODO

- Remove or refactor use of the `system()` method to improve portability and security.
- Provide support for user-defined directory structures, allowing users to customize how solution files are organized.
- Improve editor integration and support additional editors besides VSCode.
- Add more flexible and portable patch/diff viewing capabilities not dependent on external utilities.
- Additional enhancements and features planned for future releases.

## Contributing

Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License.

---

<p align="center">
  <a href="https://solved.ac/">solved.ac</a> | <a href="https://www.acmicpc.net/">Baekjoon Online Judge</a>
</p>