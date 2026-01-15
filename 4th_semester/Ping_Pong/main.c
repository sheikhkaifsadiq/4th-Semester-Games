#include "simple2d.h"
#include <stdio.h>   // For sprintf
#include <stdbool.h> // For bool type
#include <stdlib.h>  // For rand, srand
#include <time.h>    // For time
#include <math.h>    // For fabs, cos, sin
#include <string.h>  // For strcmp

// Define M_PI if it's not available by default (often included with _GNU_SOURCE)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Global Game Constants ---
// Window dimensions
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600; // 16:9 aspect ratio

// Game states
typedef enum {
    GAME_STATE_START,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAME_OVER
} GameState;

GameState current_game_state = GAME_STATE_START;

// Button and Image sizes (globally declared using #define)
#define START_BUTTON_WIDTH      200
#define START_BUTTON_HEIGHT     80

#define PAUSE_BUTTON_WIDTH      50
#define PAUSE_BUTTON_HEIGHT     50

#define RESUME_BUTTON_WIDTH     200
#define RESUME_BUTTON_HEIGHT    80

#define HOME_BUTTON_WIDTH       200
#define HOME_BUTTON_HEIGHT      80

#define EXIT_BUTTON_WIDTH       200
#define EXIT_BUTTON_HEIGHT      80
#define EXIT2_BUTTON_WIDTH      200 // New: Size for exit2.png
#define EXIT2_BUTTON_HEIGHT     80  // New: Size for exit2.png

#define RETRY_BUTTON_WIDTH      200
#define RETRY_BUTTON_HEIGHT     80

#define GAMEOVER_IMAGE_WIDTH    300 // Adjusted from BUTTON_LARGE_WIDTH * 1.5
#define GAMEOVER_IMAGE_HEIGHT   120 // Adjusted from BUTTON_LARGE_HEIGHT * 1.5

#define WINNER_IMAGE_WIDTH      250 // Adjusted from BUTTON_LARGE_WIDTH * 1.2
#define WINNER_IMAGE_HEIGHT     100 // Adjusted from BUTTON_LARGE_HEIGHT * 1.2

// Game specific constants
#define INITIAL_BALL_SPEED      7.0f // Global ball speed

// Button spacing (globally declared using #define)
#define PAUSE_MENU_BUTTON_SPACING 20 // Vertical spacing between buttons in pause menu
#define GAMEOVER_MENU_ITEM_SPACING 20 // Vertical spacing between items in game over menu

// Boundary line constants
#define BOUNDARY_LINE_THICKNESS 4.0f
#define UPPER_BOUNDARY_Y_OFFSET 60.0f // Y-coordinate for the top boundary line
#define LOWER_BOUNDARY_Y_OFFSET 0.0f // Distance from bottom for the lower boundary line (changed to 0.0f)


// Asset paths (UPDATED TO USE LOCAL ASSETS FOLDER)
const char* ASSET_PATH_START = "assets/start.png";
const char* ASSET_PATH_PAUSE = "assets/pause.png";
const char* ASSET_PATH_RESUME = "assets/resume.png";
const char* ASSET_PATH_HOME = "assets/home.png";
const char* ASSET_PATH_EXIT = "assets/exit.png"; // Original exit button for game over
const char* ASSET_PATH_EXIT2 = "assets/exit2.png"; // New: Exit button for pause menu
const char* ASSET_PATH_RETRY = "assets/retry.png";
const char* ASSET_PATH_GAMEOVER = "assets/gameover.png";
const char* ASSET_PATH_PLAYER1_WINS = "assets/player1.png"; // Assuming player1.png for player 1 win
const char* ASSET_PATH_PLAYER2_WINS = "assets/player2.png"; // Assuming player2.png for player 2 win
const char* ASSET_PATH_FONT = "assets/OpenSans-Regular.ttf"; // Font file (used for S2D_CreateText)

// Audio asset paths
const char* AUDIO_PATH_GAMESTART = "assets/gamestart.mp3";
const char* AUDIO_PATH_POP = "assets/pop.mp3"; // Assuming pop.mp3 is a short sound effect
const char* AUDIO_PATH_PING = "assets/ping.wav";
const char* AUDIO_PATH_GAMEOVER = "assets/gameover.mp3";

