#include "client.h"
int port_client;
int client_main()
{
    char ip_address[30];
    printf("Enter ip address: ");
    scanf("%s", ip_address);
    printf("Enter port: ");
    scanf("%d", &port_client);

    unsigned char buffer[1024];
    size_t buffer_size;
    char *message = "Hello Server";
    int sockfd;
    struct sockaddr_in servaddr;

    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr(ip_address);
    servaddr.sin_port = htons(port_client);
    servaddr.sin_family = AF_INET;

    // create datagram socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Create and initialize snake
    Snake *localPlayer = malloc(sizeof(Snake));
    if (localPlayer == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1); // Exit if memory allocation fails
    }

    int snake_length = 1;

    GameState *GameStateClient = malloc(sizeof(GameState));

    init_game(localPlayer);
    // Send serialized data to the server
    makeSnake(localPlayer, &snake_length);
    while (!WindowShouldClose())
    {
        // Serialize the snake
        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }
        char *key;
        bool move = true;
        if (IsKeyPressed(KEY_RIGHT) && strcmp(key, "KEY_LEFT") != 0)
        {
            localPlayer->speed = (Vector2){SQUARE_SIZE, 0};
            key = "KEY_RIGHT";
            bool move = false;
        }
        if (IsKeyPressed(KEY_LEFT) && strcmp(key, "KEY_RIGHT") != 0)
        {
            localPlayer->speed = (Vector2){-SQUARE_SIZE, 0};
            key = "KEY_LEFT";
            bool move = false;
        }
        if (IsKeyPressed(KEY_UP) && strcmp(key, "KEY_DOWN") != 0)
        {
            localPlayer->speed = (Vector2){0, -SQUARE_SIZE};
            key = "KEY_UP";
            bool move = false;
        }
        if (IsKeyPressed(KEY_DOWN) && strcmp(key, "KEY_DOWN") != 0)
        {
            localPlayer->speed = (Vector2){0, SQUARE_SIZE};
            key = "KEY_DOWN";
            bool move = false;
        }
        moveSnake(localPlayer, key);
        if (GameStateClient != NULL)
        {
            printf("food position (x,y) = (%f,%f)\n", GameStateClient->food.position.x, GameStateClient->food.position.y);
            CheckFoodCollision(localPlayer, GameStateClient);
        }

        buffer_size = serialize_snake(localPlayer, buffer);
        sendto(sockfd, buffer, buffer_size, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)NULL, NULL);

        deserialize_game_state(buffer, GameStateClient);

        draw_game(localPlayer, GameStateClient);
    }
    free(localPlayer);
    free(GameStateClient->snakes);

    // if (gameOver == true)
    // {

    // }
}

// with wraparound
void moveSnake(Snake *snake, char *key)
{
    if (snake->body[0].x < 0)
    {
        snake->body[0].x = closest_multiple_of_31(screenWidth - SQUARE_SIZE);
    }
    if (snake->body[0].x >= screenWidth)
    {
        snake->body[0].x = 0;
    }
    if (snake->body[0].y < 0)
    {
        snake->body[0].y = closest_multiple_of_31(screenHeight - SQUARE_SIZE);
    }
    if (snake->body[0].y >= screenHeight)
    {
        snake->body[0].y = 0;
    }
    // strcpy(snake->message, "snake moved");
    strcpy(snake->message, "snake moved");

    // Move the body from tail to head
    for (int i = snake->length - 1; i > 0; i--)
    {
        snake->body[i] = snake->body[i - 1]; // Move each part to the next part's position
    }

    // Move the head in the last direction
    snake->body[0].x += snake->speed.x;
    snake->body[0].y += snake->speed.y;

    // Ensure the snake doesn't move outside the screen
    // if (snake->body[0].x < 0 || snake->body[0].x >= screenWidth ||
    //     snake->body[0].y < 0 || snake->body[0].y >= screenHeight)
    // {
    //     printf("Snake hit the wall! Stopping movement...\n");
    //     gameOver = true; // End the game if the snake hits the edge
    // }

    for (int i = 1; i < snake->length; i++)
    {
        if ((int)snake->body[0].x == (int)snake->body[i].x &&
            (int)snake->body[0].y == (int)snake->body[i].y)
        {
            printf("Snake collided with itself! Game over.\n");
            gameOver = true; // End the game
            return;
        }
    }
}

// without wraparound
// void moveSnake(Snake *snake, char *key)
// {
//     if (snake->body[0].x < 0 || snake->body[0].x >= screenWidth || snake->body[0].y < 0 || snake->body[0].y >= screenHeight)
//     {
//         gameOver = true;
//     }
//     // strcpy(snake->message, "snake moved");
//     strcpy(snake->message, "snake moved");

//     // Move the body from tail to head
//     for (int i = snake->length - 1; i > 0; i--)
//     {
//         snake->body[i] = snake->body[i - 1]; // Move each part to the next part's position
//     }

//     // Move the head in the last direction
//     snake->body[0].x += snake->speed.x;
//     snake->body[0].y += snake->speed.y;

//     for (int i = 1; i < snake->length; i++)
//     {
//         if ((int)snake->body[0].x == (int)snake->body[i].x &&
//             (int)snake->body[0].y == (int)snake->body[i].y)
//         {
//             printf("Snake collided with itself! Game over.\n");
//             gameOver = true; // End the game
//             return;
//         }
//     }
// }

