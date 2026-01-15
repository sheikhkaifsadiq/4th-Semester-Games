#include <simple2d.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // Required for exit()
#include <time.h> // For clock() and CLOCKS_PER_SEC
#include <math.h> // For fmodf, fmaxf, fminf, sinf
#include <stdio.h> // For snprintf

// Constants
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 720
#define PLAYER_HEIGHT 80
#define PLAYER_WIDTH 55
#define BOTTOM_MARGIN 20
#define LEFT_YELLOW_LINE_X 69
#define RIGHT_YELLOW_LINE_X 567
#define HORIZONTAL_MARGIN 5
#define LANE_1_CENTER 132
#define LANE_2_CENTER 258
#define LANE_3_CENTER 382
#define LANE_4_CENTER 506

// Difficulty constants
#define MAX_ENEMIES_INITIAL 6      // Initial maximum number of enemies
#define MIN_ACTIVE_ENEMIES 4      // Minimum number of active enemies allowed
#define MAX_OBSTACLES_COUNT 2      // Fixed number of obstacles
#define ENEMY_SPEED_INCREASE_INTERVAL 10.0f // Time in seconds to increase enemy speed
#define ENEMY_SPEED_INCREASE_AMOUNT 0.5f    // Amount to increase enemy speed by
#define ENEMY_COUNT_DECREASE_INTERVAL 10.0f // Time in seconds to decrease max enemies

#define ENEMY_SPEED_MIN_INITIAL 3.5f // Initial min enemy speed
#define ENEMY_SPEED_MAX_INITIAL 8.0f // Initial max enemy speed
#define SCROLL_SPEED_INITIAL 3.0f    // Initial road scroll speed

// Button dimensions (used for pause menu buttons)
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 100

// Separate dimensions for the Start Button
#define START_BUTTON_WIDTH 250 // Larger width for the start button
#define START_BUTTON_HEIGHT 150 // Larger height for the start button

#define MOUSE_LEFT 1
// Defined these to prevent implicit declarations if not defined elsewhere
#define ENEMY_WIDTH PLAYER_WIDTH
#define ENEMY_HEIGHT PLAYER_HEIGHT // Assuming enemies are similar height to player
#define OBSTACLE_WIDTH 60 // Example size for obstacles
#define OBSTACLE_HEIGHT 60 // Example size for obstacles

// Constants for pause menu
#define PAUSE_BUTTON_SIZE 50 // Size for the square pause button in top-right
#define PAUSE_MENU_BUTTON_SPACING 30 // Vertical spacing between pause menu buttons

// Constants for game over screen
#define GAMEOVER_IMAGE_WIDTH 300
#define GAMEOVER_IMAGE_HEIGHT 150
#define GAMEOVER_BUTTON_HORIZONTAL_SPACING 40 // Spacing between Retry and Exit buttons
#define GAMEOVER_BUTTON_VERTICAL_OFFSET 30 // Vertical offset from game over image to buttons

// Constants for separate Retry and Exit button sizes
#define RETRY_BUTTON_WIDTH BUTTON_WIDTH
#define RETRY_BUTTON_HEIGHT BUTTON_HEIGHT
#define EXIT_BUTTON_WIDTH BUTTON_WIDTH
#define EXIT_BUTTON_HEIGHT BUTTON_HEIGHT

// Constant for score image (UPDATED)
#define SCORE_IMAGE_WIDTH 250
#define SCORE_IMAGE_HEIGHT 50

// Global constant for text font size on the score image
#define SCORE_TEXT_FONT_SIZE 18

// Global constants for text positioning on score image
#define SCORE_LABEL_HORIZONTAL_OFFSET 35   // Offset for "Score" label from left edge of score image (moved left)
#define SCORE_VALUE_HORIZONTAL_OFFSET 40   // Offset for score value from right edge of score image (moved left)
#define SCORE_VALUE_VERTICAL_OFFSET -1     // Vertical offset for score value (moved up)
#define TIMER_HORIZONTAL_OFFSET -5         // Horizontal offset for timer from center (moved left)

// Constants for final score image and text positioning on game over screen
#define FINAL_SCORE_IMAGE_WIDTH 200 // Adjust as needed for your finalscore.png
#define FINAL_SCORE_IMAGE_HEIGHT 150 // Adjust as needed for your finalscore.png
#define FINAL_SCORE_CURRENT_OFFSET_X 100 // X offset from finalscore.png's top-left for current score text
#define FINAL_SCORE_CURRENT_OFFSET_Y 50  // Y offset from finalscore.png's top-left for current score text
#define FINAL_SCORE_HIGH_SCORE_OFFSET_X 110 // X offset for high score text
#define FINAL_SCORE_HIGH_SCORE_OFFSET_Y 110 // Y offset for high score text (relative to star on image)
#define FINAL_SCORE_TEXT_FONT_SIZE 24 // Font size for final and high scores (can be different from SCORE_TEXT_FONT_SIZE)

// Power-up Constants
#define POWERUP_WIDTH 40
#define POWERUP_HEIGHT 40
#define LIFE_POWERUP_SPAWN_INTERVAL 20.0f // Time in seconds between life power-up spawns
#define SCORE_POPUP_DURATION 2.0f    // Duration of the +500 score animation
#define SCORE_POPUP_SPEED 50.0f      // Upward movement speed of the +500 score
// Removed SCORE_POPUP_WAVE_AMPLITUDE and SCORE_POPUP_WAVE_FREQUENCY as per user request

// NEW: Blinking effect constants
#define HEART_UI_BLINK_DURATION 3.0f // Duration for heart UI blink after pickup
#define SCORE_TEXT_BLINK_DURATION 1.0f // Duration for score text blink after power-up
#define SCORE_TEXT_BLINK_INTERVAL 0.2f // Interval for score text blinking (on/off phase)

// NEW: Speed-up power-up constants
#define SPEEDUP_WIDTH 40 // Explicitly set to 40, same as POWERUP_WIDTH
#define SPEEDUP_HEIGHT 40 // Explicitly set to 40, same as POWERUP_HEIGHT
#define SPEEDUP_POWERUP_SPAWN_INTERVAL 20.0f // Time in seconds between speed-up power-up spawns
#define PLAYER_SPEEDUP_DURATION 10.0f // Duration of the speed boost effect (for speed and size change)
#define PLAYER_HEART_INVULNERABILITY_AND_BLINK_DURATION 5.0f // Duration for heart invulnerability and blinking
#define PLAYER_SPEEDUP_INVULNERABILITY_AND_BLINK_DURATION 15.0f // Duration for speedup invulnerability and blinking


// Total number of game objects (enemies + obstacles + 1 for heart + 1 for speedup)
#define MAX_GAME_OBJECTS (MAX_ENEMIES_INITIAL + MAX_OBSTACLES_COUNT + 2)
#define HEART_POWERUP_OBJECT_ARRAY_INDEX (MAX_ENEMIES_INITIAL + MAX_OBSTACLES_COUNT) // Dedicated index for the heart power-up
#define SPEEDUP_POWERUP_OBJECT_ARRAY_INDEX (MAX_ENEMIES_INITIAL + MAX_OBSTACLES_COUNT + 1) // Dedicated index for the speed-up power-up


// Game States
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED, // New state for when the game is paused
    STATE_GAME_OVER_DELAY, // New state for the 3-second delay before actual game over
    STATE_GAME_OVER
} GameState;

// Enum for different types of game objects
typedef enum {
    TYPE_ENEMY,
    TYPE_OBSTACLE,
    TYPE_POWERUP, // Heart power-up
    TYPE_SPEEDUP  // Speed-up power-up
} GameObjectType;

// Function Prototypes (Declarations)
void draw_quad(float x, float y, float w, float h, float r, float g, float b, float a);
S2D_Sprite* load_sprite_safe(const char* path);
S2D_Text* load_text_safe(const char* font_path, const char* text_str, int size);
S2D_Sound* load_sound_safe(const char* path); // Added prototype for sound loading
void reset_game();
void on_mouse(S2D_Event e);
void render();
void update();
void on_key(S2D_Event e);
int read_high_score();
void write_high_score(int score);
int get_player_lane(); // Helper to get player's current lane

// Removed prototypes for Simple2D sound control functions that caused undefined references.
// We will now free and re-load sounds to achieve similar behavior.
void S2D_PlaySound(S2D_Sound *s); // This one is generally available.


// Helper function to draw a rectangle with alpha
void draw_quad(float x, float y, float w, float h, float r, float g, float b, float a) {
    S2D_DrawQuad(x, y, r, g, b, a, x + w, y, r, g, b, a, x + w, y + h, r, g, b, a, x, y + h, r, g, b, a);
}

// Global variables
float road_offset = 0.0f;
S2D_Sprite *player;
S2D_Sprite *enemies_sprites[MAX_ENEMIES_INITIAL]; // Renamed for clarity
S2D_Sprite *obstacles_sprites[MAX_OBSTACLES_COUNT]; // Renamed for clarity
S2D_Sprite *powerup_sprites[1]; // Only one power-up sprite (heart)
S2D_Sprite *speedup_sprites[1]; // NEW: Only one speed-up power-up sprite

S2D_Text *game_over_text;
S2D_Sprite *start_button_sprite;

// Global variables for pause menu buttons
S2D_Sprite *pause_button_sprite;
S2D_Sprite *resume_button_sprite;
S2D_Sprite *home_button_sprite;
S2D_Sprite *exit2_button_sprite;