// --- Game Assets ---
S2D_Image *img_start = NULL;
S2D_Image *img_pause = NULL;
S2D_Image *img_resume = NULL;
S2D_Image *img_home = NULL;
S2D_Image *img_exit = NULL; // Original exit button (used on game over screen)
S2D_Image *img_exit2 = NULL; // New: Exit button for pause menu
S2D_Image *img_retry = NULL;
S2D_Image *img_gameover = NULL;
S2D_Image *img_player1_wins = NULL;
S2D_Image *img_player2_wins = NULL;

// S2D_Text objects for scores
S2D_Text *player1_score_text = NULL;
S2D_Text *player2_score_text = NULL;

// Audio assets
S2D_Music *music_gamestart = NULL;
S2D_Sound *snd_pop = NULL;
S2D_Sound *snd_ping = NULL;
S2D_Music *music_gameover = NULL;

// Global window pointer to allow S2D_Close to be called from callbacks
S2D_Window *main_window = NULL;

// --- Game Variables ---
const int WINNING_SCORE = 5;

// Paddle properties
const float PADDLE_WIDTH = 15.0f;
float paddle_height; // Will be calculated based on window height
const float PADDLE_SPEED = 5.0f; // Pixels per frame

typedef struct {
    float x, y;
    float dy; // direction y
    int score;
    S2D_Color color;
} Paddle;

Paddle left_paddle;
Paddle right_paddle;

// Ball properties
const float BALL_RADIUS = 8.0f;
typedef struct {
    float x, y;
    float dx, dy; // velocity x, y
    float speed; // Current speed of the ball in the round
    S2D_Color color;
} Ball;

Ball game_ball;

// Ball speed increase variables (NOW BASED ON COLLISIONS)
const float SPEED_INCREASE_AMOUNT = 1.0f; // Increase speed by 1 unit
int paddle_collision_count = 0; // Counts total paddle collisions in a round
const int SPEED_INCREASE_COLLISIONS = 3; // Number of paddle collisions required to increase speed

Uint32 last_tick_time; // For delta time calculation

// --- Helper Functions ---

// Manual S2D_Color creation (if S2D_RGBA is not defined as a macro in simple2d.h)
// This function assumes S2D_Color is a struct with r, g, b, a members
S2D_Color create_s2d_color(float r, float g, float b, float a) {
    S2D_Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return color;
}


// Function to load all necessary images and font
void load_assets() {
    img_start = S2D_CreateImage(ASSET_PATH_START);
    img_pause = S2D_CreateImage(ASSET_PATH_PAUSE);
    img_resume = S2D_CreateImage(ASSET_PATH_RESUME);
    img_home = S2D_CreateImage(ASSET_PATH_HOME);
    img_exit = S2D_CreateImage(ASSET_PATH_EXIT);     // For game over screen
    img_exit2 = S2D_CreateImage(ASSET_PATH_EXIT2);   // For pause menu
    img_retry = S2D_CreateImage(ASSET_PATH_RETRY);
    img_gameover = S2D_CreateImage(ASSET_PATH_GAMEOVER);
    img_player1_wins = S2D_CreateImage(ASSET_PATH_PLAYER1_WINS);
    img_player2_wins = S2D_CreateImage(ASSET_PATH_PLAYER2_WINS);
    
    // Create S2D_Text objects for scores (font path, initial text, size)
    player1_score_text = S2D_CreateText(ASSET_PATH_FONT, "Player1: 0", 24);
    player2_score_text = S2D_CreateText(ASSET_PATH_FONT, "Player2: 0", 24);

    // Set colors for the S2D_Text objects after creation
    if (player1_score_text) {
        player1_score_text->color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f); // White
    }
    if (player2_score_text) {
        player2_score_text->color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f); // White
    }

    // Set image sizes for buttons using their specific defines
    if (img_start) { img_start->width = START_BUTTON_WIDTH; img_start->height = START_BUTTON_HEIGHT; }
    if (img_pause) { img_pause->width = PAUSE_BUTTON_WIDTH; img_pause->height = PAUSE_BUTTON_HEIGHT; }
    if (img_resume) { img_resume->width = RESUME_BUTTON_WIDTH; img_resume->height = RESUME_BUTTON_HEIGHT; }
    if (img_home) { img_home->width = HOME_BUTTON_WIDTH; img_home->height = HOME_BUTTON_HEIGHT; }
    if (img_exit) { img_exit->width = EXIT_BUTTON_WIDTH; img_exit->height = EXIT_BUTTON_HEIGHT; }
    if (img_exit2) { img_exit2->width = EXIT2_BUTTON_WIDTH; img_exit2->height = EXIT2_BUTTON_HEIGHT; } // New: Set size for exit2.png
    if (img_retry) { img_retry->width = RETRY_BUTTON_WIDTH; img_retry->height = RETRY_BUTTON_HEIGHT; }
    
    // Set sizes for game over specific images
    if (img_gameover) { img_gameover->width = GAMEOVER_IMAGE_WIDTH; img_gameover->height = GAMEOVER_IMAGE_HEIGHT; }
    if (img_player1_wins) { img_player1_wins->width = WINNER_IMAGE_WIDTH; img_player1_wins->height = WINNER_IMAGE_HEIGHT; }
    if (img_player2_wins) { img_player2_wins->width = WINNER_IMAGE_WIDTH; img_player2_wins->height = WINNER_IMAGE_HEIGHT; }

    // Set initial positions for score texts
    if (player1_score_text) {
        // S2D_Text objects have width/height properties after creation, used for centering
        player1_score_text->x = WINDOW_WIDTH / 4.0f - player1_score_text->width / 2.0f;
        player1_score_text->y = 20;
    }
    if (player2_score_text) {
        player2_score_text->x = WINDOW_WIDTH * 3.0f / 4.0f - player2_score_text->width / 2.0f;
        player2_score_text->y = 20;
    }

    // Load audio assets
    music_gamestart = S2D_CreateMusic(AUDIO_PATH_GAMESTART);
    snd_pop = S2D_CreateSound(AUDIO_PATH_POP);
    snd_ping = S2D_CreateSound(AUDIO_PATH_PING);
    music_gameover = S2D_CreateMusic(AUDIO_PATH_GAMEOVER);
}

