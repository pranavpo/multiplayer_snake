#ifndef struct_data_h
#define struct_data_h

#include "raylib.h"
#include "string.h"

#define SNAKE_LENGTH   16
#define SQUARE_SIZE     31
#define ID 1

static const int screenWidth = 800;
static const int screenHeight = 450;
static int frameCounter = 0;
static bool gameOver = false;
// static bool pause = false;
static Vector2 offset = { 0,0 };
static int counterTail = 0;
static Vector2 snakePosition[SNAKE_LENGTH] = {0};
// int client_id = 1;

typedef struct Snake{
    char message[50];
    int id;
    Vector2 body[SNAKE_LENGTH];
    Vector2 size;              
    Vector2 speed;
    int length;
    Color color;        
}Snake;

typedef struct Food{
    Vector2 position;
    Vector2 size;
    Color color;
} Food;

typedef struct GameState{
    char message[50];
    int winner_id;
    Snake snakes[5];
    Food food;
    int num_snakes;
} GameState;

// static Snake snake_client[SNAKE_LENGTH] = {0};
// static Snake snake_server[SNAKE_LENGTH] = {0};
// static Vector2 snakePosition_client[SNAKE_LENGTH] = { 0 };
// static Vector2 snakePosition_server[SNAKE_LENGTH] = { 0 };

size_t serialize_snake(Snake * localPlayer,unsigned char * buffer);
void print_populated_buffer(unsigned char *buffer, size_t size);
size_t deserialize_snake(unsigned char *buffer, Snake *localPlayer);
void print_snake(Snake *localPlayer);
size_t serialize_game_state(GameState *state, unsigned char *buffer);
size_t deserialize_game_state(unsigned char *buffer, GameState *gameState);
// void update_game_state(GameState *state);

#endif