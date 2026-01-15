#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h> // For srand

#include "simple2d.h" // Provided header for Simple 2D library

// --- Game Constants ---
// Global Screen Dimensions
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

#define CELL_SIZE 30         // Base size for all grid elements (walls, player, monster)

#define MAZE_MAX_WIDTH (SCREEN_WIDTH / CELL_SIZE)    // Maximum width of any maze grid
#define MAZE_MAX_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)  // Maximum height of any maze grid

#define NUM_MAZES 5          // Total number of mazes to play through
#define MONSTER_MOVE_DELAY 15 // Monster moves every N frames (higher value = slower monster)
#define PLAYER_ANIMATION_SPEED 5 // Player animation frame changes every N update calls

// Re-added: Player movement speed
#define PLAYER_SPEED 3.0f // Adjust as needed for desired player movement speed

// --- Global Entity Sizes (derived from CELL_SIZE for grid alignment) ---
#define PLAYER_DRAW_SIZE (CELL_SIZE - 2)
#define MONSTER_DRAW_SIZE (CELL_SIZE - 2)
#define WALL_DRAW_SIZE CELL_SIZE
#define GOAL_DRAW_SIZE CELL_SIZE // Size for the goal sprite

// --- UI Button Dimensions and Positions ---
// Base dimensions, now individual buttons will have their own
#define BUTTON_MARGIN 20

// NEW: Specific spacing for pause menu buttons to prevent overlapping
#define PAUSE_MENU_BUTTON_SPACING 30

// Dimensions for win.png
#define WIN_IMAGE_WIDTH 400
#define WIN_IMAGE_HEIGHT 200

// Dimensions for gameover.png
#define GAMEOVER_IMAGE_WIDTH 400
#define GAMEOVER_IMAGE_HEIGHT 200

// NEW: Individual button dimensions
#define START_BUTTON_WIDTH_VAL 150
#define START_BUTTON_HEIGHT_VAL 70

#define PAUSE_BUTTON_WIDTH_VAL 60
#define PAUSE_BUTTON_HEIGHT_VAL 60

#define RESUME_BUTTON_WIDTH_VAL 120
#define RESUME_BUTTON_HEIGHT_VAL 60

#define HOME_BUTTON_WIDTH_VAL 120
#define HOME_BUTTON_HEIGHT_VAL 60

#define PAUSE_EXIT_BUTTON_WIDTH_VAL 120
#define PAUSE_EXIT_BUTTON_HEIGHT_VAL 60

#define RETRY_BUTTON_WIDTH_VAL 120
#define RETRY_BUTTON_HEIGHT_VAL 60

#define GAMEOVER_EXIT_BUTTON_WIDTH_VAL 120
#define GAMEOVER_EXIT_BUTTON_HEIGHT_VAL 60

#define GAMEWON_EXIT_BUTTON_WIDTH_VAL 120 // NEW: Separate width for game won exit button
#define GAMEWON_EXIT_BUTTON_HEIGHT_VAL 60 // NEW: Separate height for game won exit button

// --- Global Font Sizes ---
#define TITLE_FONT_SIZE 36
#define INSTRUCTION_FONT_SIZE 16
#define PAUSE_TEXT_SIZE 36
#define GAME_OVER_TEXT_SIZE 36
#define GAME_WON_TEXT_SIZE 36
#define MAZE_INFO_FONT_SIZE 26 // For "Maze X/Y" and "ROUND"


// --- Game States ---
typedef enum {
    START_SCREEN,
    PLAYING,
    PAUSED,
    GAME_OVER,
    GAME_WON
} GameState;

// --- Maze Structure ---
typedef struct {
    char grid[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH]; // 2D array representing the maze
    int width;                                  // Actual width of the current maze
    int height;                                 // Actual height of the current maze
    int player_start_x, player_start_y;         // Player's initial spawn coordinates
    int monster_start_x, monster_start_y;       // Monster's initial spawn coordinates
    int goal_x, goal_y;                         // Goal cell coordinates
} Maze;

// --- UI Button Bounding Box Structure ---
typedef struct {
    int x, y, w, h; // Position and dimensions of the button
} ButtonRect;


// --- Global Game Variables ---
S2D_Window *game_window;          // Pointer to the Simple 2D window
GameState current_game_state = START_SCREEN; // Initial game state

Maze current_maze;                // The currently loaded maze
int current_maze_index = 0;       // Index of the current maze (0 to NUM_MAZES-1)
int current_round = 1;            // NEW: Current round number

// Player variables
float player_x, player_y;         // Player's current position in maze grid coordinates (now float for smoother movement)
float prev_player_x, prev_y; // Player's previous position for movement detection
S2D_Image *player_stand_img = NULL; // Image for player when standing still (stand.png)
S2D_Image *player_sprites[8];     // Array of images for player animation (player1.png to player8.png)
int player_frame = 0;             // Current animation frame for player
int player_frame_timer = 0;       // Timer to control player animation speed
int player_direction_angle = 0;   // Rotation angle for player sprite (0: Up, 90: Right, 180: Down, 270: Left)

// Re-added: Player movement control variables (for continuous movement)
bool player_is_moving = false;   // Flag to indicate if a movement key is held
int player_desired_dx = 0;       // Desired change in player's x-coordinate (-1, 0, or 1)
int player_desired_dy = 0;       // Desired change in player's y-coordinate (-1, 0, or 1)


// Monster variables
int monster_x, monster_y;         // Monster's current position
int monster_move_timer = 0;       // Timer to control monster movement
S2D_Image *monster_sprites[2];    // Images for monster animation
int monster_frame = 0;            // Current animation frame for monster
int monster_direction_angle = 0;  // NEW: Rotation angle for monster sprite
int prev_monster_x, prev_monster_y; // NEW: Previous monster position for direction detection


// Goal graphics
S2D_Image *goal_img = NULL;       // Image for the goal

// Maze Graphics
S2D_Image *wall_img = NULL;       // Image for the maze walls (wll.png)
S2D_Image *background_img = NULL; // Image for the background below walls (background.png)

// Global offsets for centering the maze within the fixed window
int maze_offset_x = 0;
int maze_offset_y = 0;

// UI Images (pointers to S2D_Image objects)
S2D_Image *start_button_img = NULL;
S2D_Image *pause_button_img = NULL;
S2D_Image *resume_button_img = NULL;
S2D_Image *home_button_img = NULL;
S2D_Image *pause_exit_button_img = NULL; // Exit button for PAUSED state
S2D_Image *gameover_exit_button_img = NULL; // Exit button for GAME_OVER state
S2D_Image *gamewon_exit_button_img = NULL; // NEW: Exit button for GAME_WON state

S2D_Image *game_over_img = NULL;
S2D_Image *retry_button_img = NULL;
S2D_Image *win_image_sprite = NULL; // NEW: Sprite for the "You Win" image


// UI Button Bounding Boxes (for click detection)
ButtonRect start_button_rect;
ButtonRect pause_button_rect;
ButtonRect resume_button_rect;
ButtonRect home_button_rect;
ButtonRect pause_exit_button_rect;
ButtonRect gameover_exit_button_rect;
ButtonRect gamewon_exit_button_rect; // NEW: ButtonRect for game won exit button
ButtonRect retry_button_rect;

// UI Text elements
S2D_Text *title_text = NULL;
S2D_Text *instruction_text = NULL;
S2D_Text *pause_text = NULL;
S2D_Text *game_over_text = NULL;
S2D_Text *game_won_text = NULL;
S2D_Text *current_maze_label_text = NULL;
S2D_Text *round_label_text = NULL;      // NEW: "ROUND" label
S2D_Text *current_round_value_text = NULL; // NEW: Round number value
char round_buffer[16];                  // NEW: Buffer for round number string

// Sound effects
S2D_Sound *game_start_sound = NULL; // NEW: Sound for game start/new round
S2D_Sound *game_over_sound = NULL;  // NEW: Sound for game over


// --- Function Prototypes ---
// Simple 2D callbacks
void update();
void render();
void on_key(S2D_Event e);
void on_mouse(S2D_Event e);

// Game logic functions
bool load_maze(int maze_number);
void draw_maze();
void draw_player();
void draw_monster();
// Changed collision check to use float for player position
bool check_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
bool is_valid_move(int r, int c);
// New: Check if a floating point position is valid for a cell
bool is_valid_pixel_move(float x, float y, float w, float h);
bool find_monster_next_move(int *next_r, int *next_c); // Monster AI using BFS
void reset_game();
bool is_dead_end(); // Checks if player is in a dead end

// UI functions
void game_init(); // Consolidated initialization function
void game_cleanup(); // Consolidated cleanup function
void draw_start_screen();
void draw_game_ui(); // Draws the pause button during gameplay
void draw_pause_menu();
void draw_game_over_screen();
void draw_game_won_screen(); // NEW: Function to draw the game won screen
bool is_mouse_over_button(S2D_Event e, ButtonRect rect); // Helper for button clicks

// Button click handlers
void handle_start_button_click();
void handle_pause_button_click();
void handle_resume_button_click();
void handle_home_button_click();
void handle_exit_button_click();
void handle_retry_button_click();


// --- Maze Loading Function ---
/**
 * @brief Loads a maze from a text file into the `current_maze` structure.
 * It reads the maze grid and identifies 'P', 'M', 'G' positions.
 * @param maze_idx The 0-based index of the maze to load (e.g., 0 for maze_1.txt).
 * @return True if the maze was loaded successfully, false otherwise.
 */