// Function to free all loaded images and text objects
void free_assets() {
    if (img_start) S2D_FreeImage(img_start);
    if (img_pause) S2D_FreeImage(img_pause);
    if (img_resume) S2D_FreeImage(img_resume);
    if (img_home) S2D_FreeImage(img_home);
    if (img_exit) S2D_FreeImage(img_exit);
    if (img_exit2) S2D_FreeImage(img_exit2);
    if (img_retry) S2D_FreeImage(img_retry);
    if (img_gameover) S2D_FreeImage(img_gameover);
    if (img_player1_wins) S2D_FreeImage(img_player1_wins);
    if (img_player2_wins) S2D_FreeImage(img_player2_wins);
    
    // Free S2D_Text objects
    if (player1_score_text) S2D_FreeText(player1_score_text);
    if (player2_score_text) S2D_FreeText(player2_score_text);

    // Free audio assets
    if (music_gamestart) S2D_FreeMusic(music_gamestart);
    if (snd_pop) S2D_FreeSound(snd_pop);
    if (snd_ping) S2D_FreeSound(snd_ping);
    if (music_gameover) S2D_FreeMusic(music_gameover);
}

// Resets ball position and gives it a random initial direction
void reset_ball() {
    game_ball.x = WINDOW_WIDTH / 2.0f;
    game_ball.y = WINDOW_HEIGHT / 2.0f;
    // game_ball.speed is now managed by speed_increase_timer, so it's not reset here
    
    // Random initial horizontal direction
    game_ball.dx = (rand() % 2 == 0 ? 1 : -1) * game_ball.speed;
    
    // Random initial vertical direction (smaller component to start)
    game_ball.dy = (rand() % 2 == 0 ? 1 : -1) * (float)(rand() % (int)(game_ball.speed / 2) + 1);

    // Ensure ball has some vertical movement
    if (fabs(game_ball.dy) < 1.0f) {
        game_ball.dy = (game_ball.dy >= 0 ? 1.0f : -1.0f) * 1.0f;
    }
}