// New global variables for game over screen
S2D_Sprite *game_over_image_sprite;
S2D_Sprite *retry_button_sprite;
S2D_Sprite *exit_button_sprite;

// New global variable for score image
S2D_Sprite *score_image_sprite;

// New global variables for score and timer text
S2D_Text *score_label_text;
S2D_Text *current_score_value_text;
S2D_Text *timer_text;
char timer_buffer[16]; // Buffer for mm:ss timer string

int current_score = 0;
int high_score = 0;
char score_buffer[32]; // Buffer for current score string

float current_enemy_speed_min = ENEMY_SPEED_MIN_INITIAL;
float current_enemy_speed_max = ENEMY_SPEED_MAX_INITIAL;
float current_scroll_speed = SCROLL_SPEED_INITIAL;
float difficulty_increase_rate = 0.05f;

// New global variables for difficulty scaling
int max_active_enemies_allowed = MAX_ENEMIES_INITIAL;
clock_t last_speed_increase_time;
clock_t last_enemy_count_decrease_time;
clock_t game_start_time; // To track the start of the game for timer

// Global variables for final score image and text
S2D_Sprite *final_score_image_sprite;
S2D_Text *final_current_score_text;
S2D_Text *final_high_score_text;
char final_current_score_buffer[32];
char final_high_score_buffer[32];

// Power-up related global variables
S2D_Sprite *life_ui_sprite; // For displaying the extra life icon in the UI
bool has_extra_life = false; // Indicates if the player currently has an extra life
float last_powerup_spawn_time; // To control heart power-up spawn rate

// NEW: Speed-up power-up related global variables
S2D_Sprite *speedup_ui_sprite; // For displaying the speed-up icon in the UI
bool is_speed_boost_active = false;
float speed_boost_start_time = 0.0f;
float last_speedup_spawn_time; // To control speed-up power-up spawn rate
float original_player_speed_val = 7.0f; // Store original player speed
float player_current_speed; // Player's current speed (can be boosted)
int original_score_increase_rate_val = 100; // Store original score increase rate
int current_score_increase_rate; // Current score increase rate (can be boosted)
float game_speed_multiplier = 1.0f; // NEW: Multiplier for overall game speed


// Score pop-up animation variables
S2D_Text *score_popup_text;
bool score_popup_active = false;
float score_popup_start_time;
float score_popup_x, score_popup_y; // Current position of the +500 text
float initial_score_popup_x, initial_score_popup_y; // Starting position for the animation
char score_popup_buffer[16]; // Buffer for "+500" text

// NEW: Blinking effect global variables
float heart_ui_blink_start_time = 0.0f;
bool heart_ui_blink_active = false;

// Consolidated player blinking and invulnerability
bool player_is_blinking = false;
float player_blink_start_time = 0.0f;
float player_current_blink_duration = 0.0f; // Dynamic duration for blinking
bool player_is_invulnerable = false; // Flag for invulnerability

float score_text_blink_start_time = 0.0f; // For score text blinking
bool score_text_blink_active = false;     // For score text blinking

// NEW: Game over delay variables
float game_over_delay_start_time = 0.0f;
bool game_over_sound_played = false;

// NEW: Sound variables
S2D_Sound *background_music = NULL; // Initialize to NULL to indicate no sound loaded
S2D_Sound *car_car_crash_sound;
S2D_Sound *car_obstacle_crash_sound;
S2D_Sound *gameover_sound;
S2D_Sound *car_passed_sound;
S2D_Sound *heart_pickup_sound;
S2D_Sound *heart_used_sound;
S2D_Sound *car_fasting_sound;
S2D_Sound *car_slowing_sound;
bool slowing_sound_played = false; // To ensure car_slowing.wav plays only once per boost

bool move_left = false;
bool move_right = false;
GameState current_game_state = STATE_MENU;
float player_x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
float player_y = SCREEN_HEIGHT - PLAYER_HEIGHT - BOTTOM_MARGIN;
// const float PLAYER_SPEED = 7.0f; // Now a global variable `player_current_speed`
int active_enemy_count = 0;

// Animation variables for start button
float start_button_animation_time = 0.0f;

// For manual delta time calculation
static clock_t last_frame_time;

// Structure for game object and lane data
typedef struct {
    float x;
    float y;
    float speed;
    int lane;
    bool active;
    GameObjectType type; // Type of game object (enemy, obstacle, powerup, speedup)
    int sprite_index;    // Index within its type's sprite array
    bool passed_by_sound_played; // NEW: Flag for car_passed.mp3
} GameObject;

// Combined array for all game objects (enemies + obstacles + heart + speedup)
GameObject game_objects[MAX_GAME_OBJECTS];

typedef struct {
    bool is_occupied; // True if any active game object is in this lane
    bool has_obstacle; // Specific flag for obstacles (might be redundant with is_occupied)
    float last_obstacle_time; // Time when last obstacle was spawned in this lane
    float last_enemy_speed; // Last enemy speed in this lane (currently unused)
} Lane;

Lane lanes[4];

// Array of lane centers
const float LANE_CENTERS[] = {LANE_1_CENTER, LANE_2_CENTER, LANE_3_CENTER, LANE_4_CENTER};

// Function to initialize game object
void init_game_object(int index, int lane, float y, GameObjectType type, int sprite_idx) {
    if (index < 0 || index >= MAX_GAME_OBJECTS) return;

    game_objects[index].lane = lane;
    game_objects[index].y = y;
    game_objects[index].active = true;
    game_objects[index].type = type;
    game_objects[index].sprite_index = sprite_idx;
    game_objects[index].passed_by_sound_played = false; // Initialize sound flag

    float object_width;
    float object_height; // Added for clarity, though currently same as width for powerups
    float object_speed; // Declared object_speed here

    switch (type) {
        case TYPE_ENEMY:
            object_width = ENEMY_WIDTH;
            object_height = ENEMY_HEIGHT;
            object_speed = current_enemy_speed_min + (rand() / (float)RAND_MAX) * (current_enemy_speed_max - current_enemy_speed_min);
            active_enemy_count++;
            lanes[lane - 1].last_enemy_speed = object_speed;
            break;
        case TYPE_OBSTACLE:
            object_width = OBSTACLE_WIDTH;
            object_height = OBSTACLE_HEIGHT;
            object_speed = current_scroll_speed; // Obstacles move with road speed
            lanes[lane - 1].has_obstacle = true;
            lanes[lane - 1].last_obstacle_time = (float)clock() / CLOCKS_PER_SEC; // Update obstacle spawn time
            break;
        case TYPE_POWERUP: // Heart
            object_width = POWERUP_WIDTH;
            object_height = POWERUP_HEIGHT;
            object_speed = current_scroll_speed; // Power-ups move with road speed
            break;
        case TYPE_SPEEDUP: // Speed-up
            object_width = SPEEDUP_WIDTH;
            object_height = SPEEDUP_HEIGHT;
            object_speed = current_scroll_speed; // Power-ups move with road speed
            break;
    }
    game_objects[index].x = LANE_CENTERS[lane - 1] - object_width / 2;
    game_objects[index].speed = object_speed;
    lanes[lane - 1].is_occupied = true; // Mark lane as occupied by the new object
}

// Function to check collision
bool check_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// Helper function to get player's current lane
int get_player_lane() {
    float player_center_x = player_x + PLAYER_WIDTH / 2;
    if (player_center_x < LANE_1_CENTER + (LANE_2_CENTER - LANE_1_CENTER) / 2) return 1;
    if (player_center_x < LANE_2_CENTER + (LANE_3_CENTER - LANE_2_CENTER) / 2) return 3; // Corrected lane 3 calculation
    if (player_center_x < LANE_3_CENTER + (LANE_4_CENTER - LANE_3_CENTER) / 2) return 4; // Corrected lane 4 calculation
    return 2; // Default to 2nd lane if not in other lanes
}


// Function to reset the game
void reset_game() {
    current_game_state = STATE_PLAYING;
    player_x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    // Deactivate all game objects
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        game_objects[i].active = false;
        game_objects[i].passed_by_sound_played = false; // Reset sound flag
    }
    // Reset all lane states
    for (int i = 0; i < 4; i++) {
        lanes[i].is_occupied = false;
        lanes[i].has_obstacle = false;
        lanes[i].last_obstacle_time = 0.0f;
        lanes[i].last_enemy_speed = 0.0f;
    }
    active_enemy_count = 0;
    road_offset = 0.0f;

    // Reset score and difficulty
    current_score = 0;
    has_extra_life = false; // Player starts without an extra life
    current_enemy_speed_min = ENEMY_SPEED_MIN_INITIAL;
    current_enemy_speed_max = ENEMY_SPEED_MAX_INITIAL;
    current_scroll_speed = SCROLL_SPEED_INITIAL;

    // Reset speed boost state and player properties
    is_speed_boost_active = false;
    game_speed_multiplier = 1.0f; // Reset game speed multiplier
    slowing_sound_played = false; // Reset slowing sound flag
    // player_current_speed and current_score_increase_rate will be calculated based on this multiplier in update()
    if (player) { // Ensure player sprite exists before modifying
        player->width = PLAYER_WIDTH; // Reset player size
        player->height = PLAYER_HEIGHT;
    }

    // Reset game over delay variables
    game_over_delay_start_time = 0.0f;
    game_over_sound_played = false;


    // Reset difficulty scaling timers
    last_speed_increase_time = clock();
    last_enemy_count_decrease_time = clock();
    game_start_time = clock();
    // Force immediate power-up spawn attempt by setting its last spawn time to the past
    last_powerup_spawn_time = (float)clock() / CLOCKS_PER_SEC - LIFE_POWERUP_SPAWN_INTERVAL;
    last_speedup_spawn_time = (float)clock() / CLOCKS_PER_SEC - SPEEDUP_POWERUP_SPAWN_INTERVAL; // NEW: Reset speedup spawn timer

    // Deactivate score pop-up
    score_popup_active = false;
    if (score_popup_text) score_popup_text->color.a = 0.0f;

    // NEW: Reset blinking states and invulnerability
    heart_ui_blink_active = false;
    player_is_blinking = false;
    player_blink_start_time = 0.0f;
    player_current_blink_duration = 0.0f;
    player_is_invulnerable = false;
    score_text_blink_active = false; // NEW: Reset score text blink
    if (player) player->color.a = 1.0f; // Ensure player is fully visible
    if (current_score_value_text) { // NEW: Ensure score text is white
        current_score_value_text->color.r = 1.0f;
        current_score_value_text->color.g = 1.0f;
        current_score_value_text->color.b = 1.0f;
        current_score_value_text->color.a = 1.0f;
    }


    // Spawn initial enemies in distinct lanes
    int lanes_used[4] = {0};
    int enemies_to_spawn = 2;
    for (int i = 0; i < enemies_to_spawn; i++) {
        int lane;
        do {
            lane = (rand() % 4) + 1;
        } while (lanes_used[lane - 1] == 1);

        lanes_used[lane - 1] = 1;

        for (int j = 0; j < MAX_ENEMIES_INITIAL; j++) {
            if (!game_objects[j].active) {
                init_game_object(j, lane, -PLAYER_HEIGHT, TYPE_ENEMY, rand() % 3); // Random enemy sprite index
                break;
            }
        }
    }
}

