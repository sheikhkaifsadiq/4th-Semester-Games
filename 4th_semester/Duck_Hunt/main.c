#include "simple2d.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h> // Keep for fprintf for error logging, but remove printf
#include <time.h>
#include <math.h>
#include <string.h> // For strcmp and snprintf

// --- Global Window Dimensions ---
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// --- Game Specific Constants (Measurable Elements Declared Globally) ---

// Duck properties
const int DUCK_WIDTH = 80;  // Width of the duck sprite
const int DUCK_HEIGHT = 60; // Height of the duck sprite

// Bottom margin where duck disappears after falling (Ground Y Limit)
const int GROUND_Y_LIMIT = 400; // Changed from 500 to 400 as requested

// Game Rules Specific Constants
const int SHOTS_PER_DUCK = 3; // Number of shots player has for each duck
const int DUCKS_PER_ROUND = 5; // Total ducks that appear in each round
const int MAX_ROUNDS = 5; // Maximum number of rounds to play

// UI Button Dimensions (PNG Sizes)
// Start Button
const int START_BUTTON_WIDTH = 200;
const int START_BUTTON_HEIGHT = 80;

// Pause Button (in-game)
const int PAUSE_BUTTON_SIZE = 50;

// Pause Menu Buttons
const int RESUME_BUTTON_WIDTH = 120;
const int RESUME_BUTTON_HEIGHT = 60;
const int HOME_BUTTON_WIDTH = 120;
const int HOME_BUTTON_HEIGHT = 60;
const int PAUSE_EXIT_BUTTON_WIDTH = 120;
const int PAUSE_EXIT_BUTTON_HEIGHT = 60;

// Game Over / Game Won Buttons
const int RETRY_BUTTON_WIDTH = 120;
const int RETRY_BUTTON_HEIGHT = 60;
const int GAMEOVER_EXIT_BUTTON_WIDTH = 120;
const int GAMEOVER_EXIT_BUTTON_HEIGHT = 60;

// Common Button Spacing
#define BUTTON_MARGIN 20

// Life Heart UI Icon Size (PNG Size)
const int LIFE_HEART_SIZE = 40;

// Game Over Image Rendered Size (PNG Size)
const int GAMEOVER_IMAGE_RENDER_WIDTH = 500;
const int GAMEOVER_IMAGE_RENDER_HEIGHT = 200;

// Game Won Image Rendered Size (PNG Size)
const int GAMEWON_IMAGE_RENDER_WIDTH = 500;
const int GAMEWON_IMAGE_RENDER_HEIGHT = 200;


// Game State constants
typedef enum {
    START_SCREEN,
    PLAYING,
    PAUSED,
    ROUND_CLEAR, // New state for displaying round clear message
    GAME_OVER,
    GAME_WON // New state for winning the game
} GameState;

// Round Clear Screen Duration
const unsigned int ROUND_CLEAR_DURATION = 2000; // 2 seconds for "ROUND CLEAR" display

// Fly Away Text Duration
const unsigned int FLY_AWAY_TEXT_DURATION = 1000; // 1 second for "FLY AWAY" text

// Shotgun reload delay
const unsigned int SHOTGUN_RELOAD_DELAY = 300; // milliseconds


// --- Global Game Variables ---

// Duck properties
typedef struct {
    int x, y;
    int width, height;
    int anim_frame;
    unsigned int anim_timer;
    int anim_speed; // Duck animation speed
    int x_speed, y_speed; // Duck movement speed
    int direction;
    S2D_Sprite *right_frames[3];
    S2D_Sprite *upright_frames[3];
    S2D_Sprite *up_frames[3];
    S2D_Sprite *left_frames[3];
    S2D_Sprite *downleft_frames[3];
    S2D_Sprite *downright_frames[3];
    S2D_Sprite *down_frames[3];
    S2D_Sprite *upleft_frames[3];
    S2D_Sprite *kill_frame;
    S2D_Sprite *falling1_frame;
    S2D_Sprite *falling2_frame;
    float scale;
    unsigned int direction_timer; // Timer for duck changing direction
    bool hit_wall;
    bool is_dead;
    unsigned int death_timer; // Timer for duck death animation
    bool flew_away; // True if duck escaped
    bool active;    // Is this duck currently active in the game?
    bool falling_sound_active; // Flag to track if the falling sound has been played for this duck
    unsigned int duck_spawn_time; // Time when this duck was spawned
    int shots_remaining_on_this_duck; // Shots remaining for this specific duck
} Duck;

// Only one duck active at a time in Game A mode
Duck current_active_duck; // Use a single duck instance

S2D_Sprite *background;
S2D_Sprite *background2; // Added for when player misses
S2D_Sprite *background3; // Added for when player misses 3 times (Game Over)
S2D_Window *window; // Make window global

// Game state variables
GameState current_game_state = START_SCREEN;

// Player lives (represents how many ducks can escape before Game Over)
int player_lives = 3; // Initial lives for the game

// Array to hold life heart images for UI
S2D_Image *life_hearts[3]; // Declared life_hearts globally

S2D_Color pink_shade = {1.0f, 0.0f, 1.0f, 0.3f}; // Pink color with alpha for shading on duck shot
unsigned int miss_timer = 0;        // Timer for showing background2 (missed shot feedback)
unsigned int shot_feedback_timer = 0; // Timer for showing pink shade (duck shot feedback)

// Round management
int current_round = 1;
int ducks_killed_in_round = 0; // Tracks ducks killed in the current round
int ducks_spawned_this_round = 0;   // Total ducks spawned so far in the current round
int ducks_finished_this_round = 0;  // Total ducks that have been killed or flown away in current round

// UI Elements (Image pointers)
S2D_Image *start_button_img = NULL;
S2D_Image *pause_button_img = NULL;
S2D_Image *resume_button_img = NULL;
S2D_Image *home_button_img = NULL;
S2D_Image *pause_exit_button_img = NULL; // Exit button for PAUSED state
S2D_Image *gameover_exit_button_img = NULL; // Exit button for GAME_OVER state
S2D_Image *retry_button_img = NULL;
S2D_Image *game_over_img = NULL;
S2D_Image *game_won_img = NULL; // New: Image for game won screen

// UI Text elements
S2D_Text *game_over_text = NULL;
S2D_Text *round_display_text = NULL; // To display current round during PLAYING state
char round_display_buffer[20]; // Buffer for "Round X" text

S2D_Text *round_clear_text = NULL; // New text for "ROUND CLEAR"
S2D_Text *ducks_hit_text = NULL; // New text for "Ducks Hit: X / 10"
char ducks_hit_buffer[30]; // Buffer for ducks hit text
unsigned int round_clear_timer = 0; // Timer for ROUND_CLEAR state

S2D_Text *fly_away_text = NULL; // New text for "FLY AWAY"
unsigned int fly_away_timer = 0; // Timer for FLY_AWAY text display


// Button Bounding Box Structure
typedef struct {
    int x, y, w, h; // Position and dimensions of the button
} ButtonRect;

// Button Rectangles (positions and sizes derived from global constants)
ButtonRect start_button_rect;
ButtonRect pause_button_rect;
ButtonRect resume_button_rect;
ButtonRect home_button_rect;
ButtonRect pause_exit_button_rect;
ButtonRect gameover_exit_button_rect; // Used for both Game Over and Game Won exit
ButtonRect retry_button_rect; // Used for both Game Over and Game Won play again

// Sound variables
S2D_Sound *banjo_music = NULL;
S2D_Sound *background_game_music = NULL;
S2D_Sound *game_over_sound = NULL;
S2D_Sound *drop_fall_sound = NULL; // This will now be dynamically loaded/freed
S2D_Sound *shotgun_shot_sound = NULL;
S2D_Sound *shotgun_reload_sound = NULL;
S2D_Sound *success_sound = NULL; // This is now specifically for Game Won
S2D_Sound *wings_flap_sound = NULL;
S2D_Sound *man_grinning_sound = NULL; // This is now specifically for Round Clear
S2D_Sound *game_won_sound = NULL; // This will be assigned success.mp3

// Sound state flags/timers
bool game_over_sound_played_once = false;
bool game_won_sound_played_once = false; // New flag for game won sound
bool round_clear_sound_played_once = false; // New flag for round clear sound
unsigned int shotgun_reload_timer = 0;


// --- Function Prototypes ---
// Helper functions for common operations
bool isPointInRect(int px, int py, int rx, int ry, int rw, int rh);
int getDirectionFromSpeed(int x_speed, int y_speed);
int get_min_hits_for_round(int round_num);
int get_duck_speed_for_round(int round_num);
unsigned int get_duck_direction_change_interval_for_round(int round_num);
unsigned int get_duck_escape_time_limit_for_round(int round_num); // New: Dynamic escape time

// Duck management functions
void initDuck(Duck *duck,
              const char *right_frames_paths[],
              const char *upright_frames_paths[],
              const char *up_frames_paths[],
              const char *left_frames_paths[],
              const char *downleft_frames_paths[],
              const char *downright_frames_paths[],
              const char *down_frames_paths[],
              const char *upleft_frames_paths[],
              const char *kill_frame_path,
              const char *falling1_frame_path,
              const char *falling2_frame_path);