// Resets game variables to initial state for a new round
void reset_game() {
    left_paddle.score = 0;
    right_paddle.score = 0;

    // Reset ball speed and collision count for the new round
    game_ball.speed = INITIAL_BALL_SPEED;
    paddle_collision_count = 0; // Reset collision counter

    // Update text objects with new scores (free old, create new)
    if (player1_score_text) S2D_FreeText(player1_score_text);
    player1_score_text = S2D_CreateText(ASSET_PATH_FONT, "Player1: 0", 24);
    if (player1_score_text) {
        player1_score_text->color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f);
        player1_score_text->x = WINDOW_WIDTH / 4.0f - player1_score_text->width / 2.0f;
        player1_score_text->y = 20;
    }

    if (player2_score_text) S2D_FreeText(player2_score_text);
    player2_score_text = S2D_CreateText(ASSET_PATH_FONT, "Player2: 0", 24);
    if (player2_score_text) {
        player2_score_text->color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f);
        player2_score_text->x = WINDOW_WIDTH * 3.0f / 4.0f - player2_score_text->width / 2.0f;
        player2_score_text->y = 20;
    }

    left_paddle.y = (WINDOW_HEIGHT - paddle_height) / 2.0f;
    right_paddle.y = (WINDOW_HEIGHT - paddle_height) / 2.0f;

    left_paddle.dy = 0;
    right_paddle.dy = 0;

    reset_ball();
}

// --- Drawing Functions ---

void draw_game_elements() {
    // Draw the middle line (using S2D_DrawQuad)
    S2D_DrawQuad(
        WINDOW_WIDTH / 2.0f - 2, 0, 1.0f, 1.0f, 1.0f, 1.0f, // Top-left (x1, y1, r1, g1, b1, a1)
        WINDOW_WIDTH / 2.0f - 2 + 4, 0, 1.0f, 1.0f, 1.0f, 1.0f, // Top-right (x2, y2, r2, g2, b2, a2)
        WINDOW_WIDTH / 2.0f - 2 + 4, WINDOW_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, // Bottom-right (x3, y3, r3, g3, b3, a3)
        WINDOW_WIDTH / 2.0f - 2, WINDOW_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f  // Bottom-left (x4, y4, r4, g4, b4, a4)
    );

    // Define off-white color for boundaries
    S2D_Color boundary_color = create_s2d_color(0.9f, 0.9f, 0.9f, 1.0f);

    // Draw upper boundary line
    S2D_DrawQuad(
        0, UPPER_BOUNDARY_Y_OFFSET, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a,
        WINDOW_WIDTH, UPPER_BOUNDARY_Y_OFFSET, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a,
        WINDOW_WIDTH, UPPER_BOUNDARY_Y_OFFSET + BOUNDARY_LINE_THICKNESS, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a,
        0, UPPER_BOUNDARY_Y_OFFSET + BOUNDARY_LINE_THICKNESS, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a
    );

    // Draw lower boundary line
    S2D_DrawQuad(
        0, WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET - BOUNDARY_LINE_THICKNESS, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a,
        WINDOW_WIDTH, WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET - BOUNDARY_LINE_THICKNESS, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a,
        WINDOW_WIDTH, WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a,
        0, WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET, boundary_color.r, boundary_color.g, boundary_color.b, boundary_color.a
    );

    // Draw paddles (using S2D_DrawQuad)
    S2D_DrawQuad(
        left_paddle.x, left_paddle.y, left_paddle.color.r, left_paddle.color.g, left_paddle.color.b, left_paddle.color.a,
        left_paddle.x + PADDLE_WIDTH, left_paddle.y, left_paddle.color.r, left_paddle.color.g, left_paddle.color.b, left_paddle.color.a,
        left_paddle.x + PADDLE_WIDTH, left_paddle.y + paddle_height, left_paddle.color.r, left_paddle.color.g, left_paddle.color.b, left_paddle.color.a,
        left_paddle.x, left_paddle.y + paddle_height, left_paddle.color.r, left_paddle.color.g, left_paddle.color.b, left_paddle.color.a
    );

    S2D_DrawQuad(
        right_paddle.x, right_paddle.y, right_paddle.color.r, right_paddle.color.g, right_paddle.color.b, right_paddle.color.a,
        right_paddle.x + PADDLE_WIDTH, right_paddle.y, right_paddle.color.r, right_paddle.color.g, right_paddle.color.b, right_paddle.color.a,
        right_paddle.x + PADDLE_WIDTH, right_paddle.y + paddle_height, right_paddle.color.r, right_paddle.color.g, right_paddle.color.b, right_paddle.color.a,
        right_paddle.x, right_paddle.y + paddle_height, right_paddle.color.r, right_paddle.color.g, right_paddle.color.b, right_paddle.color.a
    );

    // Draw ball
    S2D_DrawCircle(game_ball.x, game_ball.y, BALL_RADIUS, 16,
                   game_ball.color.r, game_ball.color.g, game_ball.color.b, game_ball.color.a);

    // Draw scores using the S2D_Text objects
    if (player1_score_text) S2D_DrawText(player1_score_text);
    if (player2_score_text) S2D_DrawText(player2_score_text);
}

