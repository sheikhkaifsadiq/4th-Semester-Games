Duck Hunt Game
2D Duck Hunt Game - Sprint 4
Developed using the Simple2D library in C.

Project Description
This classic-style 2D Duck Hunt game, developed using the Simple2D library in C, has progressed through several development sprints. Sprint 4 introduces dynamic round progression based on percentage-based duck kills, a dedicated game won screen with a visual win.png image, and refined global declarations for UI element sizes. The game now offers a more challenging and visually complete experience with its core mechanics, state management, and immersive sound effects.

Key Features
Player Interaction: Aim and shoot at flying ducks using mouse clicks.

Duck Behavior: Ducks move randomly across the screen with varying speeds and direction changes. Their speed and direction change frequency increase with each round, adding challenge.

Duck States: Ducks can be flying, hit (briefly show a "kill" frame), falling, or flying away (if missed too many times or time runs out).

Multi-Round Gameplay: Progress through multiple rounds, each with increasing difficulty.

Dynamic Round Progression: To clear a round, players must hit a minimum percentage of ducks out of the total ducks spawned in that round:

Round 1: At least 40% of ducks

Round 2: At least 50% of ducks

Round 3: At least 60% of ducks

Round 4: At least 70% of ducks

Round 5: At least 80% of ducks

Lives System: Players have a limited number of lives (represented by hearts). A life is lost each time a duck escapes (flies away). The game ends if all lives are lost. Lives reset at the beginning of each new round.

Game States: The game transitions through distinct states: Start Screen, Playing, Paused, Round Clear, Game Over, and Game Won.

User Interface (UI):

Pause Button: Allows players to pause the game at any time during gameplay.

Lives Display: Visual heart icons indicate remaining player lives.

Round Display: Shows the current round number during gameplay.

"FLY AWAY" Feedback: A fading text message appears when a duck escapes, indicating a lost life.

"ROUND CLEAR" Screen: Displays after completing a round, showing the number of ducks hit for that round.

Game Over Screen: Displays a gameover.png image and provides options to retry or exit.

Game Won Screen: Displays a win.png image when all rounds are successfully completed, offering options to play again or exit.

Sound Effects & Music:

Background Music: Different tracks for the start screen and active gameplay.

Shotgun Sounds: Distinct sounds for firing and reloading.

Duck Sounds: Wings flapping when ducks fly away.

Falling Sound: A distinct sound when a duck is shot and begins to fall.

Game Over Sound: Plays when the game ends.

Round Clear Sound: Plays upon successful completion of a round.

Game Won Sound: Plays when the player wins the entire game.

Visual Feedback:

A brief pink shade overlay appears on the screen when a duck is successfully hit.

The background briefly changes when a shot misses all ducks.

Button Functionality: Interactive buttons for starting the game, pausing/resuming, returning home, retrying a round/game, and exiting the application.

Custom Fonts: Utilizes "WolfalconRegular-RpjW3.ttf" for various text displays, enhancing the game's aesthetic.

Future Features
Implement a high score persistence system (e.g., saving to a file).

Introduce different types of ducks with varying behaviors or point values.

Add power-ups (e.g., extra shots, temporary invulnerability).

More diverse backgrounds and environmental elements.

Two-player mode or competitive scoring.