void updateDuck(Duck *duck);
void drawDuck(Duck *duck);
void spawn_new_duck(); // Helper to spawn a single duck

// Drawing functions
void drawBackground();
void draw_start_screen();
void draw_playing_ui(); // Draws pause button and lives
void draw_pause_menu();
void draw_round_clear_screen(); // New: Draw round clear screen
void draw_game_over_screen();
void draw_game_won_screen(); // New: Draw game won screen

// Main game loop functions (called by Simple2D)
void update();
void render();
void onMouse(S2D_Event e);

// Game flow control functions
void game_init();    // Initializes all assets and game state
void game_cleanup(); // Frees all assets before program exit
void reset_game();   // Resets game to initial state (Start Screen)
void start_round(int round_num); // Configures and starts a new round

// Button click handlers
void handle_start_button_click();
void handle_pause_button_click();
void handle_resume_button_click();
void handle_home_button_click();
void handle_exit_button_click();
void handle_retry_button_click(); // Used for both Game Over and Game Won
void handle_play_again_from_won_click(); // Specific for "Play Again" on Game Won screen

// Safe Asset Loading Functions (with error messages)
S2D_Sound* load_sound_safe(const char* path);
S2D_Sprite* load_sprite_safe(const char* path); // Reports faulty PNGs for sprites
S2D_Image* load_image_safe(const char* path);   // Reports faulty PNGs for images

// Music management functions (adjusted for Simple2D's S2D_PlaySound signature)
S2D_Sound* load_and_play_music(S2D_Sound **music_ptr, const char* path); // Loads and plays background music
void stop_and_free_music(S2D_Sound **music_ptr); // Stops and frees music resources


// --- Function Implementations ---

// Function to check collision between a point and a rectangle
bool isPointInRect(int px, int py, int rx, int ry, int rw, int rh) {
    return (px > rx && px < rx + rw && py > ry && py < ry + rh);
}

// Determines the direction index based on duck's x and y speeds
int getDirectionFromSpeed(int x_speed, int y_speed) {
    if (x_speed > 0 && y_speed == 0) return 0; // Right
    if (x_speed > 0 && y_speed < 0) return 1; // Up-Right
    if (x_speed == 0 && y_speed < 0) return 2; // Up
    if (x_speed < 0 && y_speed == 0) return 3; // Left
    if (x_speed < 0 && y_speed > 0) return 4; // Down-Left
    if (x_speed > 0 && y_speed > 0) return 5; // Down-Right
    if (x_speed == 0 && y_speed > 0) return 6; // Down
    if (x_speed < 0 && y_speed < 0) return 7; // Up-Left
    return 0; // Default to Right if no match (shouldn't happen with valid speeds)
}

// Returns the minimum number of ducks to hit for a given round
int get_min_hits_for_round(int round_num) {
    float percentage;
    switch (round_num) {
        case 1: percentage = 0.40f; break; // 40%
        case 2: percentage = 0.50f; break; // 50%
        case 3: percentage = 0.60f; break; // 60%
        case 4: percentage = 0.70f; break; // 70%
        case 5: percentage = 0.80f; break; // 80%
        default: percentage = 1.00f; break; // 100% for rounds beyond 5
    }
    return (int)ceilf(DUCKS_PER_ROUND * percentage);
}

// Returns the duck speed for a given round
int get_duck_speed_for_round(int round_num) {
    // Speed increases with round number
    if (round_num <= 2) return 2;
    if (round_num <= 5) return 3;
    if (round_num <= 10) return 4;
    return 5; // Max speed for very high rounds
}

// Returns the duck direction change interval for a given round (less frequent = more predictable)
unsigned int get_duck_direction_change_interval_for_round(int round_num) {
    // Interval decreases (more frequent changes) with higher rounds for less predictability
    if (round_num <= 2) return 2000; // 2 seconds
    if (round_num <= 5) return 1500; // 1.5 seconds
    if (round_num <= 10) return 1000; // 1 second
    return 750; // 0.75 seconds for very high rounds
}

// Returns the duck escape time limit for a given round (less time = harder)
unsigned int get_duck_escape_time_limit_for_round(int round_num) {
    // Escape time decreases (ducks fly away faster) with higher rounds
    if (round_num <= 2) return 5000; // 5 seconds
    if (round_num <= 5) return 4000; // 4 seconds
    if (round_num <= 10) return 3000; // 3 seconds
    return 2500; // 2.5 seconds for very high rounds
}


// Sets the duck's movement speeds and updates its direction index
void setDuckDirection(Duck *duck, int x_speed, int y_speed, int direction) {
    duck->x_speed = x_speed;
    duck->y_speed = y_speed;
    duck->direction = direction;
}

// Initializes a duck's properties and loads its animation sprites
// Sprites are loaded only once globally by the first duck initialized.
void initDuck(Duck *duck,
              const char *right_frames_paths[],
              const char *upright_frames_paths[],
              const char *up_frames_paths[],
              const char *left_frames_paths[],
              const char *downleft_frames_paths[],
              const char *downright_frames_paths[],
              const char *down_frames_paths[],
              const char *upleft_frames_paths[],
              const char *kill_frame_path,
              const char *falling1_frame_path,
              const char *falling2_frame_path
              ) {

    // Use a static flag to ensure sprites are loaded only once to prevent memory leaks and redundant loading.
    static bool sprites_loaded = false;
    if (!sprites_loaded) {
        for (int i = 0; i < 3; i++) {
            duck->right_frames[i] = load_sprite_safe(right_frames_paths[i]);
            duck->upright_frames[i] = load_sprite_safe(upright_frames_paths[i]);
            duck->up_frames[i] = load_sprite_safe(up_frames_paths[i]);
            duck->left_frames[i] = load_sprite_safe(left_frames_paths[i]);
            duck->downleft_frames[i] = load_sprite_safe(downleft_frames_paths[i]);
            duck->downright_frames[i] = load_sprite_safe(downright_frames_paths[i]);
            duck->down_frames[i] = load_sprite_safe(down_frames_paths[i]);
            duck->upleft_frames[i] = load_sprite_safe(upleft_frames_paths[i]);
        }
        duck->kill_frame = load_sprite_safe(kill_frame_path);
        duck->falling1_frame = load_sprite_safe(falling1_frame_path);
        duck->falling2_frame = load_sprite_safe(falling2_frame_path);
        sprites_loaded = true;
    } else {
        // If sprites are already loaded by the first duck, subsequent ducks share the same sprite pointers.
        // Since we are now using a single `current_active_duck`, this block is less critical but harmless.
        Duck *first_duck_for_sprites = &current_active_duck;
        for (int i = 0; i < 3; i++) {
            duck->right_frames[i] = first_duck_for_sprites->right_frames[i];
            duck->upright_frames[i] = first_duck_for_sprites->upright_frames[i];
            duck->up_frames[i] = first_duck_for_sprites->up_frames[i];
            duck->left_frames[i] = first_duck_for_sprites->left_frames[i];
            duck->downleft_frames[i] = first_duck_for_sprites->downleft_frames[i];
            duck->downright_frames[i] = first_duck_for_sprites->downright_frames[i];
            duck->down_frames[i] = first_duck_for_sprites->down_frames[i];
            duck->upleft_frames[i] = first_duck_for_sprites->upleft_frames[i];
        }
        duck->kill_frame = first_duck_for_sprites->kill_frame;
        duck->falling1_frame = first_duck_for_sprites->falling1_frame;
        duck->falling2_frame = first_duck_for_sprites->falling2_frame;
    }

    // Set initial duck state and position
    duck->scale = 1.0; // Reset scale to default
    duck->width = DUCK_WIDTH; // Use globally declared DUCK_WIDTH
    duck->height = DUCK_HEIGHT; // Use globally declared DUCK_HEIGHT
    duck->x = rand() % (WINDOW_WIDTH - duck->width); // Random X position within window bounds
    // Ensure initial spawn is above the ground limit
    duck->y = rand() % (GROUND_Y_LIMIT - duck->height - 50); // Spawn higher up to avoid immediate fall/hit

    duck->anim_frame = 0;
    duck->anim_timer = SDL_GetTicks();
    duck->anim_speed = 100; // Animation frame change interval in ms
    duck->is_dead = false;
    duck->death_timer = 0;
    duck->flew_away = false;
    duck->active = true; // Mark as active for game logic
    duck->falling_sound_active = false; // Initialize sound flag to false
    duck->duck_spawn_time = SDL_GetTicks(); // Record spawn time
    duck->shots_remaining_on_this_duck = SHOTS_PER_DUCK; // Reset shots for new duck

    // Determine duck speed and direction change interval based on current round
    int current_duck_speed = get_duck_speed_for_round(current_round);

    // Random initial direction
    int dir = rand() % 8;
    // Ensure initial direction doesn't immediately send duck below ground limit if starting low
    if (dir == 4 || dir == 5 || dir == 6) { // Down-Left, Down-Right, Down
        if (duck->y + duck->height + current_duck_speed > GROUND_Y_LIMIT) {
            dir = rand() % 3; // Force an upward direction (Up-Right, Up, Up-Left)
            if (dir == 0) dir = 1; // Up-Right
            else if (dir == 1) dir = 2; // Up
            else dir = 7; // Up-Left
        }
    }

    // Set duck's initial speed and direction based on chosen 'dir'
    switch (dir) {
        case 0: setDuckDirection(duck, current_duck_speed, 0, 0); break;
        case 1: setDuckDirection(duck, current_duck_speed, -current_duck_speed, 1); break;
        case 2: setDuckDirection(duck, 0, -current_duck_speed, 2); break;
        case 3: setDuckDirection(duck, -current_duck_speed, 0, 3); break;
        case 4: setDuckDirection(duck, -current_duck_speed, current_duck_speed, 4); break;
        case 5: setDuckDirection(duck, current_duck_speed, current_duck_speed, 5); break;
        case 6: setDuckDirection(duck, 0, current_duck_speed, 6); break;
        case 7: setDuckDirection(duck, -current_duck_speed, -current_duck_speed, 7); break;
    }

    duck->direction_timer = SDL_GetTicks(); // Initialize timer for direction changes
    duck->hit_wall = false; // Reset wall hit flag
}