// --- Game Logic Functions ---

void update_paddle_positions() {
    // Left paddle movement
    left_paddle.y += left_paddle.dy;
    // Clamp paddle to new upper and lower boundary lines
    if (left_paddle.y < UPPER_BOUNDARY_Y_OFFSET) {
        left_paddle.y = UPPER_BOUNDARY_Y_OFFSET;
    }
    if (left_paddle.y + paddle_height > WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET) {
        left_paddle.y = WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET - paddle_height;
    }

    // Right paddle movement
    right_paddle.y += right_paddle.dy;
    // Clamp paddle to new upper and lower boundary lines
    if (right_paddle.y < UPPER_BOUNDARY_Y_OFFSET) {
        right_paddle.y = UPPER_BOUNDARY_Y_OFFSET;
    }
    if (right_paddle.y + paddle_height > WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET) {
        right_paddle.y = WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET - paddle_height;
    }
}

void update_ball_position() {
    game_ball.x += game_ball.dx;
    game_ball.y += game_ball.dy;

    // Ball collision with upper boundary line
    if (game_ball.y - BALL_RADIUS < UPPER_BOUNDARY_Y_OFFSET) {
        game_ball.dy *= -1; // Reverse vertical direction
        game_ball.y = UPPER_BOUNDARY_Y_OFFSET + BALL_RADIUS; // Adjust position to prevent sticking
    }
    // Ball collision with lower boundary line
    if (game_ball.y + BALL_RADIUS > WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET) {
        game_ball.dy *= -1; // Reverse vertical direction
        game_ball.y = WINDOW_HEIGHT - LOWER_BOUNDARY_Y_OFFSET - BALL_RADIUS; // Adjust position to prevent sticking
    }

    // Ball collision with left paddle (non-curvy surface)
    if (game_ball.x - BALL_RADIUS < left_paddle.x + PADDLE_WIDTH &&
        game_ball.y + BALL_RADIUS > left_paddle.y &&
        game_ball.y - BALL_RADIUS < left_paddle.y + paddle_height &&
        game_ball.dx < 0 // Only check if ball is moving towards the paddle
    ) {
        game_ball.dx *= -1; // Reverse horizontal direction
        game_ball.x = left_paddle.x + PADDLE_WIDTH + BALL_RADIUS; // Move ball out to prevent sticking
        
        // Play pop sound
        if (snd_pop) S2D_PlaySound(snd_pop);

        // Increase collision count and check for speed increase
        paddle_collision_count++;
        if (paddle_collision_count >= SPEED_INCREASE_COLLISIONS) {
            game_ball.speed += SPEED_INCREASE_AMOUNT;
            paddle_collision_count = 0; // Reset counter for next speed increase
            
            // Re-normalize dx/dy to new speed while maintaining current direction
            float current_magnitude = sqrt(game_ball.dx * game_ball.dx + game_ball.dy * game_ball.dy);
            if (current_magnitude > 0) {
                game_ball.dx = (game_ball.dx / current_magnitude) * game_ball.speed;
                game_ball.dy = (game_ball.dy / current_magnitude) * game_ball.speed;
            }
        }
    }

    // Ball collision with right paddle (non-curvy surface)
    if (game_ball.x + BALL_RADIUS > right_paddle.x &&
        game_ball.y + BALL_RADIUS > right_paddle.y &&
        game_ball.y - BALL_RADIUS < right_paddle.y + paddle_height &&
        game_ball.dx > 0 // Only check if ball is moving towards the paddle
    ) {
        game_ball.dx *= -1; // Reverse horizontal direction
        game_ball.x = right_paddle.x - BALL_RADIUS; // Move ball out to prevent sticking

        // Play pop sound
        if (snd_pop) S2D_PlaySound(snd_pop);

        // Increase collision count and check for speed increase
        paddle_collision_count++;
        if (paddle_collision_count >= SPEED_INCREASE_COLLISIONS) {
            game_ball.speed += SPEED_INCREASE_AMOUNT;
            paddle_collision_count = 0; // Reset counter for next speed increase
            
            // Re-normalize dx/dy to new speed while maintaining current direction
            float current_magnitude = sqrt(game_ball.dx * game_ball.dx + game_ball.dy * game_ball.dy);
            if (current_magnitude > 0) {
                game_ball.dx = (game_ball.dx / current_magnitude) * game_ball.speed;
                game_ball.dy = (game_ball.dy / current_magnitude) * game_ball.speed;
            }
        }
    }

    // Ball goes out of bounds (scoring)
    if (game_ball.x - BALL_RADIUS < 0) { // Right player scores
        right_paddle.score++;
        char score_text[20];
        sprintf(score_text, "Player2: %d", right_paddle.score);
        // Free old text object and create new one for update
        if (player2_score_text) S2D_FreeText(player2_score_text);
        player2_score_text = S2D_CreateText(ASSET_PATH_FONT, score_text, 24);
        if (player2_score_text) {
            player2_score_text->color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f);
            player2_score_text->x = WINDOW_WIDTH * 3.0f / 4.0f - player2_score_text->width / 2.0f;
            player2_score_text->y = 20;
        }
        
        // Play ping sound
        if (snd_ping) S2D_PlaySound(snd_ping);

        if (right_paddle.score >= WINNING_SCORE) {
            current_game_state = GAME_STATE_GAME_OVER;
            if (music_gameover) S2D_PlayMusic(music_gameover, false); // Play game over music once
        } else {
            reset_ball();
        }
    } else if (game_ball.x + BALL_RADIUS > WINDOW_WIDTH) { // Left player scores
        left_paddle.score++;
        char score_text[20];
        sprintf(score_text, "Player1: %d", left_paddle.score);
        // Free old text object and create new one for update
        if (player1_score_text) S2D_FreeText(player1_score_text);
        player1_score_text = S2D_CreateText(ASSET_PATH_FONT, score_text, 24);
        if (player1_score_text) {
            player1_score_text->color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f);
            player1_score_text->x = WINDOW_WIDTH / 4.0f - player1_score_text->width / 2.0f;
            player1_score_text->y = 20;
        }

        // Play ping sound
        if (snd_ping) S2D_PlaySound(snd_ping);

        if (left_paddle.score >= WINNING_SCORE) {
            current_game_state = GAME_STATE_GAME_OVER;
            if (music_gameover) S2D_PlayMusic(music_gameover, false); // Play game over music once
        } else {
            reset_ball();
        }
    }
}