// Function to read the high score from file
int read_high_score() {
    FILE *file = fopen("assets/score.txt", "r");
    int score = 0;
    if (file) {
        if (fscanf(file, "%d", &score) != 1) {
            score = 0; // File exists but content is invalid, reset to 0
        }
        fclose(file);
    } else {
        // File doesn't exist, this is fine, just return 0
    }
    return score;
}

// Function to write the high score to file
void write_high_score(int score) {
    FILE *file = fopen("assets/score.txt", "w"); // "w" creates or truncates the file
    if (file) {
        fprintf(file, "%d", score);
        fclose(file);
    } else {
        perror("Failed to open score.txt for writing");
    }
}


// Function for mouse events
void on_mouse(S2D_Event e) {
    if (e.type == S2D_MOUSE_DOWN && e.button == MOUSE_LEFT) {
        if (current_game_state == STATE_MENU) {
            float scaled_width = START_BUTTON_WIDTH * (1.0f + 0.1f * sinf(start_button_animation_time));
            float scaled_height = START_BUTTON_HEIGHT * (1.0f + 0.1f * sinf(start_button_animation_time));
            float scaled_x = SCREEN_WIDTH / 2 - scaled_width / 2;
            float scaled_y = SCREEN_HEIGHT / 2 - scaled_height / 2;

            if (e.x >= scaled_x && e.x < scaled_x + scaled_width &&
                e.y >= scaled_y && e.y < scaled_y + scaled_height) {
                reset_game(); // Start the game
                // Background music: Free existing, load new, play.
                // This is a workaround as Simple2D does not expose S2D_LoopSound or S2D_PauseSound directly.
                if (background_music) S2D_FreeSound(background_music);
                background_music = load_sound_safe("assets/background.mp3");
                if (background_music) S2D_PlaySound(background_music); // Play (will loop if Simple2D handles it implicitly)
            }
        } else if (current_game_state == STATE_PLAYING) {
            float pause_btn_x = SCREEN_WIDTH - PAUSE_BUTTON_SIZE - 10;
            float pause_btn_y = 10;
            if (e.x >= pause_btn_x && e.x < pause_btn_x + PAUSE_BUTTON_SIZE &&
                e.y >= pause_btn_y && e.y < pause_btn_y + PAUSE_BUTTON_SIZE) {
                current_game_state = STATE_PAUSED;
                // Background music: Free the sound when pausing.
                // This effectively stops it, as there's no direct pause function.
                if (background_music) {
                    S2D_FreeSound(background_music);
                    background_music = NULL;
                }
            }
        } else if (current_game_state == STATE_PAUSED) {
            float total_buttons_height = 3 * BUTTON_HEIGHT + 2 * PAUSE_MENU_BUTTON_SPACING;
            float start_y = (SCREEN_HEIGHT - total_buttons_height) / 2;

            float resume_btn_x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
            float resume_btn_y = start_y;
            float home_btn_x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
            float home_btn_y = start_y + BUTTON_HEIGHT + PAUSE_MENU_BUTTON_SPACING;
            float exit_btn_x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
            float exit_btn_y = start_y + 2 * (BUTTON_HEIGHT + PAUSE_MENU_BUTTON_SPACING);

            if (e.x >= resume_btn_x && e.x < resume_btn_x + BUTTON_WIDTH &&
                e.y >= resume_btn_y && e.y < resume_btn_y + BUTTON_HEIGHT) {
                current_game_state = STATE_PLAYING;
                // Background music: Re-load and play if it was freed (after a pause).
                if (!background_music) {
                    background_music = load_sound_safe("assets/background.mp3");
                }
                if (background_music) S2D_PlaySound(background_music); // Play (will loop if Simple2D handles it implicitly)
            } else if (e.x >= home_btn_x && e.x < home_btn_x + BUTTON_WIDTH &&
                     e.y >= home_btn_y && e.y < home_btn_y + BUTTON_HEIGHT) {
                reset_game();
                current_game_state = STATE_MENU;
                // Background music: Stop when returning to menu.
                if (background_music) {
                    S2D_FreeSound(background_music);
                    background_music = NULL;
                }
            } else if (e.x >= exit_btn_x && e.x < exit_btn_x + BUTTON_WIDTH &&
                     e.y >= exit_btn_y && e.y < exit_btn_y + BUTTON_HEIGHT) {
                S2D_Quit();
                exit(0);
            }
        } else if (current_game_state == STATE_GAME_OVER) {
            float game_over_img_y = SCREEN_HEIGHT / 2 - GAMEOVER_IMAGE_HEIGHT / 2;
            float buttons_y = game_over_img_y + GAMEOVER_IMAGE_HEIGHT + GAMEOVER_BUTTON_VERTICAL_OFFSET;

            float total_buttons_width = RETRY_BUTTON_WIDTH + EXIT_BUTTON_WIDTH + GAMEOVER_BUTTON_HORIZONTAL_SPACING;
            float start_x = SCREEN_WIDTH / 2 - total_buttons_width / 2;

            float retry_btn_x = start_x;
            float exit_btn_x = start_x + RETRY_BUTTON_WIDTH + GAMEOVER_BUTTON_HORIZONTAL_SPACING;
            float button_y = buttons_y;

            if (e.x >= retry_btn_x && e.x < retry_btn_x + RETRY_BUTTON_WIDTH &&
                e.y >= button_y && e.y < button_y + RETRY_BUTTON_HEIGHT) {
                reset_game();
                // Background music: Free existing, load new, play on retry.
                if (background_music) S2D_FreeSound(background_music);
                background_music = load_sound_safe("assets/background.mp3");
                if (background_music) S2D_PlaySound(background_music); // Play (will loop if Simple2D handles it implicitly)
            } else if (e.x >= exit_btn_x && e.x < exit_btn_x + EXIT_BUTTON_WIDTH &&
                     e.y >= button_y && e.y < button_y + EXIT_BUTTON_HEIGHT) {
                S2D_Quit();
                exit(0);
            }
        }
    }
}