// Spawns a new duck, setting its properties and making it active
void spawn_new_duck() {
    initDuck(&current_active_duck,
             (const char*[]){"assets/right1.png", "assets/right2.png", "assets/right3.png"},
             (const char*[]){"assets/upright1.png", "assets/upright2.png", "assets/upright3.png"},
             (const char*[]){"assets/up1.png", "assets/up2.png", "assets/up3.png"},
             (const char*[]){"assets/left1.png", "assets/left2.png", "assets/left3.png"},
             (const char*[]){"assets/downleft1.png", "assets/downleft2.png", "assets/downleft3.png"},
             (const char*[]){"assets/downright1.png", "assets/downright2.png", "assets/downright3.png"},
             (const char*[]){"assets/down1.png", "assets/down2.png", "assets/down3.png"},
             (const char*[]){"assets/upleft1.png", "assets/upleft2.png", "assets/upleft3.png"},
             "assets/kill.png", "assets/falling1.png", "assets/falling2.png");
    ducks_spawned_this_round++; // Increment count of ducks spawned for the round
    // printf("Spawned duck %d of %d for Round %d\n", ducks_spawned_this_round, DUCKS_PER_ROUND, current_round); // Removed
}


// Updates the state of a single duck (movement, animation, death/flying away)
void updateDuck(Duck *duck) {
    if (!duck->active) return; // Only update active ducks

    // Handle duck death (falling animation)
    if (duck->is_dead) {
        if (SDL_GetTicks() - duck->death_timer > 500 && duck->y < GROUND_Y_LIMIT) { // Fall for 500ms then continue falling until ground limit
            if (!duck->falling_sound_active) { // Play falling sound once when falling begins
                drop_fall_sound = load_sound_safe("assets/drop_fall.mp3"); // Dynamically load sound
                if (drop_fall_sound) S2D_PlaySound(drop_fall_sound); // Play sound once
                duck->falling_sound_active = true;
            }
            duck->y += 5; // Falling speed
            duck->anim_frame = (duck->anim_frame + 1) % 2; // Cycle between falling animation frames
        }
        if (duck->y >= GROUND_Y_LIMIT) { // Duck has fallen into bushes (reached ground limit)
            duck->y = GROUND_Y_LIMIT; // Cap at ground to prevent falling through
            duck->active = false; // Deactivate the duck as it's out of play
            ducks_finished_this_round++; // Increment finished count for round progression
            if (duck->falling_sound_active) { // If sound was active, free it
                if (drop_fall_sound) S2D_FreeSound(drop_fall_sound);
                drop_fall_sound = NULL; // Set pointer to NULL after freeing
            }
            duck->falling_sound_active = false; // Reset sound flag
        }
        return; // No further movement or direction changes if dead
    }

    // Handle duck flying away (missed by player or timed out)
    if (duck->flew_away) {
        duck->y -= 5; // Duck flies upwards
        if (SDL_GetTicks() - duck->anim_timer > duck->anim_speed) {
            duck->anim_frame = (duck->anim_frame + 1) % 3; // Animate flying up
            duck->anim_timer = SDL_GetTicks();
        }
        if (duck->y < -duck->height) { // Duck flew completely off screen
            duck->active = false; // Deactivate the duck permanently
            ducks_finished_this_round++; // Increment finished count for round progression
        }
        return; // No further movement or direction changes if flying away
    }

    // Check if duck escapes due to timeout
    if (SDL_GetTicks() - duck->duck_spawn_time > get_duck_escape_time_limit_for_round(current_round)) {
        duck->flew_away = true; // Duck flew away
        player_lives--; // Lose a life
        if (wings_flap_sound) S2D_PlaySound(wings_flap_sound); // Play wings flap sound
        fly_away_timer = SDL_GetTicks(); // Start "FLY AWAY" text timer
        // printf("Duck flew away (timeout)! Lives left: %d\n", player_lives); // Removed
        return; // Stop further updates for this duck, it's flying away
    }


    // Standard duck animation
    if (SDL_GetTicks() - duck->anim_timer > duck->anim_speed) {
        duck->anim_frame = (duck->anim_frame + 1) % 3; // Cycle through 3 animation frames
        duck->anim_timer = SDL_GetTicks();
    }

    // Duck direction change logic
    if (SDL_GetTicks() - duck->direction_timer > get_duck_direction_change_interval_for_round(current_round)) {
        duck->direction_timer = SDL_GetTicks();
        int new_dir = rand() % 8; // Randomly choose a new direction (0-7)
        int current_duck_speed = get_duck_speed_for_round(current_round);
        // Set new speed and direction based on the random choice
        switch (new_dir) {
            case 0: setDuckDirection(duck, current_duck_speed, 0, 0); break;
            case 1: setDuckDirection(duck, current_duck_speed, -current_duck_speed, 1); break;
            case 2: setDuckDirection(duck, 0, -current_duck_speed, 2); break;
            case 3: setDuckDirection(duck, -current_duck_speed, 0, 3); break;
            case 4: setDuckDirection(duck, -current_duck_speed, current_duck_speed, 4); break;
            case 5: setDuckDirection(duck, current_duck_speed, current_duck_speed, 5); break;
            case 6: setDuckDirection(duck, 0, current_duck_speed, 6); break;
            case 7: setDuckDirection(duck, -current_duck_speed, -current_duck_speed, 7); break;
    }
    }

    // Update duck position
    duck->x += duck->x_speed;
    duck->y += duck->y_speed;

    bool hit = false; // Flag to track if duck hit a boundary

    // Boundary checks for X-axis (left/right window edges)
    if (duck->x < 0) {
        duck->x = 0;          // Keep duck within bounds
        duck->x_speed *= -1;  // Reverse X direction
        hit = true;
    } else if (duck->x > WINDOW_WIDTH - duck->width) {
        duck->x = WINDOW_WIDTH - duck->width; // Keep duck within bounds
        duck->x_speed *= -1;                // Reverse X direction
        hit = true;
    }

    // Boundary checks for Y-axis (top window edge and GROUND_Y_LIMIT)
    if (duck->y < 0) {
        duck->y = 0;          // Keep duck within bounds
        duck->y_speed *= -1;  // Reverse Y direction
        hit = true;
    } else if (duck->y > GROUND_Y_LIMIT - duck->height) { // Duck hits the "ground" limit
        duck->y = GROUND_Y_LIMIT - duck->height; // Keep duck above ground
        duck->y_speed *= -1;                    // Reverse Y direction
        hit = true;
    }

    // If a boundary was hit, update duck's current direction
    if (hit) {
        duck->direction = getDirectionFromSpeed(duck->x_speed, duck->y_speed);
        duck->hit_wall = true; // Set flag (not currently used for visual feedback but could be)
    } else {
        duck->hit_wall = false;
    }
}

// Draws a single duck based on its current state and animation frame
void drawDuck(Duck *duck) {
    if (!duck->active) return; // Only draw active ducks

    // Draw dead duck (shot) animation
    if (duck->is_dead) {
        if(SDL_GetTicks() - duck->death_timer <= 500){ // First 500ms: draw kill frame
            duck->kill_frame->x = duck->x;
            duck->kill_frame->y = duck->y;
            S2D_DrawSprite(duck->kill_frame);
        }
        else{ // After 500ms: draw falling animation
            if (duck->y < GROUND_Y_LIMIT){ // Only draw falling if above ground
                if(duck->anim_frame == 0){ // Cycle between two falling frames
                    duck->falling1_frame->x = duck->x;
                    duck->falling1_frame->y = duck->y;
                    S2D_DrawSprite(duck->falling1_frame);
                }
                else{
                    duck->falling2_frame->x = duck->x;
                    duck->falling2_frame->y = duck->y;
                    S2D_DrawSprite(duck->falling2_frame);
                }
            }
        }
        return; // No further drawing for dead ducks
    }

    // Draw duck flying away (missed) animation
    if (duck->flew_away) {
        S2D_Sprite *up_frame = duck->up_frames[duck->anim_frame]; // Use 'up' animation frames
        up_frame->x = duck->x;
        up_frame->y = duck->y;
        S2D_DrawSprite(up_frame);
        return; // No further drawing for flying away ducks
    }

    // Select the correct animation frame based on current direction
    S2D_Sprite *frame_sets[8][3] = {
        {duck->right_frames[0], duck->right_frames[1], duck->right_frames[2]},
        {duck->upright_frames[0], duck->upright_frames[1], duck->upright_frames[2]},
        {duck->up_frames[0], duck->up_frames[1], duck->up_frames[2]},
        {duck->left_frames[0], duck->left_frames[1], duck->left_frames[2]},
        {duck->downleft_frames[0], duck->downleft_frames[1], duck->downleft_frames[2]},
        {duck->downright_frames[0], duck->downright_frames[1], duck->downright_frames[2]},
        {duck->down_frames[0], duck->down_frames[1], duck->down_frames[2]},
        {duck->upleft_frames[0], duck->upleft_frames[1], duck->upleft_frames[2]}
    };

    S2D_Sprite *current_frame = frame_sets[duck->direction][duck->anim_frame];
    current_frame->x = duck->x;
    current_frame->y = duck->y;
    S2D_DrawSprite(current_frame); // Draw the current animation frame
}