File Hierarchy
duck_hunt_game/
│
├── assets/
│   ├── right1.png               # Duck flying right (frame 1)
│   ├── right2.png               # Duck flying right (frame 2)
│   ├── right3.png               # Duck flying right (frame 3)
│   ├── upright1.png             # Duck flying up-right (frame 1)
│   ├── upright2.png             # Duck flying up-right (frame 2)
│   ├── upright3.png             # Duck flying up-right (frame 3)
│   ├── up1.png                  # Duck flying up (frame 1)
│   ├── up2.png                  # Duck flying up (frame 2)
│   ├── up3.png                  # Duck flying up (frame 3)
│   ├── left1.png                # Duck flying left (frame 1)
│   ├── left2.png                # Duck flying left (frame 2)
│   ├── left3.png                # Duck flying left (frame 3)
│   ├── downleft1.png            # Duck flying down-left (frame 1)
│   ├── downleft2.png            # Duck flying down-left (frame 2)
│   ├── downleft3.png            # Duck flying down-left (frame 3)
│   ├── downright1.png           # Duck flying down-right (frame 1)
│   ├── downright2.png           # Duck flying down-right (frame 2)
│   ├── downright3.png           # Duck flying down-right (frame 3)
│   ├── down1.png                # Duck flying down (frame 1)
│   ├── down2.png                # Duck flying down (frame 2)
│   ├── down3.png                # Duck flying down (frame 3)
│   ├── upleft1.png              # Duck flying up-left (frame 1)
│   ├── upleft2.png              # Duck flying up-left (frame 2)
│   ├── upleft3.png              # Duck flying up-left (frame 3)
│   ├── kill.png                 # Duck hit/killed frame
│   ├── falling1.png             # Duck falling (frame 1)
│   ├── falling2.png             # Duck falling (frame 2)
│   ├── background.png           # Main game background
│   ├── background2.png          # Background for missed shot feedback
│   ├── background3.png          # Background for game over screen
│   ├── start.png                # Start button image
│   ├── pause.png                # Pause button image
│   ├── resume.png               # Resume button image
│   ├── home.png                 # Home button image
│   ├── exit2.png                # Exit button image (from pause menu)
│   ├── gameover.png             # Game over screen image
│   ├── retry.png                # Retry button image
│   ├── exit.png                 # Exit button image (from game over/win screen)
│   ├── win.png                  # Game won screen image
│   ├── life.png                 # Player life icon (heart)
│   ├── WolfalconRegular-RpjW3.ttf # Custom font file
│   ├── banjo.mp3                # Start screen music
│   ├── background.mp3           # In-game background music
│   ├── game_over.ogg            # Game over sound effect
│   ├── drop_fall.mp3            # Duck falling sound effect
│   ├── shotgun_shot.mp3         # Shotgun firing sound effect
│   ├── shotgun_reload.mp3       # Shotgun reloading sound effect
│   ├── success.mp3              # Game won sound effect
│   ├── wings_flap.mp3           # Duck wings flapping sound effect
│   └── man_grinning.mp3         # Round clear sound effect
│
├── src/
│   └── main.c                   # Main game logic
│
└── README.md                    # Project description and setup guide (this file)

Sprint History
Sprint 1: Focused on implementing basic duck movement, animation cycles (flying, killed, falling), and the core shooting mechanic. Established the initial game loop and rendering.

Sprint 2: Introduced the multi-round structure, player lives system, and basic UI elements like the pause button and round display. Implemented the game over state.

Sprint 3: Integrated various sound effects and background music for a more immersive audio experience. Enhanced the UI with a comprehensive pause menu and "FLY AWAY" text feedback.

Sprint 4: Implemented dynamic round progression based on percentage-based duck kills. Added a dedicated "Game Won" screen with a win.png image. Globalized the size declarations for all UI elements for better maintainability and consistency.

Setup and How to Run
To compile and run this game, you will need the Simple2D library installed and configured on your system.

Prerequisites:
A C compiler (e.g., GCC).

The Simple2D library (ensure it's correctly installed and linked). You can find installation instructions on the Simple2D GitHub page or documentation.

Clone the Repository (or download files):
git clone <your-repository-url>
cd duck_hunt_game

(If you downloaded the files, ensure the assets, src, and README.md are in the correct relative paths as shown in the File Hierarchy.)

Compile the Game:
Navigate to the duck_hunt_game directory and compile main.c using the Simple2D build command. This command is suitable for environments like MSYS2 MinGW64:

simple2d build src/main.c

Run the Game:
After successful compilation, run the executable:

./main.exe

Enjoy the game!