// Function to render game elements
void render() {
    // Always draw the background and road, regardless of game state
    draw_quad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.1, 0.6, 0.1, 1.0f); // Background

    int tile_height = 25;
    int block_count = SCREEN_HEIGHT / tile_height + 2;

    for (int block = 0; block < 2; block++) {
        float base_y = road_offset - block * SCREEN_HEIGHT;
        draw_quad(60, base_y, 520, SCREEN_HEIGHT, 0.18, 0.18, 0.18, 1.0f); // Road

        for (int i = 0; i < block_count; i++) {
            float y = base_y + i * tile_height;
            // Curb colors
            float left_shade = ((block % 2 == 0) ? i : i + 1) % 2 == 0 ? 0.75f : 0.6f;
            float right_shade = ((block % 2 == 0) ? i + 1 : i) % 2 == 0 ? 0.75f : 0.6f;
            draw_quad(40, y, 20, tile_height, left_shade, left_shade, left_shade, 1.0f); // Left curb
            draw_quad(580, y, 20, tile_height, right_shade, right_shade, right_shade, 1.0f); // Right curb

            // Road edge colors (red/white)
            float edge_g_b = (i + block) % 2 == 0 ? 0.0f : 1.0f;
            draw_quad(60, y, 8, tile_height, 1.0f, edge_g_b, edge_g_b, 1.0f); // Road edge left
            draw_quad(572, y, 8, tile_height, 1.0f, edge_g_b, edge_g_b, 1.0f); // Road edge right
        }

        // Road lines (yellow and white)
        for (int i = 0; i < block_count * (tile_height / 20 + 1); i++) {
            float y_20 = base_y + i * 20; // For yellow lines
            draw_quad(LEFT_YELLOW_LINE_X, y_20, 4, 16, 1, 0.8, 0, 1.0f);
            draw_quad(320, y_20, 4, 16, 1, 0.8, 0, 1.0f);
            draw_quad(RIGHT_YELLOW_LINE_X, y_20, 4, 16, 1, 0.8, 0, 1.0f);

            if (i % 2 == 0) { // Draw white lines every other 20-unit increment (effectively 40 units)
                float y_40 = base_y + i * 20;
                draw_quad(195, y_40, 4, 35, 1, 1, 1, 1.0f);
                draw_quad(445, y_40, 4, 35, 1, 1, 1, 1.0f);
            }
        }
    }

    // Only draw player, enemies, and obstacles if playing or game over or paused (or during game over delay)
    if (current_game_state == STATE_PLAYING || current_game_state == STATE_GAME_OVER_DELAY || current_game_state == STATE_GAME_OVER || current_game_state == STATE_PAUSED) {
        // Draw player
        // Adjust player's rendered position and size based on speed boost.
        // This change is instantaneous when 'is_speed_boost_active' changes.
        player->width = is_speed_boost_active ? PLAYER_WIDTH * 1.5f : PLAYER_WIDTH;
        player->height = is_speed_boost_active ? PLAYER_HEIGHT * 1.5f : PLAYER_HEIGHT;
        // Adjust player_x to keep the center of the player in the same lane
        player->x = player_x - (player->width - PLAYER_WIDTH) / 2;
        // Adjust player_y to maintain bottom margin
        player->y = SCREEN_HEIGHT - player->height - BOTTOM_MARGIN;

        // Apply player blinking effect if active
        if (player_is_blinking) {
            float current_time_sec = (float)clock() / CLOCKS_PER_SEC;
            float elapsed_blink_time = current_time_sec - player_blink_start_time;
            float alpha = 0.5f + 0.5f * sinf(elapsed_blink_time * 20.0f * M_PI); // Rapid blinking
            player->color.a = fmaxf(0.0f, fminf(1.0f, alpha)); // Clamp alpha between 0 and 1
        }
        // If not blinking, ensure player is fully visible
        else {
            player->color.a = 1.0f;
        }
        S2D_DrawSprite(player);

        // Draw enemies, obstacles, and power-ups
        for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
            if (game_objects[i].active) {
                S2D_Sprite *sprite_to_draw = NULL;
                float current_width = 0, current_height = 0;

                switch (game_objects[i].type) {
                    case TYPE_ENEMY:
                        sprite_to_draw = enemies_sprites[game_objects[i].sprite_index];
                        current_width = ENEMY_WIDTH;
                        current_height = ENEMY_HEIGHT;
                        break;
                    case TYPE_OBSTACLE:
                        sprite_to_draw = obstacles_sprites[game_objects[i].sprite_index];
                        current_width = OBSTACLE_WIDTH;
                        current_height = OBSTACLE_HEIGHT;
                        break;
                    case TYPE_POWERUP: // Heart
                        sprite_to_draw = powerup_sprites[game_objects[i].sprite_index];
                        current_width = POWERUP_WIDTH;
                        current_height = POWERUP_HEIGHT;
                        break;
                    case TYPE_SPEEDUP: // Speed-up
                        sprite_to_draw = speedup_sprites[game_objects[i].sprite_index];
                        current_width = SPEEDUP_WIDTH;
                        current_height = SPEEDUP_HEIGHT;
                        break;
                }
                if (sprite_to_draw) {
                    sprite_to_draw->x = game_objects[i].x;
                    sprite_to_draw->y = game_objects[i].y;
                    sprite_to_draw->width = current_width;
                    sprite_to_draw->height = current_height;
                    S2D_DrawSprite(sprite_to_draw);
                } else {
                    fprintf(stderr, "Error: Sprite is NULL for active game object type %d, index %d.\n", game_objects[i].type, game_objects[i].sprite_index);
                }
            }
        }
    }

    // Draw UI elements based on game state (these are drawn on top)
    if (current_game_state == STATE_PLAYING || current_game_state == STATE_GAME_OVER_DELAY) { // UI visible during delay
        // Draw score image at the top
        if (score_image_sprite) {
            score_image_sprite->x = SCREEN_WIDTH / 2 - SCORE_IMAGE_WIDTH / 2;
            score_image_sprite->y = 10; // Small padding from top
            score_image_sprite->width = SCORE_IMAGE_WIDTH;
            score_image_sprite->height = SCORE_IMAGE_HEIGHT;
            S2D_DrawSprite(score_image_sprite);
        } else {
            fprintf(stderr, "Error: score_image_sprite is NULL. Check assets/score.png\n");
        }

        // Draw "Score" label on the left side of the score image
        if (score_label_text) {
            score_label_text->x = score_image_sprite->x + SCORE_LABEL_HORIZONTAL_OFFSET; // Offset from left edge of image
            score_label_text->y = score_image_sprite->y + (SCORE_IMAGE_HEIGHT / 2) - (score_label_text->height / 2);
            S2D_DrawText(score_label_text);
        }

        // Draw current score value on the right side of the score image
        if (current_score_value_text) {
            current_score_value_text->x = score_image_sprite->x + SCORE_IMAGE_WIDTH - current_score_value_text->width - SCORE_VALUE_HORIZONTAL_OFFSET; // Offset from right edge
            current_score_value_text->y = score_image_sprite->y + (SCORE_IMAGE_HEIGHT / 2) - (current_score_value_text->height / 2) + SCORE_VALUE_VERTICAL_OFFSET; // Offset vertically

            // Apply score text blinking effect if active
            if (score_text_blink_active) {
                float current_time_sec = (float)clock() / CLOCKS_PER_SEC;
                float elapsed_blink_time = current_time_sec - score_text_blink_start_time;
                // Determine blink phase
                if (fmodf(elapsed_blink_time, SCORE_TEXT_BLINK_INTERVAL * 2) < SCORE_TEXT_BLINK_INTERVAL) {
                    // "On" phase: green
                    current_score_value_text->color.r = 0.0f;
                    current_score_value_text->color.g = 1.0f;
                    current_score_value_text->color.b = 0.0f;
                } else {
                    // "Off" phase: white
                    current_score_value_text->color.r = 1.0f;
                    current_score_value_text->color.g = 1.0f;
                    current_score_value_text->color.b = 1.0f;
                }
            } else {
                // Ensure it's white when not blinking
                current_score_value_text->color.r = 1.0f;
                current_score_value_text->color.g = 1.0f;
                current_score_value_text->color.b = 1.0f;
            }
            S2D_DrawText(current_score_value_text);
        }

        // Draw timer in the center of the score image, adjusted by TIMER_HORIZONTAL_OFFSET
        if (timer_text) {
            timer_text->x = score_image_sprite->x + (SCORE_IMAGE_WIDTH / 2) - (timer_text->width / 2) + TIMER_HORIZONTAL_OFFSET;
            timer_text->y = score_image_sprite->y + (SCORE_IMAGE_HEIGHT / 2) - (timer_text->height / 2);
            S2D_DrawText(timer_text);
        }

        // Draw pause button
        if (pause_button_sprite) {
            pause_button_sprite->x = SCREEN_WIDTH - PAUSE_BUTTON_SIZE - 10; // 10px padding from right
            pause_button_sprite->y = 10; // 10px padding from top
            pause_button_sprite->width = PAUSE_BUTTON_SIZE;
            pause_button_sprite->height = PAUSE_BUTTON_SIZE;
            S2D_DrawSprite(pause_button_sprite);
        } else {
            fprintf(stderr, "Error: pause_button_sprite is NULL. Check assets/pause.png\n");
        }

        // Draw extra life UI sprite if available
        if (has_extra_life && life_ui_sprite) {
            life_ui_sprite->x = 10; // Top-left corner, 10px padding
            life_ui_sprite->y = 10;
            // life_ui_sprite->width and height are already set in main()

            // Apply heart UI blinking effect if active
            if (heart_ui_blink_active) {
                float current_time_sec = (float)clock() / CLOCKS_PER_SEC;
                float elapsed_blink_time = current_time_sec - heart_ui_blink_start_time;
                // Blink rapidly (e.g., 10 times per second)
                float alpha = 0.5f + 0.5f * sinf(elapsed_blink_time * 20.0f * M_PI); // Sine wave for blinking
                life_ui_sprite->color.a = fmaxf(0.0f, fminf(1.0f, alpha)); // Clamp alpha between 0 and 1
            } else {
                life_ui_sprite->color.a = 1.0f; // Fully visible when not blinking
            }
            S2D_DrawSprite(life_ui_sprite);
        }

        // Draw speed-up UI sprite if active
        if (is_speed_boost_active && speedup_ui_sprite) {
            speedup_ui_sprite->x = 10;
            // Position below heart if heart is active, otherwise at top-left
            speedup_ui_sprite->y = has_extra_life ? (10 + POWERUP_HEIGHT + 5) : 10; // 5px padding
            // speedup_ui_sprite->width and height are already set in main()
            // No blinking for speedup UI requested, just solid
            speedup_ui_sprite->color.a = 1.0f;
            S2D_DrawSprite(speedup_ui_sprite);
        }

        // Draw score pop-up text if active
        if (score_popup_active && score_popup_text) {
            score_popup_text->x = score_popup_x - (score_popup_text->width / 2); // Center text
            score_popup_text->y = score_popup_y;
            S2D_DrawText(score_popup_text);
        }

    }

    if (current_game_state == STATE_GAME_OVER) {
        // Calculate positions for final score image (above game over image)
        float final_score_img_y = SCREEN_HEIGHT / 2 - GAMEOVER_IMAGE_HEIGHT / 2 - FINAL_SCORE_IMAGE_HEIGHT - 20; // 20px buffer above gameover
        float final_score_img_x = SCREEN_WIDTH / 2 - FINAL_SCORE_IMAGE_WIDTH / 2;

        if (final_score_image_sprite) {
            final_score_image_sprite->x = final_score_img_x;
            final_score_image_sprite->y = final_score_img_y;
            final_score_image_sprite->width = FINAL_SCORE_IMAGE_WIDTH;
            final_score_image_sprite->height = FINAL_SCORE_IMAGE_HEIGHT;
            S2D_DrawSprite(final_score_image_sprite);
        } else {
            fprintf(stderr, "Error: final_score_image_sprite is NULL. Drawing fallback rectangle.\n");
            draw_quad(final_score_img_x, final_score_img_y, FINAL_SCORE_IMAGE_WIDTH, FINAL_SCORE_IMAGE_HEIGHT, 0.0f, 0.5f, 0.7f, 0.8f); // Blue fallback
        }

        // Draw current score on final score image
        if (final_current_score_text) {
            final_current_score_text->x = final_score_img_x + FINAL_SCORE_CURRENT_OFFSET_X - (final_current_score_text->width / 2); // Center text within its offset area
            final_current_score_text->y = final_score_img_y + FINAL_SCORE_CURRENT_OFFSET_Y;
            S2D_DrawText(final_current_score_text);
        }

        // Draw high score on final score image
        if (final_high_score_text) {
            final_high_score_text->x = final_score_img_x + FINAL_SCORE_HIGH_SCORE_OFFSET_X - (final_high_score_text->width / 2); // Center text within its offset area
            final_high_score_text->y = final_score_img_y + FINAL_SCORE_HIGH_SCORE_OFFSET_Y;
            S2D_DrawText(final_high_score_text);
        }


        // Now, reposition gameover.png and its buttons below the final score image
        // Adjust game_over_img_y to be below final_score_image_sprite with some padding
        float game_over_img_y = final_score_img_y + FINAL_SCORE_IMAGE_HEIGHT + 20; // 20px padding between finalscore and gameover

        // Draw gameover.png in the center
        if (game_over_image_sprite) {
            game_over_image_sprite->x = SCREEN_WIDTH / 2 - GAMEOVER_IMAGE_WIDTH / 2;
            game_over_image_sprite->y = game_over_img_y; // Updated Y position
            game_over_image_sprite->width = GAMEOVER_IMAGE_WIDTH;
            game_over_image_sprite->height = GAMEOVER_IMAGE_HEIGHT;
            S2D_DrawSprite(game_over_image_sprite);
        } else {
            fprintf(stderr, "Error: game_over_image_sprite is NULL. Drawing fallback rectangle.\n");
            draw_quad(SCREEN_WIDTH / 2 - GAMEOVER_IMAGE_WIDTH / 2, game_over_img_y, GAMEOVER_IMAGE_WIDTH, GAMEOVER_IMAGE_HEIGHT, 0.7f, 0.0f, 0.0f, 0.8f); // Red fallback
        }

        // Calculate positions for retry and exit buttons below game over image
        float buttons_y = game_over_img_y + GAMEOVER_IMAGE_HEIGHT + GAMEOVER_BUTTON_VERTICAL_OFFSET; // Relative to new game_over_img_y
        float total_buttons_width = RETRY_BUTTON_WIDTH + EXIT_BUTTON_WIDTH + GAMEOVER_BUTTON_HORIZONTAL_SPACING;
        float start_x = SCREEN_WIDTH / 2 - total_buttons_width / 2;

        // Draw Retry button
        if (retry_button_sprite) {
            retry_button_sprite->width = RETRY_BUTTON_WIDTH;
            retry_button_sprite->height = RETRY_BUTTON_HEIGHT;
            retry_button_sprite->x = start_x;
            retry_button_sprite->y = buttons_y;
            S2D_DrawSprite(retry_button_sprite);
        } else {
            fprintf(stderr, "Error: retry_button_sprite is NULL. Check assets/retry.png\n");
        }

        // Draw Exit button
        if (exit_button_sprite) {
            exit_button_sprite->width = EXIT_BUTTON_WIDTH;
            exit_button_sprite->height = EXIT_BUTTON_HEIGHT;
            exit_button_sprite->x = start_x + RETRY_BUTTON_WIDTH + GAMEOVER_BUTTON_HORIZONTAL_SPACING;
            exit_button_sprite->y = buttons_y;
            S2D_DrawSprite(exit_button_sprite);
        } else {
            fprintf(stderr, "Error: exit_button_sprite is NULL. Check assets/exit.png\n");
        }

    } else if (current_game_state == STATE_MENU) {
        // Draw animated start button
        float scale = 1.0f + 0.1f * sinf(start_button_animation_time); // Scale between 0.9 and 1.1
        start_button_sprite->width = START_BUTTON_WIDTH * scale;
        start_button_sprite->height = START_BUTTON_HEIGHT * scale;
        start_button_sprite->x = SCREEN_WIDTH / 2 - start_button_sprite->width / 2;
        start_button_sprite->y = SCREEN_HEIGHT / 2 - start_button_sprite->height / 2;
        S2D_DrawSprite(start_button_sprite);
    } else if (current_game_state == STATE_PAUSED) {
        // Draw a semi-transparent overlay to dim the background
        draw_quad(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 0.0f, 0.5f); // Black, 50% transparent

        // Calculate positions for pause menu buttons
        float total_buttons_height = 3 * BUTTON_HEIGHT + 2 * PAUSE_MENU_BUTTON_SPACING;
        float start_y = (SCREEN_HEIGHT - total_buttons_height) / 2;

        // Resume button
        if (resume_button_sprite) {
            resume_button_sprite->width = BUTTON_WIDTH;
            resume_button_sprite->height = BUTTON_HEIGHT;
            resume_button_sprite->x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
            resume_button_sprite->y = start_y;
            S2D_DrawSprite(resume_button_sprite);
        } else {
            fprintf(stderr, "Error: resume_button_sprite is NULL. Check assets/resume.png\n");
        }

        // Home button
        if (home_button_sprite) {
            home_button_sprite->width = BUTTON_WIDTH;
            home_button_sprite->height = BUTTON_HEIGHT;
            home_button_sprite->x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
            home_button_sprite->y = start_y + BUTTON_HEIGHT + PAUSE_MENU_BUTTON_SPACING;
            S2D_DrawSprite(home_button_sprite);
        } else {
            fprintf(stderr, "Error: home_button_sprite is NULL. Check assets/home.png\n");
        }

        // Exit button
        if (exit2_button_sprite) {
            exit2_button_sprite->width = BUTTON_WIDTH;
            exit2_button_sprite->height = BUTTON_HEIGHT;
            exit2_button_sprite->x = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
            exit2_button_sprite->y = start_y + 2 * (BUTTON_HEIGHT + PAUSE_MENU_BUTTON_SPACING);
            S2D_DrawSprite(exit2_button_sprite);
        } else {
            fprintf(stderr, "Error: exit2_button_sprite is NULL. Check assets/exit2.png\n");
        }
    }

    // Scroll road only if playing
    if (current_game_state == STATE_PLAYING) {
        // This is now handled in update()
    }
}