// Draws the appropriate background based on game state (miss, shot feedback, game over)
void drawBackground() {
    if (miss_timer > 0) { // A miss occurred, show feedback background (background2)
        if (SDL_GetTicks() - miss_timer < 300) { // Show for 300ms
            background2->x = 0; background2->y = 0;
            background2->width = WINDOW_WIDTH; background2->height = WINDOW_HEIGHT;
            S2D_DrawSprite(background2); // Show background2.png
        } else {
            miss_timer = 0; // Reset timer, revert to default background logic
        }
    } else if (shot_feedback_timer > 0) { // A duck was shot, show pink shade feedback
        if (SDL_GetTicks() - shot_feedback_timer < 300) { // Show shade for 300ms
            background->x = 0; background->y = 0;
            background->width = WINDOW_WIDTH; background->height = WINDOW_HEIGHT;
            S2D_DrawSprite(background);
             // Draw a semi-transparent pink quad over the background
             S2D_DrawQuad(
                0, 0, pink_shade.r, pink_shade.g, pink_shade.b, pink_shade.a,
                WINDOW_WIDTH, 0, pink_shade.r, pink_shade.g, pink_shade.b, pink_shade.a,
                WINDOW_WIDTH, WINDOW_HEIGHT, pink_shade.r, pink_shade.g, pink_shade.b, pink_shade.a,
                0, WINDOW_HEIGHT, pink_shade.r, pink_shade.g, pink_shade.b, pink_shade.a
            );
        } else {
            shot_feedback_timer = 0; // Reset timer, revert to default background
        }
    }
    else if (player_lives <= 0 && current_game_state == GAME_OVER) // Only show background3 if game over
    {
        background3->x = 0; background3->y = 0;
        background3->width = WINDOW_WIDTH; background3->height = WINDOW_HEIGHT;
        S2D_DrawSprite(background3);
    }
    else {
        background->x = 0; background->y = 0;
        background->width = WINDOW_WIDTH; background->height = WINDOW_HEIGHT;
        S2D_DrawSprite(background); // Show the default background
    }
}

// Main update loop for game logic (called by Simple2D)
void update() {
    unsigned int current_ticks = SDL_GetTicks(); // Get current time once per update for efficiency

    // Handle shotgun reload sound timer
    if (shotgun_reload_timer > 0 && current_ticks - shotgun_reload_timer > SHOTGUN_RELOAD_DELAY) {
        if (shotgun_reload_sound) S2D_PlaySound(shotgun_reload_sound);
        shotgun_reload_timer = 0; // Reset timer after playing sound
    }

    // Handle game state transitions and logic
    switch (current_game_state) {
        case START_SCREEN:
        case PAUSED:
        case GAME_OVER:
        case GAME_WON: // No game logic updates in these UI-focused states
            return;

        case ROUND_CLEAR:
            // Play man_grinning_sound when round is successfully cleared, only once
            if (!round_clear_sound_played_once && ducks_killed_in_round >= get_min_hits_for_round(current_round)) {
                if (man_grinning_sound) S2D_PlaySound(man_grinning_sound);
                round_clear_sound_played_once = true;
            }

            if (current_ticks - round_clear_timer > ROUND_CLEAR_DURATION) {
                // Transition to next round or game over if round clear fails
                if (ducks_killed_in_round >= get_min_hits_for_round(current_round)) {
                    // Check if this is the last round
                    if (current_round == MAX_ROUNDS) {
                        current_game_state = GAME_WON; // Transition to Game Won state
                        if (!game_won_sound_played_once) {
                            stop_and_free_music(&background_game_music);
                            if (game_won_sound) S2D_PlaySound(game_won_sound); // This is success.mp3
                            game_won_sound_played_once = true;
                        }
                        // printf("Game Won! All %d rounds completed.\n", MAX_ROUNDS); // Removed
                    } else {
                        current_round++; // Increment to next round
                        ducks_killed_in_round = 0;
                        ducks_spawned_this_round = 0;
                        ducks_finished_this_round = 0;
                        player_lives = 3; // Reset lives for the new round
                        current_game_state = PLAYING;
                        load_and_play_music(&background_game_music, "assets/background.mp3"); // Start game music
                        spawn_new_duck(); // Spawn the first duck of the new round
                        // printf("Round %d completed! Starting Round %d.\n", current_round - 1, current_round); // Removed
                    }
                } else {
                    // Didn't hit enough ducks, game over
                    current_game_state = GAME_OVER;
                    if (!game_over_sound_played_once) {
                        stop_and_free_music(&background_game_music);
                        if (game_over_sound) S2D_PlaySound(game_over_sound);
                        game_over_sound_played_once = true;
                    }
                    // printf("Game Over! Didn't hit enough ducks in Round %d.\n", current_round); // Removed
                }
            }
            return; // No further game logic during ROUND_CLEAR

        case PLAYING:
            // Update the single active duck's state
            if (current_active_duck.active) {
                updateDuck(&current_active_duck);
            }

            // Handle "FLY AWAY" text display timer
            if (fly_away_timer > 0 && current_ticks - fly_away_timer > FLY_AWAY_TEXT_DURATION) {
                fly_away_timer = 0; // Reset timer to hide text
            }

            // If the current duck is no longer active (killed or flew away)
            if (!current_active_duck.active) {
                // Check if all ducks for the current round have been processed
                if (ducks_finished_this_round >= DUCKS_PER_ROUND) {
                    // End of round logic
                    stop_and_free_music(&background_game_music); // Stop game music before round clear screen
                    current_game_state = ROUND_CLEAR;
                    round_clear_timer = current_ticks; // Start round clear timer
                    round_clear_sound_played_once = false; // Reset flag for next round clear

                    // Prepare text for round clear screen
                    snprintf(ducks_hit_buffer, sizeof(ducks_hit_buffer), "Ducks Hit: %d / %d", ducks_killed_in_round, DUCKS_PER_ROUND);
                    S2D_SetText(ducks_hit_text, ducks_hit_buffer);

                } else {
                    // Not all ducks processed for the round, spawn the next one
                    spawn_new_duck();
                }
            }

            // Check for Game Over condition based on player lives
            if (player_lives <= 0) {
                current_game_state = GAME_OVER;
                if (!game_over_sound_played_once) {
                    stop_and_free_music(&background_game_music); // Stop game music on game over
                    if (game_over_sound) S2D_PlaySound(game_over_sound);
                    game_over_sound_played_once = true;
                }
                // Ensure the current duck flies away if game over due to lives
                if (current_active_duck.active && !current_active_duck.is_dead && !current_active_duck.flew_away) {
                    current_active_duck.flew_away = true;
                    current_active_duck.x_speed = 0;
                    current_active_duck.y_speed = -get_duck_speed_for_round(current_round); // Fly up at current round speed
                    setDuckDirection(&current_active_duck, 0, -get_duck_speed_for_round(current_round), 2); // Set 'up' direction
                    if (wings_flap_sound) S2D_PlaySound(wings_flap_sound); // Play wings flap sound
                }
                // printf("Game Over! No lives left.\n"); // Removed
            }
            break;
    }
}

