# Echoes of Aurion

## Game Description

**Echoes of Aurion** is a 2D story-driven adventure RPG created using the **iGraphics** library in C/C++. Five sacred seals hold a barrier over the kingdom of Aurion, and one Guardian in every generation is chosen to watch over them. When the seals begin to weaken and the Guardian disappears while investigating, the player — his apprentice — is sent to find him.

The player travels through four regions, always one step behind the man he is looking for, and reaches him only in the final region.

## Features
- Four explorable regions with scrolling maps and a camera that follows the player.
- Six mini-games, two of which the player chooses between, so two regions are worth replaying.
- Branching dialogue with a typewriter effect and a different box design for each speaker.
- An inventory of five collectible items, three of them optional perks that make mini-games easier.
- A save system that stores the player's name, items and progress, resumed from the main menu.
- Background music, button sound effects, and a settings screen to toggle both.
- Fade transitions, animated title cards, full-screen story scenes and visual effects.
- Sixteen hand-drawn maps and 249 art assets, all drawn by the team.

## Project Details
IDE: Visual Studio 2013

Language: C, C++

Platform: Windows PC

Resolution: 1280 x 720

Genre: 2D story-driven adventure RPG

## How to Run the Project

### Option 1 — Just play it (no setup)

A built Release executable is included in the repository.

1. Download the repository (**Code → Download ZIP**) and extract it.
2. Open the `demo` folder.
3. Run **`demo.exe`**.

Keep the `.exe` where it is — it loads its assets from the `Images` and `Audios` folders beside it using relative paths, so moving it on its own will leave the game without art or sound. Run it from the extracted folder, not from inside the ZIP.

Windows may show a "Windows protected your PC" warning the first time, since the executable is unsigned. Click **More info → Run anyway**.

### Option 2 — Build from source

Make sure you have the following installed:
- **Visual Studio 2013**
- **iGraphics Library** (included in this repository)

Open the project in Visual Studio 2013
- Open Visual Studio 2013.
- Go to File → Open → Project/Solution.
- Locate and select the .sln file from the cloned repository.
- Click Build → Build Solution
- Run the program by clicking Debug → Start Without Debugging

The Images and Audios folders must stay beside the project file, as assets are loaded using relative paths.

## How to Play

### **Controls**
| Action | Move | Interact / Talk / Choose | Advance Dialogue | Use Perk Item |
|-------------|----------|-----------|-----------|-------|
| **Input** | `W` `A` `S` `D` or Arrow Keys | Left Mouse Click | Left Mouse Click | `SPACE` |

### **Game Rules**

- Every mini-game is timed and allows a limited number of mistakes before it fails, and can be retried.
- Optional perk items are given by NPCs the player is not required to meet. Each one makes a specific mini-game easier, but the game can be finished without any of them.
- In The Warded Dark, a wardstone only charges while the player stands completely still, and a single step resets it. Alcoves hide the player from the ghosts.
- In The Mirror Hall, the player and his reflection move on the same keys, but the reflection's left and right are inverted. Both must reach their own sigil at the same moment.
- In The Sealing, the player has five Resolve. Every attack is shown on the floor before it lands. Being hit costs one Resolve and resets the seal being channelled.
- Progress is saved automatically on reaching each new region.

## Project Contributors

1. Masaba Juairia
2. Jasia Jabin
3. Abrer Hossen Mahi

## Screenshots

### **Main Menu**
<img src="https://github.com/user-attachments/assets/e7c5bbdf-23d7-405c-96e5-7401610a9fa2" width="450">

### **Emberfall Village**
<img src="https://github.com/user-attachments/assets/69588464-81f0-438f-b23f-24cb259a3cb7" width="450">

### **The Warded Dark**
<img src="https://github.com/user-attachments/assets/0a12b07f-4eee-478f-940a-d0b823512495" width="450">

### **The Sealing**
<img src="https://github.com/user-attachments/assets/11591d90-9154-48b5-9cac-e4e0e9a67d80" width="450">

## Credits

Background music from [Pixabay](https://pixabay.com/music/) (Pixabay Content License).

All character art, backgrounds, maps and UI assets were drawn by the project team.

## Youtube Link
[CSE 1200 Project: Echoes of Aurion](https://youtu.be/_3myMHOHWFc)

## Project Report
[Project Report: Echoes of Aurion](https://github.com/user-attachments/files/33013212/Echoes.of.Aurion.Final.Report.pdf)
