# Graphics-project-358

🏀 Basketball Game using OpenGL

A simple 2D Basketball Shooting Game built with OpenGL, GLFW, and GLAD in C++.

The player can move left/right and shoot a basketball toward the basket.

The game includes gravity-based ball physics and a live score counter.


📸 Game Features

🎮 Player movement using keyboard

🏀 Basketball shooting system

🌍 Simple gravity physics

🎯 Score detection when ball enters hoop

🧍 Stickman player design

🪟 Dynamic window title showing score

⚡ Real-time rendering using OpenGL

🛠 Technologies Used

C++

OpenGL 3.3

GLFW

GLAD

🎹 Controls

| Key     | Action     |
| ------- | ---------- |
| `A`     | Move Left  |
| `D`     | Move Right |
| `SPACE` | Shoot Ball |
| `ESC`   | Exit Game  |


🧠 Game Logic

Player

The player can move horizontally.

Position is limited within screen bounds.

Ball Physics

Ball follows projectile motion.

Gravity decreases vertical velocity over time.

Scoring

Score increases when the ball reaches the basket ring area.



📂 Project Structure

BasketballGame/
│
├── main.cpp
├── glad.c
├── glad.h
├── glfw3.h
├── README.md



⚙️ Requirements

Before running the project, make sure you have:


OpenGL

GLFW

GLAD

C++ Compiler (G++ / MinGW / MSVC)




🚀 How to Run

1️⃣ Clone Repository

git clone https://github.com/your-username/basketball-game-opengl.git