// Main render loop for drawing (called by Simple2D)
void render() {
    drawBackground(); // Draw the current background

    // Draw the single active duck
    if (current_active_duck.active) {
        drawDuck(&current_active_duck);
    }

    // Draw UI elements based on the current game state
    if (current_game_state == START_SCREEN) {
        draw_start_screen();
    } else if (current_game_state == PLAYING) {
        draw_playing_ui(); // Draw pause button and lives

        // Display current round number
        snprintf(round_display_buffer, sizeof(round_display_buffer), "Round %d / %d", current_round, MAX_ROUNDS);
        S2D_SetText(round_display_text, round_display_buffer);
        round_display_text->x = (WINDOW_WIDTH - round_display_text->width) / 2;
        round_display_text->y = 10; // Top center
        S2D_DrawText(round_display_text);

        // Draw "FLY AWAY" text if active
        if (fly_away_timer > 0) {
            float elapsed_time = (float)(SDL_GetTicks() - fly_away_timer) / 1000.0f;
            float alpha = 1.0f - (elapsed_time / (FLY_AWAY_TEXT_DURATION / 1000.0f)); // Fade out
            fly_away_text->color.a = fmaxf(0.0f, alpha); // Ensure alpha doesn't go below 0

            fly_away_text->x = (WINDOW_WIDTH - fly_away_text->width) / 2;
            fly_away_text->y = (WINDOW_HEIGHT / 2) - (fly_away_text->height / 2);
            S2D_DrawText(fly_away_text);
        }

    } else if (current_game_state == PAUSED) {
        draw_pause_menu();
    } else if (current_game_state == ROUND_CLEAR) {
        draw_round_clear_screen(); // Draw the new round clear screen
    } else if (current_game_state == GAME_OVER) {
        draw_game_over_screen();
    } else if (current_game_state == GAME_WON) { // New: Draw game won screen
        draw_game_won_screen();
    }
}

// Handles mouse click events
void onMouse(S2D_Event e) {
    if (e.type == S2D_MOUSE_DOWN && e.button == S2D_MOUSE_LEFT) { // Only respond to left mouse clicks
        if (current_game_state == START_SCREEN) {
            if (isPointInRect(e.x, e.y, start_button_rect.x, start_button_rect.y, start_button_rect.w, start_button_rect.h)) {
                handle_start_button_click(); // Start button clicked
            }
        } else if (current_game_state == PLAYING) {
            // Play shotgun shot sound and set reload timer on every click in PLAYING state
            if (shotgun_shot_sound) S2D_PlaySound(shotgun_shot_sound);
            shotgun_reload_timer = SDL_GetTicks();

            // Check for pause button click first
            if (isPointInRect(e.x, e.y, pause_button_rect.x, pause_button_rect.y, pause_button_rect.w, pause_button_rect.h)) {
                handle_pause_button_click();
                return; // Consume event, don't check for duck clicks if pause button clicked
            }

            // Check for duck clicks if not pause button
            if (current_active_duck.active && !current_active_duck.is_dead && !current_active_duck.flew_away) {
                // Check if mouse click is within duck's bounding box
                if (isPointInRect(e.x, e.y, current_active_duck.x, current_active_duck.y, current_active_duck.width, current_active_duck.height)) {
                    current_active_duck.is_dead = true;         // Mark duck as hit
                    current_active_duck.death_timer = SDL_GetTicks(); // Start death timer
                    current_active_duck.anim_frame = 0;         // Reset animation frame for kill sprite
                    current_active_duck.x_speed = 0;            // Stop horizontal movement
                    current_active_duck.y_speed = 0;            // Stop vertical movement (before falling)
                    ducks_killed_in_round++;                // Increment score
                    shot_feedback_timer = SDL_GetTicks();   // Start visual feedback (pink shade)
                    // Removed: if (success_sound) S2D_PlaySound(success_sound); // This was playing success.mp3 on duck kill
                    // printf("Duck shot! Ducks killed this round: %d\n", ducks_killed_in_round); // Removed
                } else {
                    // Player missed the duck
                    current_active_duck.shots_remaining_on_this_duck--;
                    // printf("Miss! Shots remaining on current duck: %d\n", current_active_duck.shots_remaining_on_this_duck); // Removed

                    if (current_active_duck.shots_remaining_on_this_duck <= 0) {
                        current_active_duck.flew_away = true; // Duck flew away because all shots were missed
                        player_lives--; // Lose a life
                        if (wings_flap_sound) S2D_PlaySound(wings_flap_sound); // Play wings flap sound
                        fly_away_timer = SDL_GetTicks(); // Start "FLY AWAY" text timer
                        // printf("Duck flew away (missed all shots)! Lives left: %d\n", player_lives); // Removed
                    }
                }
            }
        } else if (current_game_state == PAUSED) {
            // Handle clicks on pause menu buttons
            if (isPointInRect(e.x, e.y, resume_button_rect.x, resume_button_rect.y, resume_button_rect.w, resume_button_rect.h)) {
                handle_resume_button_click();
            } else if (isPointInRect(e.x, e.y, home_button_rect.x, home_button_rect.y, home_button_rect.w, home_button_rect.h)) {
                handle_home_button_click();
            } else if (isPointInRect(e.x, e.y, pause_exit_button_rect.x, pause_exit_button_rect.y, pause_exit_button_rect.w, pause_exit_button_rect.h)) {
                handle_exit_button_click();
            }
        } else if (current_game_state == GAME_OVER) {
            // Handle clicks on game over screen buttons
            if (isPointInRect(e.x, e.y, retry_button_rect.x, retry_button_rect.y, retry_button_rect.w, retry_button_rect.h)) {
                handle_retry_button_click();
            } else if (isPointInRect(e.x, e.y, gameover_exit_button_rect.x, gameover_exit_button_rect.y, gameover_exit_button_rect.w, gameover_exit_button_rect.h)) {
                handle_exit_button_click();
            }
        } else if (current_game_state == GAME_WON) {
            // Handle clicks on game won screen buttons
            if (isPointInRect(e.x, e.y, retry_button_rect.x, retry_button_rect.y, retry_button_rect.w, retry_button_rect.h)) {
                handle_play_again_from_won_click(); // "Play Again" button
            } else if (isPointInRect(e.x, e.y, gameover_exit_button_rect.x, gameover_exit_button_rect.y, gameover_exit_button_rect.w, gameover_exit_button_rect.h)) {
                handle_exit_button_click();
            }
        }
        // No clicks handled in ROUND_CLEAR state, it's automatic
    }
}

// Game Initialization: Loads all assets and sets up initial game state
void game_init() {
    srand(time(NULL)); // Initialize random seed for random duck movements/spawns

    // Load common duck sprites (only once, even if multiple ducks are spawned)
    const char *right[] = {"assets/right1.png", "assets/right2.png", "assets/right3.png"};
    const char *upright[] = {"assets/upright1.png", "assets/upright2.png", "assets/upright3.png"};
    const char *up[] = {"assets/up1.png", "assets/up2.png", "assets/up3.png"};
    const char *left[] = {"assets/left1.png", "assets/left2.png", "assets/left3.png"};
    const char *downleft[] = {"assets/downleft1.png", "assets/downleft2.png", "assets/downleft3.png"};
    const char *downright[] = {"assets/downright1.png", "assets/downright2.png", "assets/downright3.png"};
    const char *down[] = {"assets/down1.png", "assets/down2.png", "assets/down3.png"};
    const char *upleft[] = {"assets/upleft1.png", "assets/upleft2.png", "assets/upleft3.png"};
    const char *kill_frame = "assets/kill.png";
    const char *falling1_frame = "assets/falling1.png";
    const char *falling2_frame = "assets/falling2.png";

    // Initialize the single duck instance to load sprites
    initDuck(&current_active_duck, right, upright, up, left, downleft, downright, down, upleft, kill_frame, falling1_frame, falling2_frame);
    current_active_duck.active = false; // Deactivate this initial duck; it was just for loading

    // Load backgrounds (using safe loading functions to report errors)
    background = load_sprite_safe("assets/background.png");
    background2 = load_sprite_safe("assets/background2.png");
    background3 = load_sprite_safe("assets/background3.png");

    // Load UI Images (using safe loading functions)
    start_button_img = load_image_safe("assets/start.png");
    pause_button_img = load_image_safe("assets/pause.png");
    resume_button_img = load_image_safe("assets/resume.png");
    home_button_img = load_image_safe("assets/home.png");
    pause_exit_button_img = load_image_safe("assets/exit2.png");
    gameover_exit_button_img = load_image_safe("assets/exit.png");
    retry_button_img = load_image_safe("assets/retry.png");
    game_over_img = load_image_safe("assets/gameover.png");
    game_won_img = load_image_safe("assets/win.png"); // Load win.png for game won screen

    // Load life heart images (using safe loading functions)
    for (int i = 0; i < 3; i++) {
        life_hearts[i] = load_image_safe("assets/life.png");
    }

    // Set up button rectangles using globally declared dimensions
    start_button_rect = (ButtonRect){0, 0, START_BUTTON_WIDTH, START_BUTTON_HEIGHT};
    pause_button_rect = (ButtonRect){WINDOW_WIDTH - PAUSE_BUTTON_SIZE - 10, 10, PAUSE_BUTTON_SIZE, PAUSE_BUTTON_SIZE};
    resume_button_rect = (ButtonRect){0, 0, RESUME_BUTTON_WIDTH, RESUME_BUTTON_HEIGHT};
    home_button_rect = (ButtonRect){0, 0, HOME_BUTTON_WIDTH, HOME_BUTTON_HEIGHT};
    pause_exit_button_rect = (ButtonRect){0, 0, PAUSE_EXIT_BUTTON_WIDTH, PAUSE_EXIT_BUTTON_HEIGHT};
    retry_button_rect = (ButtonRect){0, 0, RETRY_BUTTON_WIDTH, RETRY_BUTTON_HEIGHT};
    gameover_exit_button_rect = (ButtonRect){0, 0, GAMEOVER_EXIT_BUTTON_WIDTH, GAMEOVER_EXIT_BUTTON_HEIGHT};

    // Load UI text elements and set their properties
    game_over_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", "GAME OVER", 48);
    if (!game_over_text) {
        fprintf(stderr, "Failed to load font for game_over_text: assets/WolfalconRegular-RpjW3.ttf\n");
    }
    game_over_text->color = (S2D_Color){1.0f, 0.0f, 0.0f, 1.0f}; // Red text color

    round_display_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", "Round 1", 24); // Smaller text for in-game display
    if (!round_display_text) {
        fprintf(stderr, "Failed to load font for round_display_text: assets/WolfalconRegular-RpjW3.ttf\n");
    }
    round_display_text->color = (S2D_Color){1.0f, 1.0f, 0.0f, 1.0f}; // Yellow text color

    round_clear_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", "ROUND CLEAR", 48);
    if (!round_clear_text) {
        fprintf(stderr, "Failed to load font for round_clear_text: assets/WolfalconRegular-RpjW3.ttf\n");
    }
    round_clear_text->color = (S2D_Color){0.0f, 1.0f, 0.0f, 1.0f}; // Green text color

    ducks_hit_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", "Ducks Hit: 0 / 10", 36);
    if (!ducks_hit_text) {
        fprintf(stderr, "Failed to load font for ducks_hit_text: assets/WolfalconRegular-RpjW3.ttf\n");
    }
    ducks_hit_text->color = (S2D_Color){1.0f, 1.0f, 1.0f, 1.0f}; // White text color

    fly_away_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", "FLY AWAY", 48);
    if (!fly_away_text) {
        fprintf(stderr, "Failed to load font for fly_away_text: assets/WolfalconRegular-RpjW3.ttf\n");
    }
    fly_away_text->color = (S2D_Color){1.0f, 0.0f, 0.0f, 1.0f}; // Red text color, initially transparent
    fly_away_text->color.a = 0.0f;

    // game_won_text is no longer needed as an S2D_Text object
    // S2D_Text *game_won_text = NULL; // New text for "YOU WON!"
    // if (!game_won_text) {
    //     fprintf(stderr, "Failed to load font for game_won_text: assets/WolfalconRegular-RpjW3.ttf\n");
    // }
    // game_won_text->color = (S2D_Color){0.0f, 1.0f, 0.0f, 1.0f}; // Green text color


    // Load sounds (using safe loading functions)
    banjo_music = load_sound_safe("assets/banjo.mp3");
    background_game_music = load_sound_safe("assets/background.mp3");
    game_over_sound = load_sound_safe("assets/game_over.ogg");
    // drop_fall_sound will be loaded dynamically in updateDuck
    shotgun_shot_sound = load_sound_safe("assets/shotgun_shot.mp3");
    shotgun_reload_sound = load_sound_safe("assets/shotgun_reload.mp3");
    success_sound = load_sound_safe("assets/success.mp3"); // success.mp3 is now for game won
    wings_flap_sound = load_sound_safe("assets/wings_flap.mp3");
    man_grinning_sound = load_sound_safe("assets/man_grinning.mp3"); // man_grinning.mp3 is now for round clear
    game_won_sound = success_sound; // Assign success_sound to game_won_sound


    // Start banjo music when game initializes (for the start screen)
    load_and_play_music(&banjo_music, "assets/banjo.mp3");
}

