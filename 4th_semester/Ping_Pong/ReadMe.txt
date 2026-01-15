2D Pong Game
This 2D Pong Game, developed using the Simple2D library in C, provides a classic arcade experience with modern enhancements.

Features
Player Movement:

The left paddle is controlled using the "W" (move up) and "S" (move down) keys.

The right paddle is controlled using the "Up" (move up) and "Down" (move down) arrow keys.

Ball Movement: The ball bounces off the top and bottom boundaries, as well as off the paddles, with its direction reversed upon collision.

Dynamic Ball Speed: The ball's speed increases by 1 unit after every 3 successful paddle collisions, making the game progressively more challenging within a round. The speed resets at the start of each new round.

Paddle and Ball Colors: The paddles are white, and the ball is yellow, providing clear visual distinction.

Center Line: A white line divides the screen into two equal parts, enhancing the visual structure and making the game easier to follow.

Game Boundaries: Upper and lower off-white boundary lines are present, which the ball and paddles cannot cross, defining the playable area.

Scoring System: The game includes a scoring system, and the first player to reach a score of 5 wins the round.

Audio Feedback:

A "game start" sound plays when a new game begins.

A "pop" sound plays when the ball collides with a paddle.

A "ping" sound plays when a ball is missed and a point is scored.

A "game over" sound plays when a player wins.

Game States & UI:

Start Screen: Displays a "Start" button to begin the game.

Playing State: The main game loop where paddles and ball move. A pause button is visible.

Paused State: The game pauses, and a semi-transparent overlay appears with "Resume", "Home", and "Exit" buttons. The pause button disappears when paused.

Game Over Screen: Displays the winner (Player 1 or Player 2), a "Game Over" image, and "Retry" and "Exit" buttons. The pause button disappears on this screen.

How to Play
Start the Game: Click the "Start" button on the initial screen.

Control Paddles:

Left Player: Use 'W' to move up, 'S' to move down.

Right Player: Use 'Up Arrow' to move up, 'Down Arrow' to move down.

Score Points: Hit the ball past your opponent's paddle to score.

Win the Game: Be the first player to reach 5 points.

Pause: Click the small pause button in the top-left corner during gameplay to pause.

Resume/Restart/Exit: Use the buttons on the pause or game over screens to continue, return to the home screen, or exit the game.

