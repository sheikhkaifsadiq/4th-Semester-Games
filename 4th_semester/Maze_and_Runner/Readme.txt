Monster and the Maze Game
Project Description
"Monster and the Maze" is an engaging 2D maze game developed using the Simple2D library in C. Players navigate through a series of challenging mazes, aiming to reach the goal while strategically avoiding a relentless monster. The game integrates comprehensive game mechanics, dynamic monster AI, robust game state management, and interactive UI elements, providing a complete and immersive experience.

Key Features
Multiple Mazes & Level Progression: Play through a predefined sequence of mazes, advancing to the next level upon successfully reaching the goal.

Player Movement: Control your character fluidly using the arrow keys or WASD, with continuous movement within the maze boundaries.

Monster AI: A sophisticated monster AI actively pursues the player using a Breadth-First Search (BFS) pathfinding algorithm, creating a constant challenge.

Goal/Exit Point: Each maze features a designated 'G' (Goal) tile that the player must reach to complete the current level and progress.

Dynamic Graphics & Animation:

Player character features 8-frame animations that dynamically rotate based on movement direction.

Monster character includes 2-frame animations that also rotate according to its movement.

Custom image assets are used for maze walls, backgrounds, and the goal indicator.

Comprehensive Game State Management: The game transitions seamlessly between distinct states: START_SCREEN, PLAYING, PAUSED, GAME_OVER, and GAME_WON.

Interactive User Interface (UI): Includes responsive buttons for Start, Pause, Resume, Home, Retry, and Exit, alongside clear text labels for game status, maze progression, and current round.

Collision Detection: Accurate collision detection between the player's character and the monster results in a GAME_OVER state.

Dead End Detection: An intelligent system detects if the player is trapped in a dead end with the monster adjacent, leading to a GAME_OVER.

Sound Effects: Immersive audio feedback enhances gameplay with distinct sounds for game start/new rounds and game over events.

Round Tracking: Keeps track of the current round number, indicating player progression through the maze levels.

Assets
All image, font, sound, and maze definition files must be placed within an assets folder located in the root directory of the project.

Images
stand.png - Player character's standing sprite.

player1.png to player8.png - Player character animation sprites for various directions.

monster1.png, monster2.png - Monster character animation sprites.

wll.png - Image used for maze walls.

background.png - General background image for the game.

goal.png - Image representing the goal tile.

start.png - Image for the game's start button.

pause.png - Image for the in-game pause button.

resume.png - Image for the resume button in the pause menu.

home.png - Image for the home button (returns to start screen).

exit.png - Image for the exit button (primarily on game over screen).

exit2.png - Alternate exit button image (used on pause and game won screens).

gameover.png - Image displayed when the game ends.

retry.png - Image for the retry button on the game over screen.

win.png - Image displayed upon successfully completing all mazes.

Fonts
Debrosee-ALPnL.ttf - Custom font used for titles and general UI labels.

Carre-JWja.ttf - Custom font used for numerical displays (e.g., round numbers).

Sounds
gamestart.mp3 - Sound played at the beginning of a game or new round.

gameover.mp3 - Sound effect played when the game ends.

Maze Files
maze_1.txt, maze_2.txt, ..., maze_5.txt - Text files defining the layout of each maze. These files use characters like # for walls,   for paths, P for the player's starting position, M for the monster's starting position, and G for the goal.

Setup and How to Run
To compile and run this game, you will need the Simple2D library installed and a C compiler configured on your system.

Prerequisites
A C compiler (e.g., GCC).

The Simple2D library (ensure it's correctly installed and linked).

Installation Steps
Download or Clone: Obtain the game files by downloading or cloning the repository to your local machine.

Navigate to Project Directory: Open your terminal or command prompt and navigate to the directory where the game.c file is located.

Place Assets: Create a folder named assets in the root directory of the project, and place all the image, font, sound, and maze .txt files (as listed in the "Assets" section above) into this assets folder.

Compile the Game: Use the Simple2D build command to compile game.c.

simple2d build game.c

Run the Game: After successful compilation, execute the compiled program.

simple2d run game.c

Gameplay
Start Screen: Begin your adventure by clicking the START button.

Movement: Control your player character using the Arrow Keys (Up, Down, Left, Right) or WASD (W: Up, S: Down, A: Left, D: Right) to navigate the maze.

Objective: Your primary goal is to reach the Goal (G) tile in each maze. Successfully reaching it will advance you to the next challenging round.

Monster Avoidance: Evade the patrolling monster (M). Direct contact with the monster or being trapped in a dead end adjacent to it will result in GAME OVER.

Pause Menu: During gameplay, press the ESC key or click the PAUSE button to access the pause menu.

RESUME: Continue your game from where you left off.

HOME: Return to the main start screen.

EXIT: Close the game application.

Game Over Screen: If your journey ends in a collision or a trap, you'll be presented with the game over screen.

RETRY: Attempt the current maze again.

EXIT: Quit the game application.

Game Won Screen: Conquer all the mazes to achieve victory!

HOME: Return to the main start screen.

EXIT: Close the game application.

The game operates on a grid-based system, with all maze elements dynamically sized based on a CELL_SIZE constant.

Acknowledgements
Developed with the Simple2D library for graphics rendering.

Assets obtained from various creative commons sources.

Future Features
Implement a comprehensive scoring system to track player performance.

Introduce more diverse monster behaviors or additional monster types.

Add various power-ups or special abilities for the player.

Develop a level editor or a procedural maze generation system.

Enhance visual effects, such as particle systems or dynamic lighting.

Integrate background music and a wider range of sound effects for richer audio.