// Game Cleanup: Frees all allocated resources
void game_cleanup() {
    // Free duck sprites. Only free the sprites loaded by the first duck, as others share pointers.
    Duck *first_duck_for_sprites = &current_active_duck;
    for (int i = 0; i < 3; i++) {
        S2D_FreeSprite(first_duck_for_sprites->right_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->upright_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->up_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->left_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->downleft_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->downright_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->down_frames[i]);
        S2D_FreeSprite(first_duck_for_sprites->upleft_frames[i]);
    }
    S2D_FreeSprite(first_duck_for_sprites->kill_frame);
    S2D_FreeSprite(first_duck_for_sprites->falling1_frame);
    S2D_FreeSprite(first_duck_for_sprites->falling2_frame);

    // Free backgrounds
    S2D_FreeSprite(background);
    S2D_FreeSprite(background2);
    S2D_FreeSprite(background3);

    // Free UI Images
    S2D_FreeImage(start_button_img);
    S2D_FreeImage(pause_button_img);
    S2D_FreeImage(resume_button_img);
    S2D_FreeImage(home_button_img);
    S2D_FreeImage(pause_exit_button_img);
    S2D_FreeImage(gameover_exit_button_img);
    S2D_FreeImage(retry_button_img);
    S2D_FreeImage(game_over_img);
    S2D_FreeImage(game_won_img); // Free game won image

    // Free life hearts images
    for (int i = 0; i < 3; i++) {
        S2D_FreeImage(life_hearts[i]);
    }

    // Free UI text elements
    S2D_FreeText(game_over_text);
    S2D_FreeText(round_display_text);
    S2D_FreeText(round_clear_text);
    S2D_FreeText(ducks_hit_text);
    S2D_FreeText(fly_away_text);
    // S2D_FreeText(game_won_text); // game_won_text is no longer used

    // Free sounds (check for NULL before freeing to prevent errors)
    if (banjo_music) S2D_FreeSound(banjo_music);
    if (background_game_music) S2D_FreeSound(background_game_music);
    if (game_over_sound) S2D_FreeSound(game_over_sound);
    // drop_fall_sound is dynamically managed, so it might be NULL here, but good to check
    if (drop_fall_sound) S2D_FreeSound(drop_fall_sound);
    if (shotgun_shot_sound) S2D_FreeSound(shotgun_shot_sound);
    if (shotgun_reload_sound) S2D_FreeSound(shotgun_reload_sound);
    // Only free success_sound if it's not the same pointer as game_won_sound
    if (success_sound && success_sound != game_won_sound) S2D_FreeSound(success_sound);
    if (wings_flap_sound) S2D_FreeSound(wings_flap_sound);
    if (man_grinning_sound) S2D_FreeSound(man_grinning_sound);
    if (game_won_sound) S2D_FreeSound(game_won_sound); // Free game won sound
}

// Resets the game to its initial start screen state
void reset_game() {
    current_round = 1;
    player_lives = 3;
    ducks_killed_in_round = 0;
    ducks_spawned_this_round = 0;
    ducks_finished_this_round = 0;
    current_active_duck.active = false; // Ensure current duck is inactive
    current_active_duck.falling_sound_active = false; // Reset falling sound flag
    if (drop_fall_sound) { // Ensure falling sound is stopped and freed on reset
        S2D_FreeSound(drop_fall_sound);
        drop_fall_sound = NULL;
    }

    miss_timer = 0;
    shot_feedback_timer = 0;
    fly_away_timer = 0; // Reset fly away text timer
    fly_away_text->color.a = 0.0f; // Hide fly away text

    current_game_state = START_SCREEN; // Return to start screen

    // Reset sound states and restart start screen music
    game_over_sound_played_once = false;
    game_won_sound_played_once = false; // Reset game won sound flag
    round_clear_sound_played_once = false; // Reset round clear sound flag
    shotgun_reload_timer = 0;
    stop_and_free_music(&background_game_music); // Stop any game music
    load_and_play_music(&banjo_music, "assets/banjo.mp3"); // Restart banjo music
}

// Configures and initiates a new round of the game
void start_round(int round_num) {
    current_round = round_num;
    player_lives = 3; // Reset lives at the start of each round
    ducks_killed_in_round = 0;
    ducks_spawned_this_round = 0;
    ducks_finished_this_round = 0;
    current_active_duck.active = false; // Ensure no duck is active initially
    if (drop_fall_sound) { // Ensure falling sound is stopped and freed when starting a new round
        S2D_FreeSound(drop_fall_sound);
        drop_fall_sound = NULL;
    }

    // Start background game music
    stop_and_free_music(&banjo_music); // Stop start screen music
    load_and_play_music(&background_game_music, "assets/background.mp3"); // Start game music

    current_game_state = PLAYING; // Directly transition to PLAYING
    spawn_new_duck(); // Spawn the first duck of the round
    round_clear_sound_played_once = false; // Ensure sound can play for the new round
}


// --- UI Drawing Functions ---

// Draws the initial start screen with the start button
void draw_start_screen() {
    // Center the start button on the screen
    start_button_rect.x = (WINDOW_WIDTH - start_button_rect.w) / 2;
    start_button_rect.y = (WINDOW_HEIGHT - start_button_rect.h) / 2;

    if (start_button_img) {
        start_button_img->x = start_button_rect.x;
        start_button_img->y = start_button_rect.y;
        start_button_img->width = start_button_rect.w; // Use globally defined width
        start_button_img->height = start_button_rect.h; // Use globally defined height
        S2D_DrawImage(start_button_img); // Draw the start button image
    } else {
        // Fallback: draw a colored quad if image fails to load
        S2D_DrawQuad(start_button_rect.x, start_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f,
                     start_button_rect.x + start_button_rect.w, start_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f,
                     start_button_rect.x + start_button_rect.w, start_button_rect.y + start_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f,
                     start_button_rect.x, start_button_rect.y + start_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f);
    }
}

