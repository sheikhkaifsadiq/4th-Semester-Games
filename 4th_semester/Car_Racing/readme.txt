2D Racing Game - Sprint 4
Developed using the Simple2D library in C.

Project Description
This 2D racing game, developed using the Simple2D library in C, has evolved through several sprints. Sprint 4 represents a significant milestone, integrating comprehensive game mechanics, a robust scoring system, detailed game state management, and the addition of crucial sound effects and interactive power-ups. The game now offers a more complete and engaging experience.

Key Features
Player Movement: Control your car using the left and right arrow keys, with movement constrained within the road boundaries.

Background Scrolling: The road background scrolls vertically, creating a dynamic illusion of continuous driving.

Player and Enemy Cars: Features a player-controlled car and various enemy vehicles. Player car size dynamically adjusts during speed boosts.

Enemies: Enemy cars spawn randomly in different lanes, moving downwards to challenge the player. Their speed increases as the game progresses.

Obstacles: Various obstacles appear randomly in lanes, moving downwards to create additional challenges.

Collision Detection: The game accurately detects collisions between the player's car and enemy cars or obstacles.

Invulnerability System:

Heart Power-up: Grants an extra life. When this extra life is used after a collision, the player becomes invulnerable and blinks for 5 seconds, allowing them to recover.

Speed Power-up: Picking up speed.png increases the player's car size by 1.5 times and boosts the overall game speed by 1.5 times for 10 seconds. During this entire 10-second duration, the player is invulnerable to collisions and blinks. The blinking effect continues for an additional 5 seconds after the speed boost ends, providing a total of 15 seconds of visual invulnerability feedback.

Game Over: The game concludes upon collision (if no extra life is available), followed by a 3-second delay before transitioning to the Game Over screen.

Game Reset: Players can easily restart the game from the Game Over screen or return to the main menu from the Pause Menu.

Lanes: The road is clearly divided into four distinct lanes, which are utilized for strategic enemy and obstacle spawning.

Scoring System: Tracks the player's current score during gameplay, with bonus points awarded for collecting power-ups.

Game Timer: Displays the elapsed time during each game session, adding another layer to tracking performance.

Improved Game Over Screen: A dedicated "finalscore.png" image is displayed, prominently showing the current game's score and the all-time highest score.

High Score Persistence: The highest score achieved is automatically saved to and loaded from a score.txt file, ensuring your achievements are persistent across game sessions.

Custom Fonts: The game utilizes "Debrosee-ALPnL.ttf" for the "SCORE" label and "Game Over" text, and "Carre-JWja.ttf" for all numerical displays (score value, timer, and high score display), enhancing the visual theme.

Pause Menu: A robust pause menu allows players to temporarily halt the game, resume play, navigate back to the main menu, or exit the application.

Sound Effects & Music: Immersive audio feedback including background music, distinct crash sounds (car-to-car, car-to-obstacle), a game over sound, a "car passed" sound, heart power-up collection and usage sounds, and speed-up/slowing down effects.

Score Pop-up: A visual "+500" animation appears on screen when power-ups are collected, providing immediate feedback.

Future Features
Additional power-up types (e.g., shields, score multipliers).

More sophisticated enemy behaviors (e.g., enemies changing lanes, braking).

Further visual enhancements (e.g., particle effects for crashes, dynamic road textures).

Introduction of a level system or progression.

File Hierarchy
car_racing_game/
│
├── assets/
│   ├── player.png            # Player car image
│   ├── enemy1.png            # Enemy car 1 image
│   ├── enemy2.png            # Enemy car 2 image
│   ├── enemy3.png            # Enemy car 3 image
│   ├── obstacle.png          # Obstacle image 1
│   ├── obstacle2.png         # Obstacle image 2
│   ├── Carre-JWja.ttf        # Font file for digits and numerical displays
│   ├── Debrosee-ALPnL.ttf    # Custom font for "Score" label and "Game Over" text
│   ├── pause.png             # Pause button image
│   ├── resume.png            # Resume button image
│   ├── home.png              # Home button image
│   ├── exit2.png             # Exit button image (from pause menu)
│   ├── gameover.png          # Game over screen image
│   ├── retry.png             # Retry button image
│   ├── exit.png              # Exit button image (from game over screen)
│   ├── score.png             # In-game score display image background
│   ├── finalscore.png        # Game over final score display image background
│   ├── background.mp3        # Background music
│   ├── car_car_crash.mp3     # Sound effect for car-to-car collision
│   ├── car_obstacle_crash.mp3# Sound effect for car-to-obstacle collision
│   ├── gameover.wav          # Game over sound effect
│   ├── car_passed.mp3        # Sound effect for passing a car
│   ├── heart.wav             # Sound effect for picking up a heart power-up
│   ├── heart_used.wav        # Sound effect for using a heart (extra life)
│   ├── car_fasting.wav       # Sound effect for speed boost activation
│   ├── car_slowing.wav       # Sound effect for speed boost nearing end
│   ├── life.png              # Heart power-up image
│   ├── speed.png             # Speed power-up image
│   └── score.txt             # Stores the highest score (created/updated by the game)
│
├── src/
│   └── main.c                # Main game logic
│
└── README.md                 # Project description and setup guide (this file)

Sprint History
Sprint 1: Focused on basic character movement and the implementation of a vertically scrolling background to simulate driving.

Sprint 2: Introduced enemy cars and obstacles, alongside fundamental collision detection, leading to a basic game over state.

Sprint 3: Implemented a comprehensive scoring system, a game timer, an improved game over screen with current and high score displays, and persistence for the high score. A pause menu was also added.

Sprint 4: Integrated various sound effects and background music, implemented power-ups (life and speed) with distinct effects, invulnerability mechanics, and visual feedback (blinking, score pop-ups).

Setup and How to Run
To compile and run this game, you will need the Simple2D library installed and configured on your system.

Prerequisites:

A C compiler (e.g., GCC).

The Simple2D library (ensure it's correctly installed and linked). You can find installation instructions on the Simple2D GitHub page or documentation.

Clone the Repository (or download files):

git clone <your-repository-url>
cd car_racing_game

(If you downloaded the files, ensure the assets, src, and score.txt are in the correct relative paths as shown in the File Hierarchy.)

Compile the Game:
Navigate to the car_racing_game directory and compile main.c using the Simple2D build command. This command is suitable for environments like MSYS2 MinGW64:

simple2d build main.c

Run the Game:
After successful compilation, run the executable:

./main.exe

Enjoy the game!