void makeSnake(Snake *localPlayer, int *snake_length)
{

    // Set the snake size, speed, and color
    // localPlayer->message[0] = '\0';
    strcpy(localPlayer->message, "snake created");
    int id = generate_random_id();
    localPlayer->id = id;
    int startPos = closest_multiple_of_31(id);
    Vector2 initial_position = {startPos, startPos}; // Example starting position
    // printf("id is %d \n", id);
    localPlayer->size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
    localPlayer->speed = (Vector2){SQUARE_SIZE, 0}; // Moving along the X-axis
    Color color = getRandomColor();
    localPlayer->color = color;
    localPlayer->length = *snake_length;

    // Initialize the body segments
    for (int i = 0; i < *snake_length; i++)
    {
        localPlayer->body[i] = (Vector2){initial_position.x + i * SQUARE_SIZE, initial_position.y + i * SQUARE_SIZE};
        printf("Snake %d: Position=(%.2f, %.2f), Size=(%.2f, %.2f), Speed=(%.2f, %.2f), ID=%d\n",
               i, localPlayer->body[i].x, localPlayer->body[i].y,
               localPlayer->size.x, localPlayer->size.y,
               localPlayer->speed.x, localPlayer->speed.y,
               localPlayer->id);
    }
    // exit(0);
}

int closest_multiple_of_31(int n)
{
    // Divide the number by 31, round it, and then multiply by 31
    return round(n / 31.0) * 31;
}

Color getRandomColor()
{
    return (Color){
        (unsigned char)(GetRandomValue(0, 255)), // Random red value (0 to 255)
        (unsigned char)(GetRandomValue(0, 255)), // Random green value (0 to 255)
        (unsigned char)(GetRandomValue(0, 255)), // Random blue value (0 to 255)
        255                                      // Alpha value (255 for full opacity)
    };
}

int generate_random_id()
{
    srand(time(NULL));

    // Generate a random number between 1 and 450
    int random_number = (rand() % 450) + 1;

    // Print the random number
    return random_number;
}

int draw_other_snakes(Snake *snake)
{
    for (int i = 0; i < snake->length; i++)
        DrawRectangleV(snake->body[i], snake->size, snake->color);
}

int init_game(Snake *snake)
{
    InitWindow(screenWidth, screenHeight, "classic game: snake");
    SetTargetFPS(9);
    gameOver = false;
    counterTail = 1;
    // pause = false;
    offset.x = screenWidth % SQUARE_SIZE;
    offset.y = screenHeight % SQUARE_SIZE;

    // for (int i = 0; i < SNAKE_LENGTH; i++)
    // {
    //     snakePosition[i] = (Vector2){ 0.0f, 0.0f };
    // }

    return 0;
}

int update_draw_frame(Snake *snake, GameState *GameState)
{
    update_game();
    draw_game(snake, GameState);
    return 0;
}

int draw_game(Snake *snake, GameState *GameState)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (gameOver)
    {
        frameCounter++; // Increment the frame counter during Game Over
        printf("game over drawing \n");
        DrawText("Game Over", screenWidth / 2 - 50, screenHeight / 2, 20, RED);

        // Wait for 180 frames (~3 seconds at 60 FPS) before closing
        if (frameCounter > 180)
        {
            CloseWindow();
            return 0;
        }
    }
    else if (strcmp(GameState->message, "WINNER") == 0)
    {
        frameCounter++; // Increment the frame counter for WINNER state
        char winnerText[50];
        sprintf(winnerText, "Player ID %d Won!", GameState->winner_id);
        DrawText(winnerText, screenWidth / 2 - MeasureText(winnerText, 20) / 2, screenHeight / 2, 20, GREEN);

        // Wait for 180 frames (~3 seconds at 60 FPS) before closing
        if (frameCounter > 180)
        {
            CloseWindow();
            return 0;
        }
    }
    else
    {
        for (int i = 0; i < screenWidth / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2){SQUARE_SIZE * i, 0}, (Vector2){SQUARE_SIZE * i, screenHeight}, LIGHTGRAY);
        }

        for (int i = 0; i < screenHeight / SQUARE_SIZE + 1; i++)
        {
            DrawLineV((Vector2){0, SQUARE_SIZE * i}, (Vector2){screenWidth, SQUARE_SIZE * i}, LIGHTGRAY);
        }
        // }

        for (int i = 0; i < snake->length; i++)
            DrawRectangleV(snake->body[i], snake->size, snake->color);

        for (int i = 0; i <= GameState->num_snakes; i++)
        {
            // printf("snake id is %d\n",GameState->snakes[i].id);
            if (GameState->snakes[i].id == snake->id)
            {
                continue;
            }
            draw_other_snakes(&GameState->snakes[i]);
        }
        Vector2 FoodPosition = {closest_multiple_of_31(GameState->food.position.x), closest_multiple_of_31(GameState->food.position.y)};
        DrawRectangleV(FoodPosition, GameState->food.size, GameState->food.color);
    }
    EndDrawing();
    return 0;
}

void CheckFoodCollision(Snake *snake, GameState *GameState)
{
    Vector2 FoodPosition = {closest_multiple_of_31(GameState->food.position.x), closest_multiple_of_31(GameState->food.position.y)};
    if ((int)snake->body[0].x == (int)FoodPosition.x && (int)snake->body[0].y == (int)FoodPosition.y)
    {
        snake->length += 1;
        strcpy(snake->message, "food eaten");
        printf("%s", snake->message);
    }
}

void update_game()
{
    //    if (!gameOver){
    // if (IsKeyPressed('P')) pause = !pause;

    // if (!pause)
    // {

    // }
    // }
}