// Draws the UI elements during active gameplay (pause button, lives)
void draw_playing_ui() {
    // Draw pause button at top-right, using PAUSE_BUTTON_SIZE
    if (pause_button_img) {
        pause_button_img->x = pause_button_rect.x;
        pause_button_img->y = pause_button_rect.y;
        pause_button_img->width = pause_button_rect.w; // Use globally defined size
        pause_button_img->height = pause_button_rect.h; // Use globally defined size
        S2D_DrawImage(pause_button_img);
    } else {
        // Fallback: draw a colored quad
        S2D_DrawQuad(pause_button_rect.x, pause_button_rect.y, 0.7f, 0.7f, 0.2f, 1.0f,
                     pause_button_rect.x + pause_button_rect.w, pause_button_rect.y, 0.7f, 0.7f, 0.2f, 1.0f,
                     pause_button_rect.x + pause_button_rect.w, pause_button_rect.y + pause_button_rect.h, 0.7f, 0.7f, 0.2f, 1.0f,
                     pause_button_rect.x, pause_button_rect.y + pause_button_rect.h, 0.7f, 0.7f, 0.2f, 1.0f);
    }

    // Draw player lives (heart icons) at top-left
    // Note: player_lives now represents total game lives, not shots per duck
    for (int i = 0; i < player_lives; i++) {
        // Draw the heart icon
        // Position with padding and spacing
        int heart_x = 10 + (i * (LIFE_HEART_SIZE + 5));
        int heart_y = 10;
        if (life_hearts[i]) {
            life_hearts[i]->x = heart_x;
            life_hearts[i]->y = heart_y;
            life_hearts[i]->width = LIFE_HEART_SIZE;
            life_hearts[i]->height = LIFE_HEART_SIZE;
            S2D_DrawImage(life_hearts[i]);
        }
    }

    // Display shots remaining for the current duck
    char shots_buffer[20];
    snprintf(shots_buffer, sizeof(shots_buffer), "Shots: %d", current_active_duck.shots_remaining_on_this_duck);
    S2D_Text *shots_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", shots_buffer, 24); // Create temporary text
    if (!shots_text) { // Added error check for shots_text
        fprintf(stderr, "Failed to load font for shots_text: assets/WolfalconRegular-RpjW3.ttf\n");
    }
    shots_text->color = (S2D_Color){1.0f, 1.0f, 1.0f, 1.0f}; // White text
    shots_text->x = 10; // Left side, below lives
    shots_text->y = 10 + LIFE_HEART_SIZE + 5;
    S2D_DrawText(shots_text);
    S2D_FreeText(shots_text); // Free temporary text after drawing
}

// Draws the pause menu overlay with resume, home, and exit buttons
void draw_pause_menu() {
    // Dim the background to highlight the menu
    S2D_DrawQuad(0, 0, 0.0f, 0.0f, 0.0f, 0.5f, WINDOW_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.5f,
                 WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f, 0, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f);

    // Position buttons relative to the center of the screen
    int center_x = WINDOW_WIDTH / 2;
    int start_y = WINDOW_HEIGHT / 2 - (RESUME_BUTTON_HEIGHT * 3 + BUTTON_MARGIN * 2) / 2; // Calculate starting Y for centered block of buttons

    resume_button_rect.x = center_x - RESUME_BUTTON_WIDTH / 2;
    resume_button_rect.y = start_y;
    home_button_rect.x = center_x - HOME_BUTTON_WIDTH / 2;
    home_button_rect.y = start_y + RESUME_BUTTON_HEIGHT + BUTTON_MARGIN;
    pause_exit_button_rect.x = center_x - PAUSE_EXIT_BUTTON_WIDTH / 2;
    pause_exit_button_rect.y = start_y + 2 * (RESUME_BUTTON_HEIGHT + BUTTON_MARGIN);

    // Draw buttons using globally defined BUTTON_WIDTH and BUTTON_HEIGHT
    if (resume_button_img) {
        resume_button_img->x = resume_button_rect.x; resume_button_img->y = resume_button_rect.y;
        resume_button_img->width = resume_button_rect.w; resume_button_img->height = resume_button_rect.h;
        S2D_DrawImage(resume_button_img);
    } else { S2D_DrawQuad(resume_button_rect.x, resume_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f, resume_button_rect.x + resume_button_rect.w, resume_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f, resume_button_rect.x + resume_button_rect.w, resume_button_rect.y + resume_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f, resume_button_rect.x, resume_button_rect.y + resume_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f); }

    if (home_button_img) {
        home_button_img->x = home_button_rect.x; home_button_img->y = home_button_rect.y;
        home_button_img->width = home_button_rect.w; home_button_img->height = home_button_rect.h;
        S2D_DrawImage(home_button_img);
    } else { S2D_DrawQuad(home_button_rect.x, home_button_rect.y, 0.5f, 0.5f, 0.5f, 1.0f, home_button_rect.x + home_button_rect.w, home_button_rect.y, 0.5f, 0.5f, 0.5f, 1.0f, home_button_rect.x + home_button_rect.w, home_button_rect.y + home_button_rect.h, 0.5f, 0.5f, 0.5f, 1.0f, home_button_rect.x, home_button_rect.y + home_button_rect.h, 0.5f, 0.5f, 0.5f, 1.0f); }

    if (pause_exit_button_img) {
        pause_exit_button_img->x = pause_exit_button_rect.x; pause_exit_button_img->y = pause_exit_button_rect.y;
        pause_exit_button_img->width = pause_exit_button_rect.w; pause_exit_button_img->height = pause_exit_button_rect.h;
        S2D_DrawImage(pause_exit_button_img);
    } else { S2D_DrawQuad(pause_exit_button_rect.x, pause_exit_button_rect.y, 1.0f, 0.0f, 0.0f, 1.0f, pause_exit_button_rect.x + pause_exit_button_rect.w, pause_exit_button_rect.y, 1.0f, 0.0f, 0.0f, 1.0f, pause_exit_button_rect.x + pause_exit_button_rect.w, pause_exit_button_rect.y + pause_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f, pause_exit_button_rect.x, pause_exit_button_rect.y + pause_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f); }
}

// Draws the new round clear screen
void draw_round_clear_screen() {
    // Dim the background
    S2D_DrawQuad(0, 0, 0.0f, 0.0f, 0.0f, 0.7f, WINDOW_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.7f,
                 WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f, 0, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f);

    // Draw "ROUND CLEAR" text
    round_clear_text->x = (WINDOW_WIDTH - round_clear_text->width) / 2;
    round_clear_text->y = (WINDOW_HEIGHT / 2) - round_clear_text->height; // Position above ducks hit text
    S2D_DrawText(round_clear_text);

    // Draw "Ducks Hit: X / 10" text
    ducks_hit_text->x = (WINDOW_WIDTH - ducks_hit_text->width) / 2;
    ducks_hit_text->y = (WINDOW_HEIGHT / 2) + 10; // Position below round clear text
    S2D_DrawText(ducks_hit_text);
}


// Draws the game over screen with retry and exit buttons
void draw_game_over_screen() {
    // Dim the background for game over screen
    S2D_DrawQuad(0, 0, 0.0f, 0.0f, 0.0f, 0.7f, WINDOW_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.7f,
                 WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f, 0, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f);

    // Draw game over image, using globally defined render dimensions
    if (game_over_img) {
        game_over_img->width = GAMEOVER_IMAGE_RENDER_WIDTH;
        game_over_img->height = GAMEOVER_IMAGE_RENDER_HEIGHT;
        game_over_img->x = (WINDOW_WIDTH - game_over_img->width) / 2;
        game_over_img->y = WINDOW_HEIGHT / 3 - game_over_img->height / 2;
        S2D_DrawImage(game_over_img);
    } else {
        // Fallback: draw "GAME OVER" text
        game_over_text->x = (WINDOW_WIDTH - game_over_text->width) / 2;
        game_over_text->y = WINDOW_HEIGHT / 3 - game_over_text->height / 2;
        S2D_DrawText(game_over_text);
    }

    // Position buttons below the game over image/text
    int center_x = WINDOW_WIDTH / 2;
    int start_y = WINDOW_HEIGHT / 2 + BUTTON_MARGIN; // Offset from middle for buttons

    retry_button_rect.x = center_x - RETRY_BUTTON_WIDTH / 2;
    retry_button_rect.y = start_y;
    gameover_exit_button_rect.x = center_x - GAMEOVER_EXIT_BUTTON_WIDTH / 2;
    gameover_exit_button_rect.y = start_y + RETRY_BUTTON_HEIGHT + BUTTON_MARGIN;

    // Draw buttons
    if (retry_button_img) {
        retry_button_img->x = retry_button_rect.x; retry_button_img->y = retry_button_rect.y;
        retry_button_img->width = retry_button_rect.w; retry_button_img->height = retry_button_rect.h;
        S2D_DrawImage(retry_button_img);
    } else { S2D_DrawQuad(retry_button_rect.x, retry_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f, retry_button_rect.x + retry_button_rect.w, retry_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f, retry_button_rect.x + retry_button_rect.w, retry_button_rect.y + retry_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f, retry_button_rect.x, retry_button_rect.y + retry_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f); }

    if (gameover_exit_button_img) {
        gameover_exit_button_img->x = gameover_exit_button_rect.x; gameover_exit_button_img->y = gameover_exit_button_rect.y;
        gameover_exit_button_img->width = gameover_exit_button_rect.w; gameover_exit_button_img->height = gameover_exit_button_rect.h;
        S2D_DrawImage(gameover_exit_button_img);
    } else { S2D_DrawQuad(gameover_exit_button_rect.x, gameover_exit_button_rect.y, 1.0f, 0.0f, 0.0f, 1.0f, gameover_exit_button_rect.x + gameover_exit_button_rect.w, gameover_exit_button_rect.y, 1.0f, 0.0f, 0.0f, 1.0f, gameover_exit_button_rect.x + gameover_exit_button_rect.w, gameover_exit_button_rect.y + gameover_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f, gameover_exit_button_rect.x, gameover_exit_button_rect.y + gameover_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f); }
}