// --- Simple2D Callbacks ---

void S2D_OnUpdate() {
    Uint32 current_tick_time = SDL_GetTicks();
    // Calculate delta_time in seconds
    float delta_time = (current_tick_time - last_tick_time) / 1000.0f;
    last_tick_time = current_tick_time;

    if (current_game_state == GAME_STATE_PLAYING) {
        update_paddle_positions();
        update_ball_position();
    }
}

void S2D_OnDraw() {
    // Use S2D_GL_Clear with an S2D_Color struct
    S2D_GL_Clear(create_s2d_color(0.17f, 0.32f, 0.24f, 1.0f)); // Dark green background for the field

    if (current_game_state == GAME_STATE_PLAYING ||
        current_game_state == GAME_STATE_PAUSED ||
        current_game_state == GAME_STATE_GAME_OVER) {
        draw_game_elements();
    }

    // Draw UI overlays based on game state
    if (current_game_state == GAME_STATE_START) {
        if (img_start) {
            img_start->x = (WINDOW_WIDTH - img_start->width) / 2.0f;
            img_start->y = (WINDOW_HEIGHT - img_start->height) / 2.0f;
            S2D_DrawImage(img_start);
        }
    } else if (current_game_state == GAME_STATE_PAUSED) {
        // Draw semi-transparent overlay using S2D_DrawQuad
        S2D_DrawQuad(
            0, 0, 0.0f, 0.0f, 0.0f, 0.7f,
            WINDOW_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.7f,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f,
            0, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f
        );

        // Calculate total height of the button block for centering
        float total_pause_block_height = RESUME_BUTTON_HEIGHT + HOME_BUTTON_HEIGHT + EXIT2_BUTTON_HEIGHT + (2 * PAUSE_MENU_BUTTON_SPACING);
        float current_y = (WINDOW_HEIGHT - total_pause_block_height) / 2.0f;

        // Draw Resume button
        if (img_resume) {
            img_resume->x = (WINDOW_WIDTH - img_resume->width) / 2.0f;
            img_resume->y = current_y;
            S2D_DrawImage(img_resume);
            current_y += RESUME_BUTTON_HEIGHT + PAUSE_MENU_BUTTON_SPACING;
        }
        // Draw Home button
        if (img_home) {
            img_home->x = (WINDOW_WIDTH - img_home->width) / 2.0f;
            img_home->y = current_y;
            S2D_DrawImage(img_home);
            current_y += HOME_BUTTON_HEIGHT + PAUSE_MENU_BUTTON_SPACING;
        }
        // Draw Exit2 button
        if (img_exit2) { // Use img_exit2 for pause menu
            img_exit2->x = (WINDOW_WIDTH - img_exit2->width) / 2.0f;
            img_exit2->y = current_y;
            S2D_DrawImage(img_exit2);
        }
    } else if (current_game_state == GAME_STATE_GAME_OVER) {
        // Draw semi-transparent overlay using S2D_DrawQuad
        S2D_DrawQuad(
            0, 0, 0.0f, 0.0f, 0.0f, 0.7f,
            WINDOW_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.7f,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f,
            0, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f
        );

        // Calculate total height of the game over block for centering
        float total_gameover_block_height = WINNER_IMAGE_HEIGHT + GAMEOVER_IMAGE_HEIGHT + RETRY_BUTTON_HEIGHT + EXIT_BUTTON_HEIGHT + (3 * GAMEOVER_MENU_ITEM_SPACING);
        float current_y = (WINDOW_HEIGHT - total_gameover_block_height) / 2.0f;

        // Determine and draw winner image (no tie.png)
        S2D_Image *winner_img = NULL;
        if (left_paddle.score > right_paddle.score) {
            winner_img = img_player1_wins;
        } else { // If scores are equal or right paddle wins, assume player 2 wins or it's a default win for P2
            winner_img = img_player2_wins;
        }
        if (winner_img) {
            winner_img->x = (WINDOW_WIDTH - winner_img->width) / 2.0f;
            winner_img->y = current_y;
            S2D_DrawImage(winner_img);
            current_y += WINNER_IMAGE_HEIGHT + GAMEOVER_MENU_ITEM_SPACING;
        }

        // Draw Game Over image
        if (img_gameover) {
            img_gameover->x = (WINDOW_WIDTH - img_gameover->width) / 2.0f;
            img_gameover->y = current_y;
            S2D_DrawImage(img_gameover);
            current_y += GAMEOVER_IMAGE_HEIGHT + GAMEOVER_MENU_ITEM_SPACING;
        }

        // Draw Retry button
        if (img_retry) {
            img_retry->x = (WINDOW_WIDTH - RETRY_BUTTON_WIDTH) / 2.0f;
            img_retry->y = current_y;
            S2D_DrawImage(img_retry);
            current_y += RETRY_BUTTON_HEIGHT + GAMEOVER_MENU_ITEM_SPACING;
        }
        // Draw Exit button
        if (img_exit) { // Use img_exit for game over screen
            img_exit->x = (WINDOW_WIDTH - EXIT_BUTTON_WIDTH) / 2.0f;
            img_exit->y = current_y;
            S2D_DrawImage(img_exit);
        }
    }

    // Draw pause button only during playing state
    if (current_game_state == GAME_STATE_PLAYING && img_pause) {
        img_pause->x = 10;
        img_pause->y = 10;
        S2D_DrawImage(img_pause);
    }
}