// Function to update game logic
void update() {
    // Calculate delta time manually
    clock_t current_frame_time = clock();
    float delta_time = (float)(current_frame_time - last_frame_time) / CLOCKS_PER_SEC;
    last_frame_time = current_frame_time;

    // Handle game over delay
    if (current_game_state == STATE_GAME_OVER_DELAY) {
        float current_time_sec = (float)current_frame_time / CLOCKS_PER_SEC;
        if (current_time_sec - game_over_delay_start_time >= 3.0f) {
            if (!game_over_sound_played) {
                S2D_PlaySound(gameover_sound);
                game_over_sound_played = true;
            }
            current_game_state = STATE_GAME_OVER;

            // Update high score on game over and save to file
            if (current_score > high_score) {
                high_score = current_score;
                write_high_score(high_score); // Write new high score to file
            }

            // Prepare text for final score display
            snprintf(final_current_score_buffer, sizeof(final_current_score_buffer), "%d", current_score);
            S2D_SetText(final_current_score_text, final_current_score_buffer);

            snprintf(final_high_score_buffer, sizeof(final_high_score_buffer), "%d", high_score);
            S2D_SetText(final_high_score_text, final_high_score_buffer);
        }
        return; // Stop further updates during delay
    }

    // Stop updates if game is over or paused
    if (current_game_state == STATE_GAME_OVER || current_game_state == STATE_PAUSED) return;

    if (current_game_state == STATE_MENU) {
        // Update start button animation only in menu state
        start_button_animation_time += 0.05f; // Adjust speed of animation
        return; // No game logic updates in menu state
    }

    // Apply game speed multiplier to player speed and score rate
    player_current_speed = original_player_speed_val * game_speed_multiplier;
    current_score_increase_rate = original_score_increase_rate_val * game_speed_multiplier;

    // Game logic for STATE_PLAYING
    // Update score
    current_score += (int)(current_score_increase_rate * delta_time); // Use current_score_increase_rate
    snprintf(score_buffer, sizeof(score_buffer), "%d", current_score);
    if (current_score_value_text) {
        S2D_SetText(current_score_value_text, score_buffer);
    }

    float current_time_sec = (float)current_frame_time / CLOCKS_PER_SEC;

    // Update timer
    float elapsed_time_sec = (float)(current_frame_time - game_start_time) / CLOCKS_PER_SEC;
    int minutes = (int)(elapsed_time_sec / 60);
    int seconds = (int)(fmodf(elapsed_time_sec, 60));
    snprintf(timer_buffer, sizeof(timer_buffer), "%02d-%02d", minutes, seconds);
    if (timer_text) {
        S2D_SetText(timer_text, timer_buffer);
    }

    // Increase enemy speed and road scroll speed over time (base speed)
    if (current_time_sec - (float)last_speed_increase_time / CLOCKS_PER_SEC >= ENEMY_SPEED_INCREASE_INTERVAL) {
        current_enemy_speed_min += ENEMY_SPEED_INCREASE_AMOUNT;
        current_enemy_speed_max += ENEMY_SPEED_INCREASE_AMOUNT;
        current_scroll_speed += ENEMY_SPEED_INCREASE_AMOUNT; // Also increase road speed
        last_speed_increase_time = current_frame_time;
    }

    // Decrease max enemies over time
    if (current_time_sec - (float)last_enemy_count_decrease_time / CLOCKS_PER_SEC >= ENEMY_COUNT_DECREASE_INTERVAL) {
        if (max_active_enemies_allowed > MIN_ACTIVE_ENEMIES) {
            max_active_enemies_allowed--;
        }
        last_enemy_count_decrease_time = current_frame_time;
    }

    // Handle player movement
    player_x -= move_left * player_current_speed; // Use player_current_speed
    player_x += move_right * player_current_speed; // Use player_current_speed
    player_x = fmaxf(LEFT_YELLOW_LINE_X + HORIZONTAL_MARGIN, player_x);
    player_x = fminf(RIGHT_YELLOW_LINE_X - PLAYER_WIDTH - HORIZONTAL_MARGIN, player_x);

    // Update enemy, obstacle, and power-up positions
    int player_lane = get_player_lane();
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        if (game_objects[i].active) {
            // Obstacles and Power-ups move with the road's scroll speed, ensuring they are "sticky"
            // Enemies move with their individual speeds
            if (game_objects[i].type == TYPE_ENEMY) {
                game_objects[i].y += game_objects[i].speed * game_speed_multiplier; // Apply game_speed_multiplier
            } else { // TYPE_OBSTACLE or TYPE_POWERUP or TYPE_SPEEDUP
                game_objects[i].y += current_scroll_speed * game_speed_multiplier; // Apply game_speed_multiplier
            }

            // Check for "car passed" sound
            if (game_objects[i].type == TYPE_ENEMY && !game_objects[i].passed_by_sound_played && game_objects[i].y > player_y) {
                // Check if enemy is in an adjacent lane
                int enemy_lane = game_objects[i].lane;
                if (abs(enemy_lane - player_lane) == 1) {
                    S2D_PlaySound(car_passed_sound);
                    game_objects[i].passed_by_sound_played = true; // Mark sound as played for this enemy
                }
            }


            if (game_objects[i].y > SCREEN_HEIGHT) {
                game_objects[i].active = false;
                lanes[game_objects[i].lane - 1].is_occupied = false; // Free up the lane
                if (game_objects[i].type == TYPE_ENEMY) {
                    active_enemy_count--;
                } else if (game_objects[i].type == TYPE_OBSTACLE) {
                    lanes[game_objects[i].lane - 1].has_obstacle = false;
                }
                // No specific action for power-ups leaving screen, just deactivates
            }
        }
    }

    // Scroll road
    road_offset = fmodf(road_offset + current_scroll_speed * game_speed_multiplier, SCREEN_HEIGHT); // Apply game_speed_multiplier


    // Check for collision
    for (int i = 0; i < MAX_GAME_OBJECTS; i++) {
        if (game_objects[i].active) {
            float obj_width = 0, obj_height = 0;

            switch (game_objects[i].type) {
                case TYPE_ENEMY:
                    obj_width = ENEMY_WIDTH;
                    obj_height = ENEMY_HEIGHT;
                    break;
                case TYPE_OBSTACLE:
                    obj_width = OBSTACLE_WIDTH;
                    obj_height = OBSTACLE_HEIGHT;
                    break;
                case TYPE_POWERUP: // Heart
                    obj_width = POWERUP_WIDTH;
                    obj_height = POWERUP_HEIGHT;
                    break;
                case TYPE_SPEEDUP: // Speed-up
                    obj_width = SPEEDUP_WIDTH;
                    obj_height = SPEEDUP_HEIGHT;
                    break;
            }

            // Use player's current (potentially boosted) size for collision detection
            if (check_collision(player_x, player_y, player->width, player->height,
                                 game_objects[i].x, game_objects[i].y,
                                 obj_width, obj_height)) {

                if (game_objects[i].type == TYPE_POWERUP) { // Heart power-up collected
                    game_objects[i].active = false;
                    lanes[game_objects[i].lane - 1].is_occupied = false; // Free up the lane
                    has_extra_life = true; // Player now has an extra life
                    current_score += 500; // Add score for collecting power-up
                    S2D_PlaySound(heart_pickup_sound); // Play heart pickup sound

                    // Activate heart UI blink
                    heart_ui_blink_active = true;
                    heart_ui_blink_start_time = current_time_sec;

                    // Activate score text blink
                    score_text_blink_active = true;
                    score_text_blink_start_time = current_time_sec;

                    // Activate score pop-up animation
                    score_popup_active = true;
                    score_popup_start_time = current_time_sec;
                    // Set initial position relative to the score image
                    initial_score_popup_x = score_image_sprite->x + SCORE_IMAGE_WIDTH / 2;
                    initial_score_popup_y = score_image_sprite->y + SCORE_IMAGE_HEIGHT / 2;
                    score_popup_x = initial_score_popup_x;
                    score_popup_y = initial_score_popup_y;
                    snprintf(score_popup_buffer, sizeof(score_popup_buffer), "+500");
                    S2D_SetText(score_popup_text, score_popup_buffer); // Set text for popup
                    score_popup_text->color.a = 1.0f; // Ensure it's fully visible at start

                    // Player does NOT blink or become invulnerable on heart collection.
                    // This is handled only when a life is *used* after a collision.

                } else if (game_objects[i].type == TYPE_SPEEDUP) { // NEW: Speed-up power-up collected
                    game_objects[i].active = false;
                    lanes[game_objects[i].lane - 1].is_occupied = false;
                    is_speed_boost_active = true;
                    speed_boost_start_time = current_time_sec;
                    game_speed_multiplier = 1.5f; // Set game speed multiplier to 1.5x
                    slowing_sound_played = false; // Reset for next speed boost
                    S2D_PlaySound(car_fasting_sound); // Play car fasting sound

                    // Same score popup and text blink as heart
                    current_score += 500; // Add score for collecting speed-up
                    score_popup_active = true;
                    score_popup_start_time = current_time_sec;
                    initial_score_popup_x = score_image_sprite->x + SCORE_IMAGE_WIDTH / 2;
                    initial_score_popup_y = score_image_sprite->y + SCORE_IMAGE_HEIGHT / 2;
                    score_popup_x = initial_score_popup_x;
                    score_popup_y = initial_score_popup_y;
                    snprintf(score_popup_buffer, sizeof(score_popup_buffer), "+500");
                    S2D_SetText(score_popup_text, score_popup_buffer);
                    score_popup_text->color.a = 1.0f;

                    score_text_blink_active = true;
                    score_text_blink_start_time = current_time_sec;

                    // Activate player invulnerability and blinking for speedup
                    player_is_invulnerable = true;
                    player_is_blinking = true;
                    player_blink_start_time = current_time_sec;
                    player_current_blink_duration = PLAYER_SPEEDUP_INVULNERABILITY_AND_BLINK_DURATION;

                } else { // Collision with enemy or obstacle
                    // Only process collision if player is not currently invulnerable
                    if (!player_is_invulnerable) {
                        if (has_extra_life) {
                            has_extra_life = false; // Use up the extra life
                            game_objects[i].active = false; // Disappear the colliding object
                            lanes[game_objects[i].lane - 1].is_occupied = false; // Free up the lane
                            if (game_objects[i].type == TYPE_ENEMY) {
                                active_enemy_count--;
                                // S2D_PlaySound(car_car_crash_sound); // Removed as per user request
                            } else { // Obstacle
                                lanes[game_objects[i].lane - 1].has_obstacle = false;
                                // S2D_PlaySound(car_obstacle_crash_sound); // Removed as per user request
                            }
                            S2D_PlaySound(heart_used_sound); // Play heart used sound
                            // Activate player invulnerability and blinking for heart
                            player_is_invulnerable = true;
                            player_is_blinking = true;
                            player_blink_start_time = current_time_sec;
                            player_current_blink_duration = PLAYER_HEART_INVULNERABILITY_AND_BLINK_DURATION;

                        } else {
                            // No extra life, game over
                            if (current_game_state != STATE_GAME_OVER_DELAY) { // Only run this logic once on transition
                                current_game_state = STATE_GAME_OVER_DELAY; // Transition to delay state
                                game_over_delay_start_time = current_time_sec;
                                // Background music: Stop background music when game over.
                                if (background_music) {
                                    S2D_FreeSound(background_music);
                                    background_music = NULL;
                                }
                                if (game_objects[i].type == TYPE_ENEMY) {
                                    S2D_PlaySound(car_car_crash_sound); // Play car crash sound
                                } else { // Obstacle
                                    S2D_PlaySound(car_obstacle_crash_sound); // Play obstacle crash sound
                                }
                            }
                            return; // Stop further updates if game is in delay state
                        }
                    }
                }
            }
        }
    }

    // Update score pop-up animation
    if (score_popup_active) {
        float elapsed_popup_time = current_time_sec - score_popup_start_time;
        if (elapsed_popup_time < SCORE_POPUP_DURATION) {
            score_popup_y = initial_score_popup_y - (SCORE_POPUP_SPEED * elapsed_popup_time); // Move up from initial Y
            score_popup_x = initial_score_popup_x; // Keep X stationary
            // Fade out
            score_popup_text->color.a = 1.0f - (elapsed_popup_time / SCORE_POPUP_DURATION);
        } else {
            score_popup_active = false;
            score_popup_text->color.a = 0.0f; // Ensure it's fully transparent when inactive
        }
    }

    // Deactivate heart UI blink after its duration
    if (heart_ui_blink_active && (current_time_sec - heart_ui_blink_start_time >= HEART_UI_BLINK_DURATION)) {
        heart_ui_blink_active = false;
        if (life_ui_sprite) life_ui_sprite->color.a = 1.0f; // Ensure it's fully visible after blinking
    }

    // Handle player blinking and invulnerability duration
    if (player_is_blinking) {
        if (current_time_sec - player_blink_start_time >= player_current_blink_duration) {
            player_is_blinking = false;
            player_is_invulnerable = false; // Invulnerability ends when blinking ends
            if (player) player->color.a = 1.0f; // Ensure player is fully visible
        }
    }

    // NEW: Handle speed boost duration and slowing sound
    if (is_speed_boost_active) {
        float time_remaining = (speed_boost_start_time + PLAYER_SPEEDUP_DURATION) - current_time_sec;
        if (time_remaining <= 3.0f && !slowing_sound_played) {
            S2D_PlaySound(car_slowing_sound);
            slowing_sound_played = true;
        }
        if (time_remaining <= 0.0f) {
            is_speed_boost_active = false;
            game_speed_multiplier = 1.0f; // Revert game speed
            slowing_sound_played = false; // Reset for next speed boost
            // Player size is handled in render() based on is_speed_boost_active
        }
    }


    // Spawn new enemies
    if (active_enemy_count < max_active_enemies_allowed) {
        int lane = (rand() % 4) + 1;
        if (!lanes[lane - 1].is_occupied) {
            for (int j = 0; j < MAX_ENEMIES_INITIAL; j++) {
                if (!game_objects[j].active) {
                    init_game_object(j, lane, -PLAYER_HEIGHT, TYPE_ENEMY, rand() % 3); // Random enemy sprite (0, 1, or 2)
                    break;
                }
            }
        }
    }

    // Spawn obstacles (fixed count)
    for (int j = MAX_ENEMIES_INITIAL; j < MAX_ENEMIES_INITIAL + MAX_OBSTACLES_COUNT; j++) {
        if (!game_objects[j].active) {
            int lane = (rand() % 4) + 1;
            // Only spawn if the lane isn't currently occupied by an active object
            // and enough time has passed since the last obstacle in this lane
            if (!lanes[lane - 1].is_occupied && (current_time_sec - lanes[lane - 1].last_obstacle_time >= 2.0f)) {
                init_game_object(j, lane, -PLAYER_HEIGHT, TYPE_OBSTACLE, rand() % 2); // Random obstacle sprite (0 or 1)
                break; // Spawn one obstacle per update cycle if a slot is free
            }
        }
    }

    // Spawn life power-ups (heart)
    // Check if player doesn't have an extra life, the heart power-up slot is free, and enough time has passed
    if (!has_extra_life && !game_objects[HEART_POWERUP_OBJECT_ARRAY_INDEX].active &&
        (current_time_sec - last_powerup_spawn_time >= LIFE_POWERUP_SPAWN_INTERVAL)) {
        int lane = (rand() % 4) + 1;
        // Ensure the chosen lane is not occupied by an enemy or obstacle or speedup power-up
        bool lane_free_for_powerup = true;
        for (int i = 0; i < MAX_ENEMIES_INITIAL + MAX_OBSTACLES_COUNT; i++) { // Check up to just before power-up indices
            if (game_objects[i].active && game_objects[i].lane == lane) {
                lane_free_for_powerup = false;
                break;
            }
        }
        // Also check if the speedup power-up is in this lane
        if (game_objects[SPEEDUP_POWERUP_OBJECT_ARRAY_INDEX].active && game_objects[SPEEDUP_POWERUP_OBJECT_ARRAY_INDEX].lane == lane) {
            lane_free_for_powerup = false;
        }

        if (lane_free_for_powerup) {
            init_game_object(HEART_POWERUP_OBJECT_ARRAY_INDEX, lane, -POWERUP_HEIGHT, TYPE_POWERUP, 0); // Powerup sprite index 0 (life.png)
            last_powerup_spawn_time = current_time_sec; // Reset powerup spawn timer
        }
    }

    // NEW: Spawn speed-up power-ups
    // Check if speed boost is not active, the speed-up power-up slot is free, and enough time has passed
    if (!is_speed_boost_active && !game_objects[SPEEDUP_POWERUP_OBJECT_ARRAY_INDEX].active &&
        (current_time_sec - last_speedup_spawn_time >= SPEEDUP_POWERUP_SPAWN_INTERVAL)) {
        int lane = (rand() % 4) + 1;
        // Ensure the chosen lane is not occupied by an enemy or obstacle or heart power-up
        bool lane_free_for_speedup = true; // Declaration of lane_free_for_speedup
        for (int i = 0; i < MAX_ENEMIES_INITIAL + MAX_OBSTACLES_COUNT; i++) { // Check up to just before power-up indices
            if (game_objects[i].active && game_objects[i].lane == lane) {
                lane_free_for_speedup = false;
                break;
            }
        }
        // Also check if the heart power-up is in this lane
        if (game_objects[HEART_POWERUP_OBJECT_ARRAY_INDEX].active && game_objects[HEART_POWERUP_OBJECT_ARRAY_INDEX].lane == lane) {
            lane_free_for_speedup = false;
        }

        if (lane_free_for_speedup) {
            init_game_object(SPEEDUP_POWERUP_OBJECT_ARRAY_INDEX, lane, -SPEEDUP_HEIGHT, TYPE_SPEEDUP, 0); // Speedup sprite index 0 (speed.png)
            last_speedup_spawn_time = current_time_sec; // Reset speedup spawn timer
        }
    }
}