bool load_maze(int maze_idx) {
    char filename[256];
    // Construct the filename, e.g., "assets/maze_1.txt"
    snprintf(filename, sizeof(filename), "assets/maze_%d.txt", maze_idx + 1);

    FILE *file = fopen(filename, "r");
    if (!file) {
        S2D_Error("load_maze", "Failed to open maze file: %s. Make sure 'assets' folder and maze files exist.", filename);
        return false;
    }

    int r = 0; // Row counter
    char line[MAZE_MAX_WIDTH + 2]; // Buffer for reading a line (+2 for newline and null terminator)
    current_maze.width = 0; // Initialize width and height
    current_maze.height = 0;
    bool player_found = false;
    bool monster_found = false;
    bool goal_found = false;

    // Read maze line by line
    while (fgets(line, sizeof(line), file) != NULL && r < MAZE_MAX_HEIGHT) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;
        int c = 0; // Column counter
        for (c = 0; c < strlen(line) && c < MAZE_MAX_WIDTH; ++c) {
            current_maze.grid[r][c] = line[c]; // Store character in grid
            // Identify and store spawn/goal coordinates
            if (line[c] == 'P') {
                current_maze.player_start_x = c;
                current_maze.player_start_y = r;
                player_found = true;
            } else if (line[c] == 'M') {
                current_maze.monster_start_x = c;
                current_maze.monster_start_y = r;
                monster_found = true;
            } else if (line[c] == 'G') {
                current_maze.goal_x = c;
                current_maze.goal_y = r;
                goal_found = true;
            }
        }
        if (r == 0) current_maze.width = c; // Set maze width based on the first line
        r++; // Move to next row
    }
    current_maze.height = r; // Set maze height

    fclose(file); // Close the file

    // Check if all essential elements were found
    if (!player_found || !monster_found || !goal_found) {
        S2D_Error("load_maze", "Maze file %s is missing Player (P), Monster (M), or Goal (G) markers.", filename);
        return false;
    }

    // Calculate offsets for centering the maze within the window FIRST
    maze_offset_x = (SCREEN_WIDTH - (current_maze.width * CELL_SIZE)) / 2;
    maze_offset_y = (SCREEN_HEIGHT - (current_maze.height * CELL_SIZE)) / 2;

    // Set player and monster to their starting positions for the new maze
    player_x = (float)current_maze.player_start_x * CELL_SIZE + maze_offset_x; // Initialize player_x/y as pixel coordinates
    player_y = (float)current_maze.player_start_y * CELL_SIZE + maze_offset_y;
    monster_x = current_maze.monster_start_x;
    monster_y = current_maze.monster_start_y;
    prev_monster_x = monster_x; // NEW: Initialize previous monster position
    prev_monster_y = monster_y; // NEW: Initialize previous monster position


    // Initialize previous player position for animation check
    prev_player_x = player_x;
    prev_y = player_y;
    player_frame = 0; // Reset animation frame on new maze load

    // Add detailed log for successful maze load
    S2D_Log(S2D_INFO, "Maze %d loaded successfully. Dimensions: %dx%d. Player Start: (%d,%d), Monster Start: (%d,%d), Goal: (%d,%d)",
            maze_idx + 1, current_maze.width, current_maze.height,
            (int)((player_x - maze_offset_x) / CELL_SIZE), (int)((player_y - maze_offset_y) / CELL_SIZE), monster_x, monster_y, current_maze.goal_x, current_maze.goal_y);
    return true;
}