void S2D_OnMouseEvent(S2D_Event e) {
    if (e.type == S2D_MOUSE_DOWN) {
        // Helper to check if mouse is over a button
        bool is_over_button(S2D_Image* btn_img, float mouse_x, float mouse_y) {
            if (!btn_img) return false;
            return (mouse_x >= btn_img->x && mouse_x <= btn_img->x + btn_img->width &&
                    mouse_y >= btn_img->y && mouse_y <= btn_img->y + btn_img->height);
        }

        if (current_game_state == GAME_STATE_START) {
            if (is_over_button(img_start, e.x, e.y)) {
                current_game_state = GAME_STATE_PLAYING;
                reset_game(); // Start a new game
                if (music_gamestart) S2D_PlayMusic(music_gamestart, false); // Play game start music once
            }
        } else if (current_game_state == GAME_STATE_PLAYING) {
            if (is_over_button(img_pause, e.x, e.y)) {
                current_game_state = GAME_STATE_PAUSED;
            }
        } else if (current_game_state == GAME_STATE_PAUSED) {
            if (is_over_button(img_resume, e.x, e.y)) {
                current_game_state = GAME_STATE_PLAYING;
            } else if (is_over_button(img_home, e.x, e.y)) {
                current_game_state = GAME_STATE_START;
            } else if (is_over_button(img_exit2, e.x, e.y)) { // Check for img_exit2
                if (main_window) S2D_Close(main_window); // Exit the game
            }
        } else if (current_game_state == GAME_STATE_GAME_OVER) {
            if (is_over_button(img_retry, e.x, e.y)) {
                current_game_state = GAME_STATE_PLAYING;
                reset_game(); // Start a new game
            } else if (is_over_button(img_exit, e.x, e.y)) { // Check for img_exit
                if (main_window) S2D_Close(main_window); // Exit the game
            }
        }
    }
}

