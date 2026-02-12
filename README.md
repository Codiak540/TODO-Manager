# TODO-BBS

A retro-styled, BBS-inspired TODO list manager for the terminal.

## Features

- Retro box-drawing UI with ANSI colors
- Priority and regular TODO lists
- Local or global file storage and viewing
- Commit-based workflow
- Priority conflict resolution with auto-bump or manual reassignment

## Installation

### From AUR (Arch Linux)

```bash
yay -S todo-bbs
# or
paru -S todo-bbs
```

### From Source

#### Requirements
- C++11 compatible compiler (g++, clang++)
- CMake 3.10 or higher

#### Build Instructions

```bash
git clone https://github.com/yourusername/todo-bbs.git
cd todo-bbs
mkdir build && cd build
cmake ..
make
sudo make install
```

Or simply compile directly:

```bash
g++ -std=c++11 main.cpp boxes.cpp -o TODO_Manager
./TODO_Manager
```

## Usage

Run the program:
```bash
todo-bbs
```

On first run, choose between:
- **Local mode**: Stores TODO lists in current directory
- **Global mode**: Stores TODO lists in `~/Documents/todo/`

### Commands

1. **View TODO Lists** - Display both priority and regular lists
2. **Add Item** - Add a new priority or regular TODO item
3. **Remove Item** - Remove an item from either list
4. **Commit Changes** - Save changes to disk
5. **Exit** - Quit (warns about uncommitted changes)

### Priority Conflict Resolution

When adding a priority item with an existing priority number, you can:
- **Bump**: Automatically increment all conflicting priorities
- **Reassign**: Manually reassign conflicting items
- **Cancel**: Abort the addition

## File Format

Priority items are stored as:
```
<priority>|<description>
```

Regular items are stored as:
```
<description>
```

## Screenshots

```
╔═════════════════════════════╗
│    ░▒▓ TODO-BBS v1.0 ▓▒░    │
│ A Retro styled Todo Manager │
╚═════════════════════════════╝
  [✓] All changes committed

  ╔═══ PRIORITY TODO LIST ═══╗
  │   [1] Fix critical bug   │
  │   [2] Update docs        │
  ╚══════════════════════════╝
  ╔═══ REGULAR TODO LIST ═══╗
  │   • Buy groceries       │
  │   • Call dentist        │
  ╚═════════════════════════╝
```

## License

GNU General Public License - see LICENSE file for details

## Contributing

Contributions welcome! Please open an issue or submit a pull request.