// --- Game Initialization and Cleanup ---
void game_init() {
    S2D_Log(S2D_INFO, "Game Initialization.");
    // Initialize random seed
    srand(time(NULL));

    // Load game specific images
    player_stand_img = S2D_CreateImage("assets/stand.png"); // Player standing image
    // Load player animation sprites
    char player_sprite_path[256];
    for (int i = 0; i < 8; ++i) {
        snprintf(player_sprite_path, sizeof(player_sprite_path), "assets/player%d.png", i + 1);
        player_sprites[i] = S2D_CreateImage(player_sprite_path);
        if (!player_sprites[i]) {
            S2D_Log(S2D_WARN, "Player sprite %s not found. Player animation may not work.", player_sprite_path);
        }
    }
    // Load monster animation sprites
    monster_sprites[0] = S2D_CreateImage("assets/monster1.png");
    monster_sprites[1] = S2D_CreateImage("assets/monster2.png");
    if (!monster_sprites[0] || !monster_sprites[1]) {
        S2D_Log(S2D_WARN, "Monster sprites not found. Monster animation may not work.");
    }

    wall_img = S2D_CreateImage("assets/wll.png"); // Load wall image (grass tile)
    if (!wall_img) {
        S2D_Log(S2D_WARN, "assets/wll.png not found. Maze walls will be drawn as fallback rectangles.");
    }
    background_img = S2D_CreateImage("assets/background.png"); // Load background image
    if (!background_img) {
        S2D_Log(S2D_WARN, "assets/background.png not found. Background will be drawn as a fallback color.");
    }
    goal_img = S2D_CreateImage("assets/goal.png"); // Load goal image
    if (!goal_img) {
        S2D_Log(S2D_WARN, "assets/goal.png not found. Goal will be drawn as a fallback color.");
    }

    // Get current window dimensions to center/position UI elements
    int win_w = SCREEN_WIDTH;
    int win_h = SCREEN_HEIGHT;

    // Calculate position for Start Button (centered)
    start_button_img = S2D_CreateImage("assets/start.png");
    if (start_button_img) {
        start_button_rect.w = START_BUTTON_WIDTH_VAL;
        start_button_rect.h = START_BUTTON_HEIGHT_VAL;
        start_button_rect.x = (win_w - start_button_rect.w) / 2;
        start_button_rect.y = (win_h - start_button_rect.h) / 2;
        start_button_img->x = start_button_rect.x;
        start_button_img->y = start_button_rect.y;
        start_button_img->width = start_button_rect.w;
        start_button_img->height = start_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/start.png not found. Start button will be a fallback rectangle.");
        start_button_rect = (ButtonRect){win_w/2 - START_BUTTON_WIDTH_VAL/2, win_h/2 - START_BUTTON_HEIGHT_VAL/2, START_BUTTON_WIDTH_VAL, START_BUTTON_HEIGHT_VAL};
    }

    // Calculate position for Pause Button (bottom-left corner, zero margin)
    pause_button_img = S2D_CreateImage("assets/pause.png");
    if (pause_button_img) {
        pause_button_rect.w = PAUSE_BUTTON_WIDTH_VAL;
        pause_button_rect.h = PAUSE_BUTTON_HEIGHT_VAL;
        pause_button_rect.x = 0; // Zero margin left
        pause_button_rect.y = win_h - PAUSE_BUTTON_HEIGHT_VAL; // Zero margin bottom
        pause_button_img->x = pause_button_rect.x;
        pause_button_img->y = pause_button_rect.y;
        pause_button_img->width = pause_button_rect.w;
        pause_button_img->height = pause_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/pause.png not found. Pause button will be a fallback rectangle.");
        pause_button_rect = (ButtonRect){0, win_h - PAUSE_BUTTON_HEIGHT_VAL, PAUSE_BUTTON_WIDTH_VAL, PAUSE_BUTTON_HEIGHT_VAL};
    }

    // Calculate positions for Pause Menu Buttons (vertically distributed, centered)
    // Use PAUSE_MENU_BUTTON_SPACING for vertical separation
    int total_menu_height = RESUME_BUTTON_HEIGHT_VAL + HOME_BUTTON_HEIGHT_VAL + PAUSE_EXIT_BUTTON_HEIGHT_VAL + (PAUSE_MENU_BUTTON_SPACING * 2);
    int menu_start_y = (win_h - total_menu_height) / 2;

    resume_button_img = S2D_CreateImage("assets/resume.png");
    if (resume_button_img) {
        resume_button_rect.w = RESUME_BUTTON_WIDTH_VAL;
        resume_button_rect.h = RESUME_BUTTON_HEIGHT_VAL;
        resume_button_rect.x = (win_w - RESUME_BUTTON_WIDTH_VAL) / 2;
        resume_button_rect.y = menu_start_y;
        resume_button_img->x = resume_button_rect.x;
        resume_button_img->y = resume_button_rect.y;
        resume_button_img->width = resume_button_rect.w;
        resume_button_img->height = resume_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/resume.png not found.");
        resume_button_rect = (ButtonRect){(win_w - RESUME_BUTTON_WIDTH_VAL) / 2, menu_start_y, RESUME_BUTTON_WIDTH_VAL, RESUME_BUTTON_HEIGHT_VAL};
    }

    home_button_img = S2D_CreateImage("assets/home.png");
    if (home_button_img) {
        home_button_rect.w = HOME_BUTTON_WIDTH_VAL;
        home_button_rect.h = HOME_BUTTON_HEIGHT_VAL;
        home_button_rect.x = (win_w - HOME_BUTTON_WIDTH_VAL) / 2;
        home_button_rect.y = menu_start_y + RESUME_BUTTON_HEIGHT_VAL + PAUSE_MENU_BUTTON_SPACING;
        home_button_img->x = home_button_rect.x;
        home_button_img->y = home_button_rect.y;
        home_button_img->width = home_button_rect.w;
        home_button_img->height = home_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/home.png not found.");
        home_button_rect = (ButtonRect){(win_w - HOME_BUTTON_WIDTH_VAL) / 2, menu_start_y + RESUME_BUTTON_HEIGHT_VAL + PAUSE_MENU_BUTTON_SPACING, HOME_BUTTON_WIDTH_VAL, HOME_BUTTON_HEIGHT_VAL};
    }

    // Change: Load exit2.png for pause menu exit button
    pause_exit_button_img = S2D_CreateImage("assets/exit2.png");
    if (pause_exit_button_img) {
        pause_exit_button_rect.w = PAUSE_EXIT_BUTTON_WIDTH_VAL;
        pause_exit_button_rect.h = PAUSE_EXIT_BUTTON_HEIGHT_VAL;
        pause_exit_button_rect.x = (win_w - PAUSE_EXIT_BUTTON_WIDTH_VAL) / 2;
        pause_exit_button_rect.y = menu_start_y + RESUME_BUTTON_HEIGHT_VAL + HOME_BUTTON_HEIGHT_VAL + (PAUSE_MENU_BUTTON_SPACING * 2);
        pause_exit_button_img->x = pause_exit_button_rect.x;
        pause_exit_button_img->y = pause_exit_button_rect.y;
        pause_exit_button_img->width = pause_exit_button_rect.w;
        pause_exit_button_img->height = pause_exit_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/exit2.png not found.");
        pause_exit_button_rect = (ButtonRect){(win_w - PAUSE_EXIT_BUTTON_WIDTH_VAL) / 2, menu_start_y + RESUME_BUTTON_HEIGHT_VAL + HOME_BUTTON_HEIGHT_VAL + (PAUSE_MENU_BUTTON_SPACING * 2), PAUSE_EXIT_BUTTON_WIDTH_VAL, PAUSE_EXIT_BUTTON_HEIGHT_VAL};
    }

    // Calculate positions for Game Over Screen elements
    game_over_img = S2D_CreateImage("assets/gameover.png");
    if (game_over_img) {
        game_over_img->width = GAMEOVER_IMAGE_WIDTH; // Set width using global constant
        game_over_img->height = GAMEOVER_IMAGE_HEIGHT; // Set height using global constant
        game_over_img->x = (win_w - game_over_img->width) / 2; // Use new width for centering
        game_over_img->y = (win_h - game_over_img->height) / 2 - 50; // Use new height for centering
    } else {
        S2D_Log(S2D_WARN, "assets/gameover.png not found.");
    }

    int go_btn_y_start = (game_over_img ? game_over_img->y + game_over_img->height : win_h / 2) + BUTTON_MARGIN;

    retry_button_img = S2D_CreateImage("assets/retry.png");
    if (retry_button_img) {
        retry_button_rect.w = RETRY_BUTTON_WIDTH_VAL;
        retry_button_rect.h = RETRY_BUTTON_HEIGHT_VAL;
        retry_button_rect.x = (win_w - RETRY_BUTTON_WIDTH_VAL) / 2;
        retry_button_rect.y = go_btn_y_start;
        retry_button_img->x = retry_button_rect.x;
        retry_button_img->y = retry_button_rect.y;
        retry_button_img->width = retry_button_rect.w;
        retry_button_img->height = retry_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/retry.png not found.");
        retry_button_rect = (ButtonRect){(win_w - RETRY_BUTTON_WIDTH_VAL) / 2, go_btn_y_start, RETRY_BUTTON_WIDTH_VAL, RETRY_BUTTON_HEIGHT_VAL};
    }

    // Change: Load exit.png for game over exit button
    gameover_exit_button_img = S2D_CreateImage("assets/exit.png");
    if (gameover_exit_button_img) {
        gameover_exit_button_rect.w = GAMEOVER_EXIT_BUTTON_WIDTH_VAL;
        gameover_exit_button_rect.h = GAMEOVER_EXIT_BUTTON_HEIGHT_VAL;
        gameover_exit_button_rect.x = (win_w - GAMEOVER_EXIT_BUTTON_WIDTH_VAL) / 2;
        gameover_exit_button_rect.y = go_btn_y_start + RETRY_BUTTON_HEIGHT_VAL + BUTTON_MARGIN;
        gameover_exit_button_img->x = gameover_exit_button_rect.x;
        gameover_exit_button_img->y = gameover_exit_button_rect.y;
        gameover_exit_button_img->width = gameover_exit_button_rect.w;
        gameover_exit_button_img->height = gameover_exit_button_rect.h;
    } else {
        S2D_Log(S2D_WARN, "assets/exit.png not found.");
        gameover_exit_button_rect = (ButtonRect){(win_w - GAMEOVER_EXIT_BUTTON_WIDTH_VAL) / 2, go_btn_y_start + RETRY_BUTTON_HEIGHT_VAL + BUTTON_MARGIN, GAMEOVER_EXIT_BUTTON_WIDTH_VAL, GAMEOVER_EXIT_BUTTON_HEIGHT_VAL};
    }

    // NEW: Load win.png and set its dimensions
    win_image_sprite = S2D_CreateImage("assets/win.png");
    if (win_image_sprite) {
        win_image_sprite->width = WIN_IMAGE_WIDTH;
        win_image_sprite->height = WIN_IMAGE_HEIGHT;
    } else {
        S2D_Log(S2D_WARN, "assets/win.png not found. 'You Win' screen will use fallback text.");
    }

    // NEW: Load exit2.png for game won exit button
    gamewon_exit_button_img = S2D_CreateImage("assets/exit2.png");
    if (gamewon_exit_button_img) {
        gamewon_exit_button_rect.w = GAMEWON_EXIT_BUTTON_WIDTH_VAL;
        gamewon_exit_button_rect.h = GAMEWON_EXIT_BUTTON_HEIGHT_VAL;
        // Ensure the image's internal width/height match the rect's for consistent rendering
        gamewon_exit_button_img->width = gamewon_exit_button_rect.w;
        gamewon_exit_button_img->height = gamewon_exit_button_rect.h;
        // Position will be set dynamically in draw_game_won_screen
    } else {
        S2D_Log(S2D_WARN, "assets/exit2.png not found for game won exit button. Using fallback rectangle.");
        // Ensure fallback rect has dimensions even if image fails to load
        gamewon_exit_button_rect.w = GAMEWON_EXIT_BUTTON_WIDTH_VAL;
        gamewon_exit_button_rect.h = GAMEWON_EXIT_BUTTON_HEIGHT_VAL;
    }


    // Initialize UI text elements
    // Using Debrosee-ALPnL.ttf for all text
    title_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "MONSTER MAZE", TITLE_FONT_SIZE);
    title_text->color = (S2D_Color){0.0, 1.0, 0.0, 1.0}; // Green
    
    instruction_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "Press START to begin!", INSTRUCTION_FONT_SIZE);
    instruction_text->color = (S2D_Color){1.0, 1.0, 1.0, 1.0}; // White
    
    pause_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "PAUSED", PAUSE_TEXT_SIZE);
    pause_text->color = (S2D_Color){1.0, 1.0, 0.0, 1.0}; // Yellow
    
    game_over_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "GAME OVER", GAME_OVER_TEXT_SIZE);
    game_over_text->color = (S2D_Color){1.0, 0.0, 0.0, 1.0}; // Red
    
    game_won_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "YOU WIN!", GAME_WON_TEXT_SIZE);
    game_won_text->color = (S2D_Color){0.0, 0.0, 1.0, 1.0}; // Blue
    
    current_maze_label_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "Maze 1/5", MAZE_INFO_FONT_SIZE);
    current_maze_label_text->color = (S2D_Color){1.0, 1.0, 0.0, 1.0}; // Changed to Yellow
    
    // NEW: Round number text elements
    round_label_text = S2D_CreateText("assets/Debrosee-ALPnL.ttf", "ROUND", MAZE_INFO_FONT_SIZE);
    if (round_label_text) round_label_text->color = (S2D_Color){1.0, 1.0, 0.0, 1.0}; // Changed to Yellow
    else S2D_Log(S2D_WARN, "Failed to load Debrosee-ALPnL.ttf for ROUND label.");

    current_round_value_text = S2D_CreateText("assets/Carre-JWja.ttf", "1", MAZE_INFO_FONT_SIZE);
    if (current_round_value_text) current_round_value_text->color = (S2D_Color){1.0, 1.0, 0.0, 1.0}; // Changed to Yellow
    else S2D_Log(S2D_WARN, "Failed to load Carre-JWja.ttf for round value.");

    // NEW: Load sound effects
    game_start_sound = S2D_CreateSound("assets/gamestart.mp3");
    if (!game_start_sound) S2D_Log(S2D_WARN, "Failed to load gamestart.mp3");

    game_over_sound = S2D_CreateSound("assets/gameover.mp3");
    if (!game_over_sound) S2D_Log(S2D_WARN, "Failed to load gameover.mp3");
}

