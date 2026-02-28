#   Game Console

A Linux-based virtual game console that runs three terminal games — **Snake**, **Tic-Tac-Toe**, and **Space Invaders** — inside a mounted disk image using a simple menu-driven main screen.

>  **Linux only.** The project relies on POSIX APIs (`fork`, `losetup`, `mount`, `termios`) that are not available on Windows or macOS.

---

## Prerequisites

- Linux
- GCC (C compiler)
- `sudo` access (required for `losetup` and `mount`)
- `dd`, `mkfs.ext4` (usually available by default)

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/<your-username>/game-console.git
cd game-console
```

### 2. Initialize the disk image

Run this **once** to create the 100 MB `storage_vgc.img` disk image and format it as ext4.

```bash
bash initialize.sh
```

### 3. Build the games

Compile the four C source files. The binaries must be placed inside the mounted image directory.

```bash
# First start the environment so the mount directory exists
bash startup.sh

# Compile game sources
gcc src/src1.c -o mount/game_snake
gcc src/src2.c -o mount/game_invadespace
gcc src/src3.c -o mount/game_tictactoe
gcc src/main-screen.c -o mount/main-screen
```

### 4. Start the console

```bash
bash startup.sh   # mounts the image (skip if already mounted)
cd mount
./main-screen
```

### 5. Stop the console

From a separate terminal, or after quitting the main screen:

```bash
cd ..
bash terminate.sh   # unmounts cleanly
```

To also delete the disk image and all generated files:

```bash
bash purge.sh
```

---

## Controls

### Main Screen

| Key | Action |
|-----|--------|
| `W` | Move selection up |
| `S` | Move selection down |
| `Enter` | Launch selected game |
| `Q` | Quit the console |

### Snake (`src1.c` → `game_snake`)

| Key | Action |
|-----|--------|
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `Q` | Quit game |

- Grid: 15 × 15
- Snake head: `O`, tail: `#`, food: `X`
- Hitting a border or the tail pauses movement until the next valid input.

###  Tic-Tac-Toe (`src3.c` → `game_tictactoe`)

| Key | Action |
|-----|--------|
| `1`–`9` | Place `X` on the corresponding cell (numpad layout) |
| `Q` | Quit game |

- You play as `X`, the computer plays as `O`.
- The computer picks a random empty cell on its turn.
- The game displays **YOU WON!**, **YOU LOST!**, or **DRAW!** and restarts.

###  Space Invaders (`src2.c` → `game_invadespace`)

| Key | Action |
|-----|--------|
| `A` | Move player left |
| `D` | Move player right |
| `Space` | Fire (one shot at a time) |
| `Q` | Quit game |

- Grid: 15 × 20. Player: `*`, bullet: `^`, enemy: `O`.
- Shoot all 6 enemies to win. A new shot can only be fired after the current one is gone.

---

## Project Structure

```
game-console/
├── src/
│   ├── main-screen.c   # Menu UI / game launcher
│   ├── src1.c          # Snake game
│   ├── src2.c          # Space Invaders game
│   └── src3.c          # Tic-Tac-Toe game
├── initialize.sh       # Create & format disk image (run once)
├── startup.sh          # Mount image and create loop device
├── terminate.sh        # Unmount image and remove loop device
├── purge.sh            # Full cleanup (unmount + delete image)
└── README.md
```