void S2D_OnKeyEvent(S2D_Event e) {
    if (current_game_state == GAME_STATE_PLAYING) {
        if (e.type == S2D_KEY_DOWN) {
            if (strcmp(e.key, "W") == 0) left_paddle.dy = -PADDLE_SPEED;
            else if (strcmp(e.key, "S") == 0) left_paddle.dy = PADDLE_SPEED;
            else if (strcmp(e.key, "Up") == 0) right_paddle.dy = -PADDLE_SPEED;
            else if (strcmp(e.key, "Down") == 0) right_paddle.dy = PADDLE_SPEED;
        } else if (e.type == S2D_KEY_UP) {
            if (strcmp(e.key, "W") == 0 || strcmp(e.key, "S") == 0) left_paddle.dy = 0;
            else if (strcmp(e.key, "Up") == 0 || strcmp(e.key, "Down") == 0) right_paddle.dy = 0;
        }
    }
}

// --- Main Function ---

int main() {
    srand(time(NULL)); // Seed random number generator

    main_window = S2D_CreateWindow(
        "Complete Pong Game", WINDOW_WIDTH, WINDOW_HEIGHT,
        S2D_OnUpdate, S2D_OnDraw, // Pass function pointers directly
        0 // flags: 0 for windowed mode, S2D_FULLSCREEN for fullscreen
    );

    // Initialize game variables
    paddle_height = WINDOW_HEIGHT / 4.0f; // Paddle is 1/4th of screen height

    left_paddle.x = 20.0f;
    left_paddle.y = (WINDOW_HEIGHT - paddle_height) / 2.0f;
    left_paddle.score = 0;
    left_paddle.color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f); // White

    right_paddle.x = WINDOW_WIDTH - PADDLE_WIDTH - 20.0f;
    right_paddle.y = (WINDOW_HEIGHT - paddle_height) / 2.0f;
    right_paddle.score = 0;
    right_paddle.color = create_s2d_color(1.0f, 1.0f, 1.0f, 1.0f); // White

    game_ball.x = WINDOW_WIDTH / 2.0f;
    game_ball.y = WINDOW_HEIGHT / 2.0f;
    game_ball.speed = INITIAL_BALL_SPEED; // Set initial speed from global define
    game_ball.color = create_s2d_color(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

    load_assets(); // Load all images and font

    // Set initial last_tick_time for delta time calculation
    last_tick_time = SDL_GetTicks();

    // Set callback functions (already handled in S2D_CreateWindow, but good practice to confirm)
    if (main_window) {
        main_window->on_mouse = S2D_OnMouseEvent;
        main_window->on_key = S2D_OnKeyEvent;
    }

    if (main_window) {
        S2D_Show(main_window); // Start the Simple2D loop
    } else {
        fprintf(stderr, "Error: Could not create Simple2D window.\n");
        return 1;
    }

    free_assets(); // Free loaded assets when window closes
    if (main_window) {
        S2D_FreeWindow(main_window);
    }
    return 0;
}