void game_cleanup() {
    S2D_Log(S2D_INFO, "Game Cleanup.");
    // Free assets
    if (player_stand_img) S2D_FreeImage(player_stand_img);
    for (int i = 0; i < 8; i++) {
        if (player_sprites[i]) S2D_FreeImage(player_sprites[i]);
    }
    for (int i = 0; i < 2; i++) {
        if (monster_sprites[i]) S2D_FreeImage(monster_sprites[i]);
    }
    if (goal_img) S2D_FreeImage(goal_img);
    if (wall_img) S2D_FreeImage(wall_img);
    if (background_img) S2D_FreeImage(background_img);

    // Free UI images
    if (start_button_img) S2D_FreeImage(start_button_img);
    if (pause_button_img) S2D_FreeImage(pause_button_img);
    if (resume_button_img) S2D_FreeImage(resume_button_img);
    if (home_button_img) S2D_FreeImage(home_button_img);
    if (pause_exit_button_img) S2D_FreeImage(pause_exit_button_img);
    if (gameover_exit_button_img) S2D_FreeImage(gameover_exit_button_img);
    if (gamewon_exit_button_img) S2D_FreeImage(gamewon_exit_button_img); // NEW: Free game won exit button
    if (game_over_img) S2D_FreeImage(game_over_img);
    if (retry_button_img) S2D_FreeImage(retry_button_img);
    if (win_image_sprite) S2D_FreeImage(win_image_sprite); // NEW: Free win image sprite


    // Free UI text elements
    if (title_text) S2D_FreeText(title_text);
    if (instruction_text) S2D_FreeText(instruction_text);
    if (pause_text) S2D_FreeText(pause_text);
    if (game_over_text) S2D_FreeText(game_over_text);
    if (game_won_text) S2D_FreeText(game_won_text);
    if (current_maze_label_text) S2D_FreeText(current_maze_label_text);
    if (round_label_text) S2D_FreeText(round_label_text);      // NEW: Free round label
    if (current_round_value_text) S2D_FreeText(current_round_value_text); // NEW: Free round value

    // NEW: Free sound effects
    if (game_start_sound) S2D_FreeSound(game_start_sound);
    if (game_over_sound) S2D_FreeSound(game_over_sound);
}

// --- Game State Management ---
void reset_game() {
    current_maze_index = 0;
    current_round = 1; // Reset round number
    // Reset player and monster positions when loading a new maze
    // Player positions are now pixel coordinates
    // These will be correctly set by load_maze after calculating maze_offset_x/y
    player_x = 0; // Temporarily set to 0, load_maze will update
    player_y = 0; // Temporarily set to 0, load_maze will update
    prev_player_x = 0;
    prev_y = 0;
    player_frame = 0; player_frame_timer = 0; player_direction_angle = 0;
    player_is_moving = false; // Reset movement state
    player_desired_dx = 0;    // Reset desired direction
    player_desired_dy = 0;    // Reset desired direction
    monster_x = 0; monster_y = 0;
    prev_monster_x = monster_x; // NEW: Reset previous monster position
    prev_monster_y = monster_y; // NEW: Reset previous monster position
    monster_move_timer = 0; monster_frame = 0;
    monster_direction_angle = 0; // NEW: Reset monster direction angle

    load_maze(current_maze_index); // Load the first maze, which now correctly sets player_x/y
    current_game_state = START_SCREEN; // Go back to the start screen
    S2D_Log(S2D_INFO, "Game reset to start screen.");
}

// --- Event Handlers ---
void on_key(S2D_Event e) {
    if (current_game_state == PLAYING) {
        // Handle key down events to start continuous movement
        if (e.type == S2D_KEY_DOWN) {
            if (strcmp(e.key, "w") == 0 || strcmp(e.key, "Up") == 0) {
                player_desired_dy = -1;
            } else if (strcmp(e.key, "s") == 0 || strcmp(e.key, "Down") == 0) {
                player_desired_dy = 1;
            } else if (strcmp(e.key, "a") == 0 || strcmp(e.key, "Left") == 0) {
                player_desired_dx = -1;
                player_direction_angle = 270; // Left
            } else if (strcmp(e.key, "d") == 0 || strcmp(e.key, "Right") == 0) {
                player_desired_dx = 1;
                player_direction_angle = 90; // Right
            } else if (strcmp(e.key, "escape") == 0) {
                current_game_state = PAUSED;
                S2D_Log(S2D_INFO, "Game Paused by keyboard.");
                // Stop player movement when pausing
                player_is_moving = false;
                player_desired_dx = 0;
                player_desired_dy = 0;
                return;
            }
            // Update player_is_moving based on current desired movement
            player_is_moving = (player_desired_dx != 0 || player_desired_dy != 0);

        }
        // Handle key up events to stop continuous movement
        else if (e.type == S2D_KEY_UP) {
            if (strcmp(e.key, "w") == 0 || strcmp(e.key, "Up") == 0) {
                player_desired_dy = 0;
            } else if (strcmp(e.key, "s") == 0 || strcmp(e.key, "Down") == 0) {
                player_desired_dy = 0;
            } else if (strcmp(e.key, "a") == 0 || strcmp(e.key, "Left") == 0) {
                player_desired_dx = 0;
            } else if (strcmp(e.key, "d") == 0 || strcmp(e.key, "Right") == 0) {
                player_desired_dx = 0;
            }
            // Update player_is_moving based on current desired movement
            player_is_moving = (player_desired_dx != 0 || player_desired_dy != 0);
        }
    } else if (e.type == S2D_KEY_DOWN && strcmp(e.key, "escape") == 0) {
        if (current_game_state == PAUSED) {
            current_game_state = PLAYING;
            S2D_Log(S2D_INFO, "Game Resumed by keyboard.");
        }
    }
}

// --- Mouse Event Handler ---
void on_mouse(S2D_Event e) {
    if (e.type == S2D_MOUSE_DOWN && e.button == S2D_MOUSE_LEFT) {
        if (current_game_state == START_SCREEN) {
            if (is_mouse_over_button(e, start_button_rect)) {
                handle_start_button_click();
            }
        } else if (current_game_state == PLAYING) {
            if (is_mouse_over_button(e, pause_button_rect)) {
                handle_pause_button_click();
            }
        } else if (current_game_state == PAUSED) {
            if (is_mouse_over_button(e, resume_button_rect)) {
                handle_resume_button_click();
            } else if (is_mouse_over_button(e, home_button_rect)) {
                handle_home_button_click();
            } else if (is_mouse_over_button(e, pause_exit_button_rect)) {
                handle_exit_button_click();
            }
        } else if (current_game_state == GAME_OVER) {
            if (is_mouse_over_button(e, retry_button_rect)) {
                handle_retry_button_click();
            } else if (is_mouse_over_button(e, gameover_exit_button_rect)) {
                handle_exit_button_click();
            }
        } else if (current_game_state == GAME_WON) {
            // For GAME_WON state, allow Home and Exit buttons
            if (is_mouse_over_button(e, home_button_rect)) { // Assuming home button is positioned for win screen
                handle_home_button_click();
            } else if (is_mouse_over_button(e, gamewon_exit_button_rect)) { // NEW: Use specific rect for game won exit button
                handle_exit_button_click();
            }
        }
    }
}


// --- Game Logic Updates ---
void update() {
    if (current_game_state == PLAYING) {
        prev_player_x = player_x; // Store current position before potential movement
        prev_y = player_y;

        // Player movement logic (continuous)
        if (player_is_moving) {
            float new_player_x = player_x + (player_desired_dx * PLAYER_SPEED);
            float new_player_y = player_y + (player_desired_dy * PLAYER_SPEED);

            // Check for collision with walls before updating position
            if (is_valid_pixel_move(new_player_x, new_player_y, PLAYER_DRAW_SIZE, PLAYER_DRAW_SIZE)) {
                player_x = new_player_x;
                player_y = new_player_y;
                // S2D_Log(S2D_INFO, "Player moved to: (%.2f, %.2f)", player_x, player_y);
            } else {
                // If the direct move is invalid, try to slide along the wall
                // Try moving only horizontally
                if (is_valid_pixel_move(new_player_x, player_y, PLAYER_DRAW_SIZE, PLAYER_DRAW_SIZE)) {
                    player_x = new_player_x;
                    // S2D_Log(S2D_INFO, "Player slid horizontally to: (%.2f, %.2f)", player_x, player_y);
                }
                // Try moving only vertically
                else if (is_valid_pixel_move(player_x, new_player_y, PLAYER_DRAW_SIZE, PLAYER_DRAW_SIZE)) {
                    player_y = new_player_y;
                    // S2D_Log(S2D_INFO, "Player slid vertically to: (%.2f, %.2f)", player_x, player_y);
                }
                // If no movement is possible, stop the player
                else {
                    // S2D_Log(S2D_INFO, "Player hit a wall and stopped.");
                    player_is_moving = false; // Stop movement if blocked
                    player_desired_dx = 0;
                    player_desired_dy = 0;
                }
            }
        }

        // Player animation logic (only if player has actually moved)
        if (player_x != prev_player_x || player_y != prev_y) {
            player_frame_timer++;
            if (player_frame_timer >= PLAYER_ANIMATION_SPEED) {
                player_frame = (player_frame + 1) % 8; // Cycle through 8 frames
                player_frame_timer = 0;
            }
        } else {
            // If player is not moving, reset to the first frame (standing pose)
            player_frame = 0;
            player_frame_timer = 0;
        }

        // Monster movement logic
        monster_move_timer++;
        if (monster_move_timer >= MONSTER_MOVE_DELAY) {
            prev_monster_x = monster_x; // Store previous position before movement
            prev_monster_y = monster_y; // Store previous position before movement

            int next_r, next_c;
            if (find_monster_next_move(&next_r, &next_c)) {
                monster_x = next_c;
                monster_y = next_r;

                // NEW: Determine monster direction based on movement
                if (monster_x > prev_monster_x) { // Moving Right
                    monster_direction_angle = 90;
                } else if (monster_x < prev_monster_x) { // Moving Left
                    monster_direction_angle = 270;
                } else if (monster_y > prev_monster_y) { // Moving Down
                    monster_direction_angle = 180;
                } else if (monster_y < prev_monster_y) { // Moving Up
                    monster_direction_angle = 0;
                }
            }
            monster_move_timer = 0;

            // Update monster animation frame
            monster_frame = (monster_frame + 1) % 2; // Cycle between 0 and 1
        }


        // Check for collision between player and monster
        if (check_collision(
                player_x, player_y, PLAYER_DRAW_SIZE, PLAYER_DRAW_SIZE,
                monster_x * CELL_SIZE + maze_offset_x, monster_y * CELL_SIZE + maze_offset_y, MONSTER_DRAW_SIZE, MONSTER_DRAW_SIZE)) {
            current_game_state = GAME_OVER;
            if (game_over_sound) S2D_PlaySound(game_over_sound); // Play game over sound
            S2D_Log(S2D_INFO, "Game Over: Monster caught player!");
        }

        // Check if the player is trapped in a dead end (game over condition)
        // This check should use the player's current grid cell, not pixel position
        if (is_dead_end()) {
            current_game_state = GAME_OVER;
            if (game_over_sound) S2D_PlaySound(game_over_sound); // Play game over sound
            S2D_Log(S2D_INFO, "Game Over: Player in dead end!");
        }

        // Check if player reached the goal
        // Convert player pixel position to grid coordinates for goal check
        int player_grid_x = (int)((player_x - maze_offset_x + (CELL_SIZE / 2)) / CELL_SIZE);
        int player_grid_y = (int)((player_y - maze_offset_y + (CELL_SIZE / 2)) / CELL_SIZE);

        if (player_grid_x == current_maze.goal_x && player_grid_y == current_maze.goal_y) {
            current_maze_index++;
            current_round++; // Increment round number
            if (current_maze_index < NUM_MAZES) {
                if (load_maze(current_maze_index)) {
                    current_game_state = PLAYING; // Stay in playing state, just load next maze
                    if (game_start_sound) S2D_PlaySound(game_start_sound); // Play game start sound for new round
                    S2D_Log(S2D_INFO, "Player reached goal! Loading next maze: %d", current_maze_index + 1);
                } else {
                    S2D_Error("update", "Failed to load next maze. Ending game.");
                    current_game_state = GAME_OVER;
                    if (game_over_sound) S2D_PlaySound(game_over_sound); // Play game over sound
                }
            } else {
                current_game_state = GAME_WON;
                S2D_Log(S2D_INFO, "Game Won! All mazes completed.");
            }
        }
    }
}