// Function for key events
void on_key(S2D_Event e) {
    if (e.type == S2D_KEY_DOWN) {
        if (strcmp(e.key, "Left") == 0) move_left = true;
        else if (strcmp(e.key, "Right") == 0) move_right = true;
    } else if (e.type == S2D_KEY_UP) {
        if (strcmp(e.key, "Left") == 0) move_left = false;
        else if (strcmp(e.key, "Right") == 0) move_right = false;
    }
}

// Helper function to load a sprite and handle errors
S2D_Sprite* load_sprite_safe(const char* path) {
    S2D_Sprite* sprite = S2D_CreateSprite(path);
    if (!sprite) {
        perror("Failed to load sprite");
    }
    return sprite;
}

// Helper function to load text and handle errors
S2D_Text* load_text_safe(const char* font_path, const char* text_str, int size) {
    S2D_Text* text = S2D_CreateText(font_path, text_str, size);
    if (!text) {
        perror("Failed to load font/text");
    }
    return text;
}

// Helper function to load sound and handle errors
S2D_Sound* load_sound_safe(const char* path) {
    S2D_Sound* sound = S2D_CreateSound(path);
    if (!sound) {
        perror("Failed to load sound");
    }
    return sound;
}


int main() {
    S2D_Window *window = S2D_CreateWindow("Car Racing Game", SCREEN_WIDTH, SCREEN_HEIGHT, update, render, 0);
    if (!window) {
        fprintf(stderr, "Failed to create Simple2D window.\n");
        return 1;
    }
    window->on_key = on_key;
    window->on_mouse = on_mouse;
    srand(time(NULL));

    // Initialize timers
    last_frame_time = clock();
    last_speed_increase_time = clock();
    last_enemy_count_decrease_time = clock();
    game_start_time = clock();
    last_powerup_spawn_time = clock(); // Initialize heart power-up spawn timer
    last_speedup_spawn_time = clock(); // NEW: Initialize speed-up power-up spawn timer

    high_score = read_high_score();

    // Initialize player's current speed and score increase rate
    player_current_speed = original_player_speed_val;
    current_score_increase_rate = original_score_increase_rate_val;


    // Load player sprite
    player = load_sprite_safe("assets/player.png");
    if (!player) goto cleanup_window;

    // Load enemy sprites
    const char* enemy_files[] = {"assets/enemy1.png", "assets/enemy2.png", "assets/enemy3.png",
                                 "assets/enemy1.png", "assets/enemy2.png", "assets/enemy3.png"};
    for (int i = 0; i < MAX_ENEMIES_INITIAL; i++) {
        enemies_sprites[i] = load_sprite_safe(enemy_files[i]);
        if (!enemies_sprites[i]) goto cleanup_enemies;
    }

    // Load obstacle sprites
    const char* obstacle_files[] = {"assets/obstacle.png", "assets/obstacle2.png", "assets/obstacle.png"};
    for (int i = 0; i < MAX_OBSTACLES_COUNT; i++) {
        obstacles_sprites[i] = load_sprite_safe(obstacle_files[i]);
        if (!obstacles_sprites[i]) goto cleanup_obstacles;
    }

    // Load power-up sprites
    powerup_sprites[0] = load_sprite_safe("assets/life.png"); // Heart power-up
    if (!powerup_sprites[0]) goto cleanup_powerups;

    speedup_sprites[0] = load_sprite_safe("assets/speed.png"); // NEW: Speed-up power-up
    if (!speedup_sprites[0]) goto cleanup_speedups;

    // Load UI sprites
    start_button_sprite = load_sprite_safe("assets/start.png");
    if (!start_button_sprite) goto cleanup_ui;

    pause_button_sprite = load_sprite_safe("assets/pause.png");
    if (!pause_button_sprite) goto cleanup_ui;

    resume_button_sprite = load_sprite_safe("assets/resume.png");
    if (!resume_button_sprite) goto cleanup_ui;

    home_button_sprite = load_sprite_safe("assets/home.png");
    if (!home_button_sprite) goto cleanup_ui;

    exit2_button_sprite = load_sprite_safe("assets/exit2.png");
    if (!exit2_button_sprite) goto cleanup_ui;

    game_over_image_sprite = load_sprite_safe("assets/gameover.png");
    if (!game_over_image_sprite) goto cleanup_ui;

    retry_button_sprite = load_sprite_safe("assets/retry.png");
    if (!retry_button_sprite) goto cleanup_ui;

    exit_button_sprite = load_sprite_safe("assets/exit.png");
    if (!exit_button_sprite) goto cleanup_ui;

    score_image_sprite = load_sprite_safe("assets/score.png");
    if (!score_image_sprite) goto cleanup_ui;

    // Load final score image sprite
    final_score_image_sprite = load_sprite_safe("assets/finalscore.png");
    if (!final_score_image_sprite) goto cleanup_ui;

    // Load life UI sprite
    life_ui_sprite = load_sprite_safe("assets/life.png"); // Load for UI display
    if (!life_ui_sprite) goto cleanup_ui;
    // Explicitly set width and height for UI sprites after loading
    life_ui_sprite->width = POWERUP_WIDTH;
    life_ui_sprite->height = POWERUP_HEIGHT;

    // NEW: Load speed-up UI sprite
    speedup_ui_sprite = load_sprite_safe("assets/speed.png"); // Load for UI display
    if (!speedup_ui_sprite) goto cleanup_ui;
    // Explicitly set width and height for UI sprites after loading
    speedup_ui_sprite->width = SPEEDUP_WIDTH;
    speedup_ui_sprite->height = SPEEDUP_HEIGHT;


    // Load text elements
    score_label_text = load_text_safe("assets/Debrosee-ALPnL.ttf", "SCORE", SCORE_TEXT_FONT_SIZE);
    if (!score_label_text) goto cleanup_text;
    score_label_text->color.r = 1.0f; score_label_text->color.g = 1.0f; score_label_text->color.b = 1.0f; score_label_text->color.a = 1.0f; // White color

    current_score_value_text = load_text_safe("assets/Carre-JWja.ttf", "0", SCORE_TEXT_FONT_SIZE);
    if (!current_score_value_text) goto cleanup_text;
    current_score_value_text->color.r = 1.0f; current_score_value_text->color.g = 1.0f; current_score_value_text->color.b = 1.0f; current_score_value_text->color.a = 1.0f; // White color

    timer_text = load_text_safe("assets/Carre-JWja.ttf", "00-00", SCORE_TEXT_FONT_SIZE);
    if (!timer_text) goto cleanup_text;
    timer_text->color.r = 1.0f; timer_text->color.g = 1.0f; timer_text->color.b = 1.0f; timer_text->color.a = 1.0f; // White color

    game_over_text = load_text_safe("assets/Debrosee-ALPnL.ttf", "Game Over", 32);
    if (!game_over_text) goto cleanup_text;
    game_over_text->x = SCREEN_WIDTH / 2 - (game_over_text->width / 2); // Center it
    game_over_text->y = SCREEN_HEIGHT / 2;
    game_over_text->color.r = 1.0f; game_over_text->color.g = 0.0f; game_over_text->color.b = 0.0f; game_over_text->color.a = 1.0f; // Red color


    // Load final score texts
    final_current_score_text = load_text_safe("assets/Carre-JWja.ttf", "0", FINAL_SCORE_TEXT_FONT_SIZE);
    if (!final_current_score_text) goto cleanup_text;
    final_current_score_text->color.r = 1.0f; final_current_score_text->color.g = 1.0f; final_current_score_text->color.b = 1.0f; final_current_score_text->color.a = 1.0f; // White color

    final_high_score_text = load_text_safe("assets/Carre-JWja.ttf", "0", FINAL_SCORE_TEXT_FONT_SIZE - 2);
    if (!final_high_score_text) goto cleanup_text;
    final_high_score_text->color.r = 1.0f; final_high_score_text->color.g = 1.0f; final_high_score_text->color.b = 1.0f; final_high_score_text->color.a = 1.0f; // White color

    // Score pop-up text
    score_popup_text = load_text_safe("assets/Carre-JWja.ttf", "+500", 24); // Font and initial text
    if (!score_popup_text) goto cleanup_text;
    score_popup_text->color.r = 0.0f; score_popup_text->color.g = 1.0f; score_popup_text->color.b = 0.0f; score_popup_text->color.a = 0.0f; // Green, initially transparent

    // NEW: Load sounds
    // background_music is initialized to NULL globally
    background_music = load_sound_safe("assets/background.mp3"); // Load initially for menu state
    if (!background_music) goto cleanup_sounds;

    car_car_crash_sound = load_sound_safe("assets/car_car_crash.mp3");
    if (!car_car_crash_sound) goto cleanup_sounds;

    car_obstacle_crash_sound = load_sound_safe("assets/car_obstacle_crash.mp3");
    if (!car_obstacle_crash_sound) goto cleanup_sounds;

    gameover_sound = load_sound_safe("assets/gameover.wav");
    if (!gameover_sound) goto cleanup_sounds;

    car_passed_sound = load_sound_safe("assets/car_passed.mp3");
    if (!car_passed_sound) goto cleanup_sounds;

    heart_pickup_sound = load_sound_safe("assets/heart.wav");
    if (!heart_pickup_sound) goto cleanup_sounds;

    heart_used_sound = load_sound_safe("assets/heart_used.wav");
    if (!heart_used_sound) goto cleanup_sounds;

    car_fasting_sound = load_sound_safe("assets/car_fasting.wav");
    if (!car_fasting_sound) goto cleanup_sounds;

    car_slowing_sound = load_sound_safe("assets/car_slowing.wav");
    if (!car_slowing_sound) goto cleanup_sounds;


    S2D_Show(window);

cleanup_sounds:
    if (car_slowing_sound) S2D_FreeSound(car_slowing_sound);
    if (car_fasting_sound) S2D_FreeSound(car_fasting_sound);
    if (heart_used_sound) S2D_FreeSound(heart_used_sound);
    if (heart_pickup_sound) S2D_FreeSound(heart_pickup_sound);
    if (car_passed_sound) S2D_FreeSound(car_passed_sound);
    if (gameover_sound) S2D_FreeSound(gameover_sound);
    if (car_obstacle_crash_sound) S2D_FreeSound(car_obstacle_crash_sound);
    if (car_car_crash_sound) S2D_FreeSound(car_car_crash_sound);
    if (background_music) S2D_FreeSound(background_music);
cleanup_text:
    if (final_high_score_text) S2D_FreeText(final_high_score_text);
    if (final_current_score_text) S2D_FreeText(final_current_score_text);
    if (timer_text) S2D_FreeText(timer_text);
    if (current_score_value_text) S2D_FreeText(current_score_value_text);
    if (score_label_text) S2D_FreeText(score_label_text);
    if (game_over_text) S2D_FreeText(game_over_text);
    if (score_popup_text) S2D_FreeText(score_popup_text);
cleanup_ui:
    if (final_score_image_sprite) S2D_FreeSprite(final_score_image_sprite);
    if (start_button_sprite) S2D_FreeSprite(start_button_sprite);
    if (pause_button_sprite) S2D_FreeSprite(pause_button_sprite);
    if (resume_button_sprite) S2D_FreeSprite(resume_button_sprite);
    if (home_button_sprite) S2D_FreeSprite(home_button_sprite);
    if (exit2_button_sprite) S2D_FreeSprite(exit2_button_sprite);
    if (game_over_image_sprite) S2D_FreeSprite(game_over_image_sprite);
    if (retry_button_sprite) S2D_FreeSprite(retry_button_sprite);
    if (exit_button_sprite) S2D_FreeSprite(exit_button_sprite);
    if (score_image_sprite) S2D_FreeSprite(score_image_sprite);
    if (life_ui_sprite) S2D_FreeSprite(life_ui_sprite);
    if (speedup_ui_sprite) S2D_FreeSprite(speedup_ui_sprite); // NEW: Free speedup UI sprite
cleanup_speedups: // NEW: Cleanup speedup sprites
    if (speedup_sprites[0]) S2D_FreeSprite(speedup_sprites[0]);
cleanup_powerups:
    if (powerup_sprites[0]) S2D_FreeSprite(powerup_sprites[0]);
cleanup_obstacles:
    for (int i = 0; i < MAX_OBSTACLES_COUNT; i++) S2D_FreeSprite(obstacles_sprites[i]);
cleanup_enemies:
    for (int i = 0; i < MAX_ENEMIES_INITIAL; i++) S2D_FreeSprite(enemies_sprites[i]);
cleanup_player:
    S2D_FreeSprite(player);
cleanup_window:
    S2D_FreeWindow(window);
    return 0;
}
