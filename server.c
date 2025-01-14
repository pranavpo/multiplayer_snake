#include "server.h"
#include "struct_data.h"
int port;
int server_main()
{
    // setup server code
    printf("Enter port number\n");
    scanf("%d", &port);
    char buffer[1024];
    char *message = "Hello Client";
    GameState *state = malloc(sizeof(GameState));
    state->food.position = (Vector2){9, 9};
    state->food.size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
    state->food.color = BLACK;
    state->num_snakes = 0;

    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    servaddr.sin_family = AF_INET;
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    len = sizeof(cliaddr);

    // Initialize state
    printf("Server listening on port %d\n", port);

    while (1)
    {
        // usleep(100000);
        // Wait for a message from a client
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer for each new message
        int n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len);

        if (n < 0)
        {
            perror("Error receiving data");
            continue; // Continue to wait for the next client message
        }

        // Deserialize the received message
        Snake deserializedSnake;
        if (deserialize_snake(buffer, &deserializedSnake) == 0)
        {
            fprintf(stderr, "Failed to deserialize snake.\n");
            continue; // Ignore this message and wait for the next one
        }

        printf("%s\n", deserializedSnake.message);

        if (strcmp(deserializedSnake.message, "food eaten") == 0)
        {
            update_game_state(&deserializedSnake, state);
            create_new_food(state);
        }
        // Process the deserialized message
        if (strcmp(deserializedSnake.message, "snake created") == 0)
        {
            add_snake_to_game_state(&deserializedSnake, state);
        }
        if (strcmp(deserializedSnake.message, "snake moved") == 0)
        {
            // printf("snake moved \n");exit(0);
            update_game_state(&deserializedSnake, state);
        }

        // printf("Number of snakes in GameState is %d\n", state->num_snakes);

        // printf("food position in server is (%f,%f)\n",state->food.position.x,state->food.position.y);
        // Serialize the updated game state
        size_t game_state_size = serialize_game_state(state, buffer);

        // Send the serialized game state back to the client
        int sent_bytes = sendto(listenfd, buffer, game_state_size, 0, (struct sockaddr *)&cliaddr, len);
        if (sent_bytes < 0)
        {
            perror("Error sending data");
        }
    }
}

void update_game_state(Snake *snake, GameState *state)
{
    for (int i = 0; i < state->num_snakes; i++)
    {
        if (state->snakes[i].id == snake->id)
        {
            // Update the snake's body
            for (int j = 0; j < snake->length; j++)
            {
                state->snakes[i].body[j] = snake->body[j];
            }

            if (strcmp(snake->message, "game over") == 0)
            {
                for (int i = 0; i < state->num_snakes; i++)
                {
                    if (state->snakes[i].id == snake->id)
                    {
                        // Remove the snake by shifting other snakes
                        for (int j = i; j < state->num_snakes - 1; j++)
                        {
                            state->snakes[j] = state->snakes[j + 1];
                        }
                        state->num_snakes--;
                        printf("Snake ID %d removed from game.\n", snake->id);
                        return;
                    }
                }
            }

            // Update snake's length
            state->snakes[i].length = snake->length;

            if (snake->length == 15)
            {
                strcpy(state->message, "WINNER");
                state->winner_id = snake->id;
            }
            // Debug message for verification
            // printf("Updated snake ID %d: Current Snake Body Positions = ", snake->id);
            // for (int j = 0; j < snake->length; j++)
            // printf("(%.2f, %.2f) ", snake->body[j].x, snake->body[j].y);
            // printf("| Game State Snake Body Positions = ");
            // for (int j = 0; j < state->snakes[i].length; j++)
            // printf("(%.2f, %.2f) ", state->snakes[i].body[j].x, state->snakes[i].body[j].y);
            // printf("\n");
            return;
        }
    }

    // If snake ID doesn't exist, add it (failsafe)
    add_snake_to_game_state(snake, state);
}

void add_snake_to_game_state(Snake *newSnake, GameState *state)
{
    // Check if the snake already exists in the state
    for (int i = 0; i < state->num_snakes; i++)
    {
        if (state->snakes[i].id == newSnake->id)
        {
            printf("Snake ID %d already exists in game state.\n", newSnake->id);
            return; // Do not add duplicate snakes
        }
    }

    // Add the new snake if space is available
    if (state->num_snakes < 5)
    {
        state->snakes[state->num_snakes] = *newSnake;
        state->num_snakes++;
        printf("Added new snake ID %d. Total snakes: %d\n", newSnake->id, state->num_snakes);
    }
    else
    {
        printf("Cannot add snake. Max number of snakes reached.\n");
    }
}

void create_new_food(GameState *state)
{
    bool validPosition = false;

    while (!validPosition)
    {
        // Generate a random position within the game grid
        state->food.position.x = closest_multiple_of_31(rand() % screenWidth);
        state->food.position.y = closest_multiple_of_31(rand() % screenHeight);

        validPosition = true;

        // Check if the food overlaps with any snake's body
        for (int i = 0; i < state->num_snakes; i++)
        {
            Snake *snake = &state->snakes[i];
            for (int j = 0; j < snake->length; j++)
            {
                if ((int)snake->body[j].x == (int)state->food.position.x &&
                    (int)snake->body[j].y == (int)state->food.position.y)
                {
                    validPosition = false; // Overlaps; generate again
                    break;
                }
            }
            if (!validPosition)
                break;
        }
    }

    // Update food size and color
    state->food.size = (Vector2){SQUARE_SIZE, SQUARE_SIZE};
    state->food.color = BLACK;

    printf("New food created at (%f, %f)\n", state->food.position.x, state->food.position.y);
}