// New: Draws the game won screen with play again and exit buttons
void draw_game_won_screen() {
    // Dim the background
    S2D_DrawQuad(0, 0, 0.0f, 0.0f, 0.0f, 0.7f, WINDOW_WIDTH, 0, 0.0f, 0.0f, 0.0f, 0.7f,
                 WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f, 0, WINDOW_HEIGHT, 0.0f, 0.0f, 0.0f, 0.7f);

    // Draw game won image, using globally defined render dimensions
    if (game_won_img) {
        game_won_img->width = GAMEWON_IMAGE_RENDER_WIDTH;
        game_won_img->height = GAMEWON_IMAGE_RENDER_HEIGHT;
        game_won_img->x = (WINDOW_WIDTH - game_won_img->width) / 2;
        game_won_img->y = WINDOW_HEIGHT / 3 - game_won_img->height / 2;
        S2D_DrawImage(game_won_img);
    } else {
        // Fallback: draw "YOU WON!" text if image fails to load
        // This text element is no longer needed as a global variable, but can be created temporarily for fallback
        S2D_Text *fallback_text = S2D_CreateText("assets/WolfalconRegular-RpjW3.ttf", "YOU WON!", 48);
        if (fallback_text) {
            fallback_text->color = (S2D_Color){0.0f, 1.0f, 0.0f, 1.0f}; // Green text color
            fallback_text->x = (WINDOW_WIDTH - fallback_text->width) / 2;
            fallback_text->y = WINDOW_HEIGHT / 3 - fallback_text->height / 2;
            S2D_DrawText(fallback_text);
            S2D_FreeText(fallback_text); // Free temporary text
        }
    }

    // Position buttons below the image
    int center_x = WINDOW_WIDTH / 2;
    int start_y = WINDOW_HEIGHT / 2 + BUTTON_MARGIN; // Offset from middle for buttons

    retry_button_rect.x = center_x - RETRY_BUTTON_WIDTH / 2; // Re-using retry button for "Play Again"
    retry_button_rect.y = start_y;
    gameover_exit_button_rect.x = center_x - GAMEOVER_EXIT_BUTTON_WIDTH / 2; // Re-using exit button
    gameover_exit_button_rect.y = start_y + RETRY_BUTTON_HEIGHT + BUTTON_MARGIN;

    // Draw "Play Again" button (using retry_button_img)
    if (retry_button_img) {
        retry_button_img->x = retry_button_rect.x; retry_button_img->y = retry_button_rect.y;
        retry_button_img->width = retry_button_rect.w; retry_button_img->height = retry_button_rect.h;
        S2D_DrawImage(retry_button_img);
    } else { S2D_DrawQuad(retry_button_rect.x, retry_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f, retry_button_rect.x + retry_button_rect.w, retry_button_rect.y, 0.2f, 0.7f, 0.2f, 1.0f, retry_button_rect.x + retry_button_rect.w, retry_button_rect.y + retry_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f, retry_button_rect.x, retry_button_rect.y + retry_button_rect.h, 0.2f, 0.7f, 0.2f, 1.0f); }

    // Draw "Exit" button (using gameover_exit_button_img)
    if (gameover_exit_button_img) {
        gameover_exit_button_img->x = gameover_exit_button_rect.x; gameover_exit_button_img->y = gameover_exit_button_rect.y;
        gameover_exit_button_img->width = gameover_exit_button_rect.w; gameover_exit_button_img->height = gameover_exit_button_rect.h;
        S2D_DrawImage(gameover_exit_button_img);
    } else { S2D_DrawQuad(gameover_exit_button_rect.x, gameover_exit_button_rect.y, 1.0f, 0.0f, 0.0f, 1.0f, gameover_exit_button_rect.x + gameover_exit_button_rect.w, gameover_exit_button_rect.y, 1.0f, 0.0f, 0.0f, 1.0f, gameover_exit_button_rect.x + gameover_exit_button_rect.w, gameover_exit_button_rect.y + gameover_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f, gameover_exit_button_rect.x, gameover_exit_button_rect.y + gameover_exit_button_rect.h, 1.0f, 0.0f, 0.0f, 1.0f); }
}


// --- Button Click Handlers ---

void handle_start_button_click() {
    start_round(1); // Begin the game with Round 1
}

void handle_pause_button_click() {
    current_game_state = PAUSED; // Pause the game
    stop_and_free_music(&background_game_music); // Stop background music when paused
}

void handle_resume_button_click() {
    current_game_state = PLAYING; // Resume game
    load_and_play_music(&background_game_music, "assets/background.mp3"); // Resume background music
}

void handle_home_button_click() {
    reset_game(); // Return to the start screen
}

void handle_exit_button_click() {
    S2D_Quit(); // Quit Simple2D framework
    exit(0);    // Exit the program
}

void handle_retry_button_click() {
    // Reset relevant game state for a retry of the current round
    ducks_killed_in_round = 0;
    ducks_spawned_this_round = 0;
    ducks_finished_this_round = 0;
    player_lives = 3; // Reset lives for the retry
    current_active_duck.active = false; // Ensure no duck is active before spawning new one
    current_active_duck.falling_sound_active = false; // Reset falling sound flag
    if (drop_fall_sound) { // Ensure falling sound is stopped and freed on retry
        S2D_FreeSound(drop_fall_sound);
        drop_fall_sound = NULL;
    }

    current_game_state = PLAYING; // Transition to playing
    load_and_play_music(&background_game_music, "assets/background.mp3"); // Start game music
    spawn_new_duck(); // Spawn the first duck for the retried round
    game_over_sound_played_once = false; // Allow game over sound to play again if needed
    game_won_sound_played_once = false; // Reset game won sound flag on retry
    round_clear_sound_played_once = false; // Reset round clear sound flag on retry
}

// New: Handles "Play Again" button click from Game Won screen
void handle_play_again_from_won_click() {
    reset_game(); // Resets the game to start screen, which is suitable for playing again from round 1
}

// --- Safe Asset Loading Functions ---
S2D_Sound* load_sound_safe(const char* path) {
    S2D_Sound* sound = S2D_CreateSound(path);
    if (!sound) {
        fprintf(stderr, "Failed to load sound: %s\n", path);
    }
    return sound;
}

S2D_Sprite* load_sprite_safe(const char* path) {
    S2D_Sprite* sprite = S2D_CreateSprite(path);
    if (!sprite) {
        fprintf(stderr, "Failed to load sprite: %s\n", path);
    }
    return sprite;
}

S2D_Image* load_image_safe(const char* path) {
    S2D_Image* image = S2D_CreateImage(path);
    if (!image) {
        fprintf(stderr, "Failed to load image: %s\n", path);
    }
    return image;
}

// --- Music management functions ---
S2D_Sound* load_and_play_music(S2D_Sound **music_ptr, const char* path) {
    if (*music_ptr) {
        S2D_FreeSound(*music_ptr); // Free existing music if any
        *music_ptr = NULL;
    }
    *music_ptr = load_sound_safe(path); // Load new music using safe function
    if (*music_ptr) {
        S2D_PlaySound(*music_ptr); // Play the newly loaded music
    }
    return *music_ptr;
}

void stop_and_free_music(S2D_Sound **music_ptr) {
    if (*music_ptr) {
        S2D_FreeSound(*music_ptr); // Free the sound resource (this effectively stops it)
        *music_ptr = NULL; // Clear the pointer
    }
}


// --- Main Program Entry ---
int main() {
    // Create the game window with specified dimensions, update/render functions, and resizable flag.
    window = S2D_CreateWindow("Duck Hunt", WINDOW_WIDTH, WINDOW_HEIGHT, update, render, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Failed to create Simple2D window.\n");
        return 1;
    }

    window->on_mouse = onMouse; // Register the mouse event handler

    game_init(); // Initialize all game assets and set initial state

    S2D_Show(window); // Start the Simple2D game loop

    game_cleanup(); // Clean up all game assets and resources after the loop ends
    S2D_FreeWindow(window); // Free the window resource
    S2D_Quit(); // Quit Simple2D framework
    return 0;
}
