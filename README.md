  

# Three Men's Morris (C++ & SFML)
🧩 A classic implementation of the ancient board game **Three Men's Morris** using C++ and the SFML graphics library. This is a two-player turn-based strategy game featuring placement, movement, and win detection via prime-number logic.

> **Note**  
> This project was created as part of a **University programming assignment**.  
> When I started, I had no experience with **SFML** and only limited knowledge of **C++** (and to be honest, I'm still learning!).  
> So the code might be a bit rough or unconventional in places.  
> If you spot anything that could be improved or have suggestions, feel free to **open a pull request** — contributions are very welcome! 😊
## 🕹️ Game Preview
<details>
<summary>Click Me</summary>
  <table align="center">
    <tr>
      <td align="center"> <img src="https://raw.githubusercontent.com/Sasivarnasarma/ThreeMensMorris/main/screenshots/ss1.png" alt="Screen Shot 1" width="300"/><br/> <sub>Start Menu</sub> </td>
      <td align="center"> <img src="https://raw.githubusercontent.com/Sasivarnasarma/ThreeMensMorris/main/screenshots/ss2.png" alt="Screen Shot 2" width="300"/><br/> <sub>Instructions Page</sub> </td>
      <td align="center"> <img src="https://raw.githubusercontent.com/Sasivarnasarma/ThreeMensMorris/main/screenshots/ss3.png" alt="Screen Shot 3" width="300"/><br/> <sub>About Page</sub> </td>
    </tr>
    <tr>
      <td align="center"> <img src="https://raw.githubusercontent.com/Sasivarnasarma/ThreeMensMorris/main/screenshots/ss4.png" alt="Screen Shot 4" width="300"/><br/> <sub>Playing Page</sub> </td>
      <td align="center"> <img src="https://raw.githubusercontent.com/Sasivarnasarma/ThreeMensMorris/main/screenshots/ss5.png" alt="Screen Shot 5" width="300"/><br/> <sub>Win Page A</sub> </td>
      <td align="center"> <img src="https://raw.githubusercontent.com/Sasivarnasarma/ThreeMensMorris/main/screenshots/ss6.png" alt="Screen Shot 6" width="300"/><br/> <sub>Win Page B</sub> </td>
    </tr>
  </table>
</details>

---
## ✨ Features
- 🎮 Two-player local game (Player A vs Player B)
- 🧠 Efficient win detection using prime multiplication
- 🧩 Smooth transitions between game phases: Start, Placement, Movement, and Win
- 🧼 Clean UI using custom textures and SFML
- 📦 Logs game activity to `game.log`
- 💡 Simple mouse-driven controls
- 🎨 Organized assets and modular code structure
---

## 🔢 Win Detection via Prime Multiplication

To detect if a player has aligned 3 tokens in a winning pattern, this project uses **unique prime numbers assigned to each board slot**:
> | 0 | 1 | 2 |  --> | 13 | 03 | 23 |
> 
> | 3 | 4 | 5 |  --> | 17 | 11 | 05 |
> 
> | 6 | 7 | 8 |  --> | 29 | 07 | 19 |
- Each winning combination (row, column, or diagonal) has a **unique product of its three primes**.
- A player's placed **token primes** are multiplied together.
- If the player's total prime product is divisible by any winning combo product, they win.

✅ This allows efficient win checking using basic arithmetic instead of nested loops or complex checks.

---

## 🔧 Installation & Run
### 🖥️ Option 1: Download Precompiled Executable
You can download a ready-to-run version of the game (including the required .dll files and assets) from the:
➡️ GitHub [Releases Page](https://github.com/Sasivarnasarma/ThreeMensMorris/releases)
📦 The ZIP archive includes:
- ThreeMensMorris.exe
- required .dll files
- assets/ folder (with images and UI textures)
📂 Just unzip and run .exe file!
### 🛠️ Option 2: Build from Source
### 📋 Prerequisites
- C++17 or later
- [SFML 2.6+](https://www.sfml-dev.org/download.php)
### 🖥️ Build Instructions
```bash
git  clone  https://github.com/Sasivarnasarma/ThreeMensMorris
cd  ThreeMensMorris
# Compile with g++ directly
g++  main.cpp  icon.res  -o  ThreeMensMorris  -lsfml-graphics  -lsfml-window  -lsfml-system  -mwindows
```
OR
**Compile via Visual Studio or MinGW with SFML linked**
### 🪟 Run Instructions
-   Place the compiled .exe file in the root project folder
-   Make sure the following are in place:
    -   The assets/ folder (with all images, fonts, etc.) is in the same directory as the .exe
    -   All required .dll files (e.g., sfml-graphics.dll, sfml-window.dll, etc.) are also in the same directory as the .exe
    ---
    
## ▶️ How to Play
**Controls**: Mouse-only interface.
### 🎯 Objective
Get three of your tokens in a row — horizontally, vertically, or diagonally — before your opponent does.

### ⚙️ Game Phases

🟢 Phase 1: Placement

- Players take turns placing their 3 tokens on any empty circle on the board.  
- Once all tokens are placed, the game transitions to the movement phase.

🔁 Phase 2: Movement
- On your turn, select one of your tokens.  
- Move it to an adjacent empty circle (no jumping allowed).  
- Use strategy to block your opponent and align your own tokens.

### 🏆 Winning the Game
- The first player to form a straight line of three tokens — horizontally, vertically, or diagonally — wins!
---

## 💻 Technologies Used

-   **C++** – for implementing game logic, win detection, and event handling.
-   **SFML (Simple and Fast Multimedia Library)** – for graphics rendering, window control, and interaction.
-   [**draw.io**](https://draw.io/) – for planning UI layout, board design, and creating mockups  
    ➤ All design templates are included in the `assets_drawio/` folder.
---

## 🧠 About the Project
This game was created as part of a **university project**, combining classic board game rules with **modern graphics programming**. The goal was to build a complete and playable graphical game using clean, efficient, and modular C++ code with SFML.

## 🤝 Contributing
Contributions are welcome! If you’d like to:
-   Add new features (AI opponent, sound effects, online multiplayer)
-   Improve the UI or UX
-   Fix bugs or optimize performance
-   Improve documentation
Feel free to fork the repo and open a pull request

## ⭐ Support
If you find this project helpful or fun, please consider giving it a **Star ⭐ on GitHub** — it really helps the project grow, reach more people, and motivates me to keep improving it!

## 📜 License
This project is licensed under the **MIT License**.

# Made with ❤️ by [Sasivarnasarma](https://github.com/Sasivarnasarma)