// --- Drawing Function for Player ---
/**
 * @brief This function draws the player, handling animation and rotation.
 */
void draw_player() {
    S2D_Image *current_player_img = NULL;

    // Determine which sprite to use: standing or animated
    if (player_frame == 0 && !player_is_moving) { // Use stand.png if not moving and on first frame
        current_player_img = player_stand_img;
    } else {
        // Player is moving or in an animation cycle, use animation sprite
        current_player_img = player_sprites[player_frame];
    }

    if (current_player_img) {
        current_player_img->x = player_x; // Use pixel coordinates directly
        current_player_img->y = player_y; // Use pixel coordinates directly
        current_player_img->width = PLAYER_DRAW_SIZE;
        current_player_img->height = PLAYER_DRAW_SIZE;

        // Reset rotation before applying new one to avoid cumulative rotation issues
        current_player_img->rotate = 0;
        current_player_img->rx = 0;
        current_player_img->ry = 0;

        // Apply rotation based on player direction
        if (player_direction_angle != 0) { // Only rotate if not facing up (0 degrees)
            // Rotate around the center of the image
            S2D_RotateImage(current_player_img, player_direction_angle, S2D_CENTER);
        }
        S2D_DrawImage(current_player_img);
    } else {
        // Fallback: Draw player as a blue circle if images are not found
        S2D_DrawCircle(
            player_x + PLAYER_DRAW_SIZE / 2, // Center X of the player sprite
            player_y + PLAYER_DRAW_SIZE / 2, // Center Y of the player sprite
            PLAYER_DRAW_SIZE / 3,                 // Radius (1/3rd of cell size)
            20,                                   // Number of sectors for smoothness
            0.0f, 0.0f, 1.0f, 1.0f                // Blue color (R, G, B, A)
        );
    }
}

// --- Drawing Function for Monster ---
/**
 * @brief Draws the monster character with animation.
 */
void draw_monster() {
    S2D_Image *current_monster_img = monster_sprites[monster_frame];

    if (current_monster_img) {
        current_monster_img->x = monster_x * CELL_SIZE + maze_offset_x;
        current_monster_img->y = monster_y * CELL_SIZE + maze_offset_y;
        current_monster_img->width = MONSTER_DRAW_SIZE;
        current_monster_img->height = MONSTER_DRAW_SIZE;

        // Reset rotation before applying new one
        current_monster_img->rotate = 0;
        current_monster_img->rx = 0;
        current_monster_img->ry = 0;

        // NEW: Apply rotation based on monster direction
        if (monster_direction_angle != 0) { // Only rotate if not facing up (0 degrees)
            S2D_RotateImage(current_monster_img, monster_direction_angle, S2D_CENTER);
        }
        S2D_DrawImage(current_monster_img);
    } else {
        // Fallback: Draw monster as a red square (quad) if images are not found
        S2D_DrawQuad(
            monster_x * CELL_SIZE + maze_offset_x, monster_y * CELL_SIZE + maze_offset_y,           1.0f, 0.0f, 0.0f, 1.0f, // Top-left
            (monster_x + 1) * CELL_SIZE + maze_offset_x, monster_y * CELL_SIZE + maze_offset_y,     1.0f, 0.0f, 0.0f, 1.0f, // Top-right
            (monster_x + 1) * CELL_SIZE + maze_offset_x, (monster_y + 1) * CELL_SIZE + maze_offset_y, 1.0f, 0.0f, 0.0f, 1.0f, // Bottom-right
            monster_x * CELL_SIZE + maze_offset_x, (monster_y + 1) * CELL_SIZE + maze_offset_y,     1.0f, 0.0f, 0.0f, 1.0f  // Bottom-left
        );
    }
}

// --- Drawing Function for Maze ---
/**
 * @brief Draws the background, maze walls, and the goal cell.
 */
void draw_maze() {
    // 1. Draw the background image first to cover the entire window
    if (background_img) {
        background_img->x = 0;
        background_img->y = 0;
        background_img->width = SCREEN_WIDTH;
        background_img->height = SCREEN_HEIGHT;
        S2D_DrawImage(background_img);
    } else {
        // Fallback: Draw a dark grey quad for the general background if image not found
        S2D_DrawQuad(0, 0, 0.1f, 0.1f, 0.1f, 1.0f,
                     SCREEN_WIDTH, 0, 0.1f, 0.1f, 0.1f, 1.0f,
                     SCREEN_WIDTH, SCREEN_HEIGHT, 0.1f, 0.1f, 0.1f, 1.0f,
                     0, SCREEN_HEIGHT, 0.1f, 0.1f, 0.1f, 1.0f);
    }

    // 2. Iterate through the maze grid and draw walls and goal on top of the background
    for (int r = 0; r < current_maze.height; ++r) {
        for (int c = 0; c < current_maze.width; ++c) {
            if (current_maze.grid[r][c] == '#') {
                // Draw wall using wll.png
                if (wall_img) {
                    wall_img->x = c * CELL_SIZE + maze_offset_x;
                    wall_img->y = r * CELL_SIZE + maze_offset_y;
                    wall_img->width = WALL_DRAW_SIZE;
                    wall_img->height = WALL_DRAW_SIZE;
                    S2D_DrawImage(wall_img);
                } else {
                    // Fallback: Draw a dark grey quad if wll.png is not found
                    S2D_DrawQuad(
                        c * CELL_SIZE + maze_offset_x, r * CELL_SIZE + maze_offset_y,           0.3f, 0.3f, 0.3f, 1.0f, // Top-left
                        (c + 1) * CELL_SIZE + maze_offset_x, r * CELL_SIZE + maze_offset_y,     0.3f, 0.3f, 0.3f, 1.0f, // Top-right
                        (c + 1) * CELL_SIZE + maze_offset_x, (r + 1) * CELL_SIZE + maze_offset_y, 0.3f, 0.3f, 0.3f, 1.0f, // Bottom-right
                        c * CELL_SIZE + maze_offset_x, (r + 1) * CELL_SIZE + maze_offset_y,     0.3f, 0.3f, 0.3f, 1.0f  // Bottom-left
                    );
                }
            } else if (current_maze.grid[r][c] == 'G') {
                // Draw goal (goal.png) with rotation based on adjacent wall
                if (goal_img) {
                    goal_img->x = c * CELL_SIZE + maze_offset_x;
                    goal_img->y = r * CELL_SIZE + maze_offset_y;
                    goal_img->width = GOAL_DRAW_SIZE;
                    goal_img->height = GOAL_DRAW_SIZE;

                    // Determine rotation based on surrounding walls
                    int goal_rotation_angle = 0; // Default: 0 degrees (downward)

                    // Check for wall below
                    if (r + 1 < current_maze.height && current_maze.grid[r + 1][c] == '#') {
                        goal_rotation_angle = 0; // Goal is on top of a bottom wall, arrow points down
                    }
                    // Check for wall to the left
                    else if (c - 1 >= 0 && current_maze.grid[r][c - 1] == '#') {
                        goal_rotation_angle = 90; // Goal is on right of a left wall, arrow points right
                    }
                    // Check for wall to the right
                    else if (c + 1 < current_maze.width && current_maze.grid[r][c + 1] == '#') {
                        goal_rotation_angle = 270; // Goal is on left of a right wall, arrow points left
                    }
                    // Check for wall above
                    else if (r - 1 >= 0 && current_maze.grid[r - 1][c] == '#') {
                        goal_rotation_angle = 180; // Goal is on bottom of a top wall, arrow points up
                    }

                    // Apply rotation
                    goal_img->rotate = 0; // Reset rotation
                    goal_img->rx = 0;
                    goal_img->ry = 0;
                    if (goal_rotation_angle != 0) {
                        S2D_RotateImage(goal_img, goal_rotation_angle, S2D_CENTER);
                    }
                    S2D_DrawImage(goal_img);
                } else {
                    // Fallback: Draw a bright green quad if goal.png is not found
                    S2D_DrawQuad(
                        c * CELL_SIZE + maze_offset_x, r * CELL_SIZE + maze_offset_y,           0.0f, 0.8f, 0.0f, 1.0f,
                        (c + 1) * CELL_SIZE + maze_offset_x, r * CELL_SIZE + maze_offset_y,     0.0f, 0.8f, 0.0f, 1.0f,
                        (c + 1) * CELL_SIZE + maze_offset_x, (r + 1) * CELL_SIZE + maze_offset_y, 0.0f, 0.8f, 0.0f, 1.0f,
                        c * CELL_SIZE + maze_offset_x, (r + 1) * CELL_SIZE + maze_offset_y,     0.0f, 0.8f, 0.0f, 1.0f
                    );
                }
            }
            // 'P' and 'M' cells are just paths, but player/monster are drawn on top
        }
    }
}

// --- Render Function (Drawing) ---
/**
 * @brief This function is called repeatedly by Simple 2D for drawing graphics.
 */
void render() {
    S2D_GL_Clear(game_window->background); // Clear the screen each frame with the window's background color

    if (current_game_state == START_SCREEN) {
        draw_start_screen(); // Display the start button
    } else if (current_game_state == PLAYING || current_game_state == PAUSED || current_game_state == GAME_OVER || current_game_state == GAME_WON) {
        draw_maze();         // Draw maze walls and goal
        draw_player();       // Draw the player
        draw_monster();      // Draw the monster

        // Draw maze label (e.g., "Maze 1/5")
        char maze_label[20];
        snprintf(maze_label, sizeof(maze_label), "Maze %d/%d", current_maze_index + 1, NUM_MAZES);
        S2D_SetText(current_maze_label_text, maze_label); // Update text string
        current_maze_label_text->x = BUTTON_MARGIN;
        current_maze_label_text->y = BUTTON_MARGIN;
        S2D_DrawText(current_maze_label_text);

        // NEW: Draw Round Number
        if (round_label_text && current_round_value_text) {
            round_label_text->x = SCREEN_WIDTH - round_label_text->width - BUTTON_MARGIN - current_round_value_text->width; // Position "ROUND" to the left of the number
            round_label_text->y = BUTTON_MARGIN;
            S2D_DrawText(round_label_text);

            snprintf(round_buffer, sizeof(round_buffer), "%d", current_round);
            S2D_SetText(current_round_value_text, round_buffer);
            current_round_value_text->x = SCREEN_WIDTH - current_round_value_text->width - BUTTON_MARGIN; // Position number
            current_round_value_text->y = BUTTON_MARGIN;
            S2D_DrawText(current_round_value_text);
        }

        if (current_game_state == PAUSED) {
            draw_pause_menu();   // Display pause menu buttons
        } else if (current_game_state == GAME_OVER) {
            draw_game_over_screen(); // Display game over image and retry/exit buttons
        } else if (current_game_state == GAME_WON) {
            draw_game_won_screen(); // NEW: Display "You Win" image and buttons
        } else { // PLAYING state
             draw_game_ui();      // Draw the pause button
        }
    }
}


// --- Helper Functions ---
/**
 * @brief Checks if a given maze cell coordinate is valid (within bounds and not a wall).
 * @param r Row coordinate.
 * @param c Column coordinate.
 * @return True if the move is valid, false otherwise.
 */
bool is_valid_move(int r, int c) {
    return (r >= 0 && r < current_maze.height &&     // Check row bounds
            c >= 0 && c < current_maze.width &&      // Check column bounds
            current_maze.grid[r][c] != '#');         // Check if it's not a wall
}

/**
 * @brief Checks if a given pixel position (x, y) for an object of size (w, h)
 * is valid within the maze, meaning it does not overlap with any wall cells.
 * @param x X-coordinate of the top-left corner of the object.
 * @param y Y-coordinate of the top-left corner of the object.
 * @param w Width of the object.
 * @param h Height of the object.
 * @return True if the object at the given pixel position does not collide with any walls, false otherwise.
 */
bool is_valid_pixel_move(float x, float y, float w, float h) {
    // Convert pixel coordinates to maze grid coordinates for the bounding box corners
    int left_col = (int)((x - maze_offset_x) / CELL_SIZE);
    int top_row = (int)((y - maze_offset_y) / CELL_SIZE);
    int right_col = (int)((x + w - 1 - maze_offset_x) / CELL_SIZE); // -1 to check inside the pixel
    int bottom_row = (int)((y + h - 1 - maze_offset_y) / CELL_SIZE);

    // Clamp coordinates to maze bounds
    if (left_col < 0) left_col = 0;
    if (top_row < 0) top_row = 0;
    if (right_col >= current_maze.width) right_col = current_maze.width - 1;
    if (bottom_row >= current_maze.height) bottom_row = current_maze.height - 1;

    // Check all cells within the object's bounding box
    for (int r = top_row; r <= bottom_row; ++r) {
        for (int c = left_col; c <= right_col; ++c) {
            // If any cell is a wall, the move is invalid
            if (current_maze.grid[r][c] == '#') {
                return false;
            }
        }
    }
    return true;
}


/**
 * @brief Performs Axis-Aligned Bounding Box (AABB) collision detection.
 * @param x1, y1, w1, h1 Coordinates and dimensions of the first rectangle.
 * @param x2, y2, w2, h2 Coordinates and dimensions of the second rectangle.
 * @return True if the rectangles overlap, false otherwise.
 */
bool check_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 && // Rect1's left edge is to the left of Rect2's right edge
            x1 + w1 > x2 && // Rect1's right edge is to the right of Rect2's left edge
            y1 < y2 + h2 && // Rect1's top edge is above Rect2's bottom edge
            y1 + h1 > y2);  // Rect1's bottom edge is below Rect2's top edge
}

/**
 * @brief Finds the monster's next optimal move towards the player using Breadth-First Search (BFS).
 * @param next_r Pointer to store the next row for the monster.
 * @param next_c Pointer to store the next column for the monster.
 * @return True if a path to the player was found and a next move is determined, false otherwise.
 */
bool find_monster_next_move(int *next_r, int *next_c) {
    // BFS Node structure to store cell coordinates, distance from monster, and parent index in queue
    typedef struct {
        int r, c;
        int dist;
        int parent_idx; // Index of the parent node in the queue, used for path reconstruction
    } BFSNode;

    // Queue for BFS. Max size is total maze cells.
    BFSNode queue[MAZE_MAX_WIDTH * MAZE_MAX_HEIGHT];
    int head = 0, tail = 0; // Queue pointers

    // 2D array to keep track of visited cells to prevent infinite loops
    bool visited[MAZE_MAX_HEIGHT][MAZE_MAX_WIDTH];
    for (int i = 0; i < current_maze.height; ++i) {
        for (int j = 0; j < current_maze.width; ++j) {
            visited[i][j] = false;
        }
    }

    // Add monster's current position to the queue as the starting point
    queue[tail++] = (BFSNode){monster_y, monster_x, 0, -1}; // -1 parent_idx for the root node
    visited[monster_y][monster_x] = true;

    // Directions for movement (Up, Down, Left, Right)
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    int player_node_idx = -1; // Stores the index of the player's node in the queue once found

    // Convert player pixel position to grid coordinates for BFS target
    int player_grid_x = (int)((player_x - maze_offset_x + (PLAYER_DRAW_SIZE / 2)) / CELL_SIZE);
    int player_grid_y = (int)((player_y - maze_offset_y + (PLAYER_DRAW_SIZE / 2)) / CELL_SIZE);

    // Clamp player grid coordinates to maze bounds
    if (player_grid_x < 0) player_grid_x = 0;
    if (player_grid_y < 0) player_grid_y = 0;
    if (player_grid_x >= current_maze.width) player_grid_x = current_maze.width - 1;
    if (player_grid_y >= current_maze.height) player_grid_y = current_maze.height - 1;


    // BFS loop
    while (head < tail) {
        BFSNode current = queue[head++]; // Dequeue the current node

        // If the current node is the player's position, we found the shortest path
        if (current.r == player_grid_y && current.c == player_grid_x) {
            player_node_idx = head - 1; // Store the index of this node
            break; // Exit BFS, path found
        }

        // Explore neighbors
        for (int i = 0; i < 4; ++i) {
            int nr = current.r + dr[i]; // Neighbor row
            int nc = current.c + dc[i]; // Neighbor column

            // If neighbor is valid (not a wall, within bounds) and not visited
            if (is_valid_move(nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = true; // Mark as visited
                // Enqueue the neighbor, storing its distance and parent
                queue[tail++] = (BFSNode){nr, nc, current.dist + 1, head - 1};
            }
        }
    }

    // Path Reconstruction: If player was found, trace back to find the monster's next step
    if (player_node_idx != -1) {
        int path_len = 0;
        int current_path_idx = player_node_idx;
        // Count path length by traversing up to the root (monster's starting position)
        while (queue[current_path_idx].parent_idx != -1) {
            path_len++;
            current_path_idx = queue[current_path_idx].parent_idx;
        }

        // If path_len is 0, monster is already on player (should not happen with proper spawn)
        if (path_len == 0) {
            *next_r = monster_y;
            *next_c = monster_x;
            return false; // No move needed
        }

        // To get the next step, find the node that is one step away from the monster
        // This is the (path_len - 1)th node from the player, when traversing backwards
        current_path_idx = player_node_idx;
        for (int i = 0; i < path_len - 1; ++i) {
            current_path_idx = queue[current_path_idx].parent_idx;
        }

        // The coordinates of this node are the monster's next move
        *next_r = queue[current_path_idx].r;
        *next_c = queue[current_path_idx].c;
        return true; // A valid next move was found
    }

    return false; // No path to player found
}

/**
 * @brief Resets the game to its initial state (start screen, first maze).
 */
// The definition of reset_game is already present above, no need for a duplicate.


/**
 * @brief Checks if the player is in a dead end.
 * A dead end is defined as having no valid moves (surrounded by walls or monster)
 * AND the monster being adjacent to the player.
 * @return True if the player is in a dead end, false otherwise.
 */
bool is_dead_end() {
    int valid_moves_count = 0;
    int dr[] = {-1, 1, 0, 0}; // Directions: Up, Down, Left, Right
    int dc[] = {0, 0, -1, 1};

    // Convert player pixel position to grid coordinates for dead end check
    int player_grid_x = (int)((player_x - maze_offset_x + (PLAYER_DRAW_SIZE / 2)) / CELL_SIZE);
    int player_grid_y = (int)((player_y - maze_offset_y + (PLAYER_DRAW_SIZE / 2)) / CELL_SIZE);

    // Clamp player grid coordinates to maze bounds
    if (player_grid_x < 0) player_grid_x = 0;
    if (player_grid_y < 0) player_grid_y = 0;
    if (player_grid_x >= current_maze.width) player_grid_x = current_maze.width - 1;
    if (player_grid_y >= current_maze.height) player_grid_y = current_maze.height - 1;


    // Check all 4 cardinal directions around the player's grid cell
    for (int i = 0; i < 4; ++i) {
        int nr = player_grid_y + dr[i];
        int nc = player_grid_x + dc[i];
        // A move is valid if it's within bounds, not a wall, AND not the monster's current position
        if (is_valid_move(nr, nc) && !(nr == monster_y && nc == monster_x)) {
            valid_moves_count++;
        }
    }

    // If the player has no valid moves (all surrounding cells are walls or occupied by monster)
    if (valid_moves_count == 0) {
        // Additionally, check if the monster is adjacent to the player.
        // This prevents ending the game if the player is just surrounded by walls but monster is far.
        for (int i = 0; i < 4; ++i) {
            int nr = player_grid_y + dr[i];
            int nc = player_grid_x + dc[i];
            if (nr == monster_y && nc == monster_x) {
                return true; // Monster is adjacent AND player has no escape -> dead end!
            }
        }
    }
    return false; // Not a dead end
}


// --- UI Functions ---

/**
 * @brief Draws the start screen, primarily the start button.
 */
void draw_start_screen() {
    // Draw the start button image
    if (start_button_img) {
        S2D_DrawImage(start_button_img);
    } else {
        // Fallback: Draw a green rectangle if start.png is not found
        S2D_DrawQuad(
            start_button_rect.x, start_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f,
            start_button_rect.x + start_button_rect.w, start_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f,
            start_button_rect.x + start_button_rect.w, start_button_rect.y + start_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f,
            start_button_rect.x, start_button_rect.y + start_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f
        );
    }

    title_text->x = (SCREEN_WIDTH - title_text->width) / 2;
    title_text->y = SCREEN_HEIGHT / 4;
    S2D_DrawText(title_text);

    instruction_text->x = (SCREEN_WIDTH - instruction_text->width) / 2;
    instruction_text->y = SCREEN_HEIGHT / 4 + title_text->height + 20;
    S2D_DrawText(instruction_text);
    // Buttons are drawn by their respective image functions, not directly here.
    // The on_mouse function handles clicks on these rects.
}

/**
 * @brief Draws the in-game UI elements, specifically the pause button.
 */
void draw_game_ui() {
    if (pause_button_img) {
        S2D_DrawImage(pause_button_img); // Draw the pause button image
    } else {
        // Fallback: Draw a yellow rectangle if pause.png is not found
        S2D_DrawQuad(
            pause_button_rect.x, pause_button_rect.y, 0.7f, 0.7f, 0.2f, 1.0f,
            pause_button_rect.x + pause_button_rect.w, pause_button_rect.y, 0.7f, 0.7f, 0.2f, 1.0f,
            pause_button_rect.x + pause_button_rect.w, pause_button_rect.y + pause_button_rect.h, 0.7f, 0.7f, 0.2f, 1.0f,
            pause_button_rect.x, pause_button_rect.y + pause_button_rect.h, 0.7f, 0.7f, 0.2f, 1.0f
        );
    }
}

/**
 * @brief Draws the pause menu, including a dimmed background and menu buttons.
 */
void draw_pause_menu() {
    // Draw a semi-transparent black overlay to dim the game behind the menu
    S2D_DrawQuad(
        0, 0, 0.0f, 0.0f, 0.0f, 0.5f, // Top-left (black, 50% alpha)
        SCREEN_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.5f, // Top-right
        SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f, // Bottom-right
        0, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f // Bottom-left
    );

    pause_text->x = (SCREEN_WIDTH - pause_text->width) / 2;
    pause_text->y = SCREEN_HEIGHT / 4 - 30; // Move text up by 30 pixels
    S2D_DrawText(pause_text);

    // Draw the resume button
    if (resume_button_img) {
        resume_button_img->x = resume_button_rect.x;
        resume_button_img->y = resume_button_rect.y;
        S2D_DrawImage(resume_button_img);
    } else { // Fallback for resume button
        S2D_DrawQuad(
            resume_button_rect.x, resume_button_rect.y,           0.2f, 0.7f, 0.2f, 1.0f, // Green
            resume_button_rect.x + resume_button_rect.w, resume_button_rect.y,     0.2f, 0.7f, 0.2f, 1.0f,
            resume_button_rect.x + resume_button_rect.w, resume_button_rect.y + resume_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f,
            resume_button_rect.x, resume_button_rect.y + resume_button_rect.h,     0.2f, 0.7f, 0.2f, 1.0f
        );
    }
    // Draw the home button
    if (home_button_img) {
        home_button_img->x = home_button_rect.x;
        home_button_img->y = home_button_rect.y;
        S2D_DrawImage(home_button_img);
    } else { // Fallback for home button
        S2D_DrawQuad(
            home_button_rect.x, home_button_rect.y,           0.5f, 0.5f, 0.5f, 1.0f, // Grey
            home_button_rect.x + home_button_rect.w, home_button_rect.y,     0.5f, 0.5f, 0.5f, 1.0f,
            home_button_rect.x + home_button_rect.w, home_button_rect.y + home_button_rect.h, 0.5f, 0.5f, 0.5f, 1.0f,
            home_button_rect.x, home_button_rect.y + home_button_rect.h,     0.5f, 0.5f, 0.5f, 1.0f
        );
    }
    // Draw the pause exit button (using exit2.png as requested)
    if (pause_exit_button_img) {
        pause_exit_button_img->x = pause_exit_button_rect.x;
        pause_exit_button_img->y = pause_exit_button_rect.y;
        S2D_DrawImage(pause_exit_button_img);
    } else { // Fallback for pause exit button
        S2D_DrawQuad(
            pause_exit_button_rect.x, pause_exit_button_rect.y,           1.0f, 0.0f, 0.0f, 1.0f, // Red
            pause_exit_button_rect.x + pause_exit_button_rect.w, pause_exit_button_rect.y,     1.0f, 0.0f, 0.0f, 1.0f,
            pause_exit_button_rect.x + pause_exit_button_rect.w, pause_exit_button_rect.y + pause_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f,
            pause_exit_button_rect.x, pause_exit_button_rect.y + pause_exit_button_rect.h,     1.0f, 0.0f, 0.0f, 1.0f
        );
    }
}

/**
 * @brief Draws the game over screen, including the game over image and retry/exit buttons.
 */
void draw_game_over_screen() {
    // Draw a more opaque black overlay for the game over screen
    S2D_DrawQuad(
        0, 0, 0.0f, 0.0f, 0.0f, 0.7f, // Top-left (black, 70% alpha)
        SCREEN_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.7f,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f,
        0, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f
    );

    // Draw the "Game Over" image
    if (game_over_img) {
        game_over_img->x = (SCREEN_WIDTH - GAMEOVER_IMAGE_WIDTH) / 2; // Use defined width
        game_over_img->y = (SCREEN_HEIGHT - GAMEOVER_IMAGE_HEIGHT) / 2 - 50; // Use defined height, slightly above center
        S2D_DrawImage(game_over_img);
    }

    // Draw the retry button
    if (retry_button_img) {
        retry_button_img->x = retry_button_rect.x;
        retry_button_img->y = retry_button_rect.y;
        S2D_DrawImage(retry_button_img);
    } else { // Fallback for retry button
        S2D_DrawQuad(
            retry_button_rect.x, retry_button_rect.y,           0.2f, 0.7f, 0.2f, 1.0f, // Green
            retry_button_rect.x + retry_button_rect.w, retry_button_rect.y,     0.2f, 0.7f, 0.2f, 1.0f,
            retry_button_rect.x + retry_button_rect.w, retry_button_rect.y + retry_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f,
            retry_button_rect.x, retry_button_rect.y + retry_button_rect.h,     0.2f, 0.7f, 0.2f, 1.0f
        );
    }
    // Draw the game over exit button (using exit.png as requested)
    if (gameover_exit_button_img) {
        gameover_exit_button_img->x = gameover_exit_button_rect.x;
        gameover_exit_button_img->y = gameover_exit_button_rect.y;
        S2D_DrawImage(gameover_exit_button_img);
    } else { // Fallback for game over exit button
        S2D_DrawQuad(
            gameover_exit_button_rect.x, gameover_exit_button_rect.y,           1.0f, 0.0f, 0.0f, 1.0f, // Red
            gameover_exit_button_rect.x + gameover_exit_button_rect.w, gameover_exit_button_rect.y,     1.0f, 0.0f, 0.0f, 1.0f,
            gameover_exit_button_rect.x + gameover_exit_button_rect.w, gameover_exit_button_rect.y + gameover_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f,
            gameover_exit_button_rect.x, gameover_exit_button_rect.y + gameover_exit_button_rect.h,     1.0f, 0.0f, 0.0f, 1.0f
        );
    }
}

/**
 * @brief NEW: Draws the game won screen, including the win image and buttons.
 */
void draw_game_won_screen() {
    // Draw a semi-transparent overlay
    S2D_DrawQuad(
        0, 0, 0.0f, 0.0f, 0.0f, 0.5f, // Black, 50% transparent
        SCREEN_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.5f,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f,
        0, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f
    );

    // Draw the "You Win" image
    if (win_image_sprite) {
        win_image_sprite->x = (SCREEN_WIDTH - WIN_IMAGE_WIDTH) / 2; // Use defined width
        win_image_sprite->y = (SCREEN_HEIGHT - WIN_IMAGE_HEIGHT) / 2 - 50; // Use defined height, centered vertically, slightly up
        S2D_DrawImage(win_image_sprite);
    } else {
        // Fallback: Draw "YOU WIN!" text if image not found
        game_won_text->x = (SCREEN_WIDTH - game_won_text->width) / 2;
        game_won_text->y = SCREEN_HEIGHT / 4;
        S2D_DrawText(game_won_text);
    }

    // Position buttons below the win image/text
    int buttons_y_start = (win_image_sprite ? win_image_sprite->y + WIN_IMAGE_HEIGHT : SCREEN_HEIGHT / 4 + game_won_text->height) + BUTTON_MARGIN;

    // Draw Home button
    if (home_button_img) {
        home_button_rect.x = (SCREEN_WIDTH - HOME_BUTTON_WIDTH_VAL) / 2; // Center horizontally
        home_button_rect.y = buttons_y_start;
        home_button_img->x = home_button_rect.x;
        home_button_img->y = home_button_rect.y;
        S2D_DrawImage(home_button_img);
    } else { // Fallback for home button image
        home_button_rect.x = (SCREEN_WIDTH - HOME_BUTTON_WIDTH_VAL) / 2;
        home_button_rect.y = buttons_y_start;
        S2D_DrawQuad(
            home_button_rect.x, home_button_rect.y,           0.5f, 0.5f, 0.5f, 1.0f, // Grey
            home_button_rect.x + home_button_rect.w, home_button_rect.y,     0.5f, 0.5f, 0.5f, 1.0f,
            home_button_rect.x + home_button_rect.w, home_button_rect.y + home_button_rect.h, 0.5f, 0.5f, 0.5f, 1.0f,
            home_button_rect.x, home_button_rect.y + home_button_rect.h,     0.5f, 0.5f, 0.5f, 1.0f
        );
    }

    // Draw Exit button (using exit2.png as requested)
    if (gamewon_exit_button_img) {
        gamewon_exit_button_rect.x = (SCREEN_WIDTH - GAMEWON_EXIT_BUTTON_WIDTH_VAL) / 2; // Center horizontally
        gamewon_exit_button_rect.y = buttons_y_start + HOME_BUTTON_HEIGHT_VAL + BUTTON_MARGIN; // This uses BUTTON_MARGIN
        gamewon_exit_button_img->x = gamewon_exit_button_rect.x;
        gamewon_exit_button_img->y = gamewon_exit_button_rect.y;
        S2D_DrawImage(gamewon_exit_button_img);
    } else {
        // Fallback: Draw a red rectangle if the image is not found
        gamewon_exit_button_rect.x = (SCREEN_WIDTH - GAMEWON_EXIT_BUTTON_WIDTH_VAL) / 2;
        gamewon_exit_button_rect.y = buttons_y_start + HOME_BUTTON_HEIGHT_VAL + BUTTON_MARGIN;
        S2D_DrawQuad(
            gamewon_exit_button_rect.x, gamewon_exit_button_rect.y,           1.0f, 0.0f, 0.0f, 1.0f, // Red
            gamewon_exit_button_rect.x + gamewon_exit_button_rect.w, gamewon_exit_button_rect.y,     1.0f, 0.0f, 0.0f, 1.0f,
            gamewon_exit_button_rect.x + gamewon_exit_button_rect.w, gamewon_exit_button_rect.y + gamewon_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f,
            gamewon_exit_button_rect.x, gamewon_exit_button_rect.y + gamewon_exit_button_rect.h,     1.0f, 0.0f, 0.0f, 1.0f
        );
    }
}


/**
 * @brief Helper function to check if the mouse click occurred within a button's bounding box.
 * @param e The S2D_Event containing mouse coordinates.
 * @param rect The ButtonRect defining the button's area.
 * @return True if the mouse is over the button, false otherwise.
 */
bool is_mouse_over_button(S2D_Event e, ButtonRect rect) {
    return (e.x >= rect.x && e.x <= rect.x + rect.w &&
            e.y >= rect.y && e.y <= rect.y + rect.h);
}

// --- Button Click Handlers ---
void handle_start_button_click() {
    S2D_Log(S2D_INFO, "Start button clicked. Loading initial maze.");
    current_maze_index = 0; // Always start from the first maze
    current_round = 1; // Start from round 1
    // Attempt to load the first maze
    if (load_maze(current_maze_index)) {
        current_game_state = PLAYING; // Only change state to PLAYING if maze loads successfully
        if (game_start_sound) S2D_PlaySound(game_start_sound); // Play game start sound
        S2D_Log(S2D_INFO, "Game started. Player and monster should now move.");
    } else {
        S2D_Error("handle_start_button_click", "Failed to load initial maze (maze_%d.txt). Game cannot start.", current_maze_index + 1);
        // Game state remains START_SCREEN if maze loading fails, allowing user to retry or check assets
    }
}

void handle_pause_button_click() {
    S2D_Log(S2D_INFO, "Pause button clicked. Pausing game.");
    current_game_state = PAUSED; // Change state to PAUSED
}

void handle_resume_button_click() {
    S2D_Log(S2D_INFO, "Resume button clicked. Resuming game.");
    current_game_state = PLAYING; // Change state back to PLAYING
}

void handle_home_button_click() {
    S2D_Log(S2D_INFO, "Home button clicked. Returning to start screen.");
    reset_game(); // Reset game state and return to START_SCREEN
}

void handle_exit_button_click() {
    S2D_Log(S2D_INFO, "Exit button clicked. Quitting game.");
    S2D_Quit(); // Properly shut down Simple2D
    exit(0);    // Terminate the application process
}

void handle_retry_button_click() {
    S2D_Log(S2D_INFO, "Retry button clicked. Reloading current maze.");
    // Reload the current maze
    if (load_maze(current_maze_index)) {
        current_game_state = PLAYING; // Resume playing if maze loads successfully
        if (game_start_sound) S2D_PlaySound(game_start_sound); // Play game start sound for retry
        S2D_Log(S2D_INFO, "Maze reloaded. Player and monster should now move.");
    } else {
        S2D_Error("handle_retry_button_click", "Failed to reload maze (maze_%d.txt). Cannot retry.", current_maze_index + 1);
        current_game_state = GAME_OVER; // Stay in GAME_OVER state if reload fails
    }
}

// --- Main Function ---
int main(int argc, char *argv[]) {
    // Initialize Simple 2D library
    if (!S2D_Init()) {
        S2D_Error("main", "Failed to initialize Simple 2D");
        return 1;
    }

    // Create the game window
    game_window = S2D_CreateWindow(
        "Monster and the Maze",
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        update,
        render,
        0
    );

    if (!game_window) {
        S2D_Error("main", "Failed to create window");
        S2D_Quit(); // Clean up Simple 2D resources
        return 1;
    }

    // Set event handlers
    game_window->on_key = on_key;
    game_window->on_mouse = on_mouse; // Use custom on_mouse to handle ButtonRects

    // Initialize game assets and UI elements
    game_init();

    // Set a dark background color for the window
    game_window->background.r = 0.1f;
    game_window->background.g = 0.1f;
    game_window->background.b = 0.1f;
    game_window->background.a = 1.0f; // Fully opaque

    // Start the Simple 2D game loop
    S2D_Show(game_window);

    // Cleanup resources
    game_cleanup();
    S2D_FreeWindow(game_window); // Free the window and its associated resources
    S2D_Quit(); // Shut down Simple 2D subsystems

    return 0; // Indicate successful execution
}
