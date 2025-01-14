#include "struct_data.h"
size_t serialize_snake(Snake *localPlayer, unsigned char *buffer)
{
    size_t offset = 0;

    memcpy(buffer + offset, &localPlayer->message, sizeof(localPlayer->message));
    offset += sizeof(localPlayer->message);

    // Serialize Snake ID
    memcpy(buffer + offset, &localPlayer->id, sizeof(localPlayer->id));
    offset += sizeof(localPlayer->id);

    // Serialize Snake length (int)
    memcpy(buffer + offset, &localPlayer->length, sizeof(localPlayer->length));
    offset += sizeof(localPlayer->length);

    // Serialize Snake body (loop through the body array)
    for (int i = 0; i < localPlayer->length; i++)
    {
        memcpy(buffer + offset, &localPlayer->body[i], sizeof(Vector2));
        offset += sizeof(Vector2);
    }

    // Serialize Snake size (Vector2)
    memcpy(buffer + offset, &localPlayer->size, sizeof(Vector2));
    offset += sizeof(Vector2);

    // Serialize Snake speed (Vector2)
    memcpy(buffer + offset, &localPlayer->speed, sizeof(Vector2));
    offset += sizeof(Vector2);

    // Serialize Snake color (int)
    memcpy(buffer + offset, &localPlayer->color, sizeof(Color));
    offset += sizeof(Color);

    return offset;
}

size_t deserialize_snake(unsigned char *buffer, Snake *localPlayer)
{
    size_t offset = 0;

    memcpy(&localPlayer->message, buffer + offset, sizeof(localPlayer->message));
    offset += sizeof(localPlayer->message);

    // Deserialize Snake ID
    memcpy(&localPlayer->id, buffer + offset, sizeof(localPlayer->id));
    offset += sizeof(localPlayer->id);

    // Deserialize Snake length (int)
    memcpy(&localPlayer->length, buffer + offset, sizeof(localPlayer->length));
    offset += sizeof(localPlayer->length);

    // Deserialize Snake body (loop through the body array)
    for (int i = 0; i < localPlayer->length; i++)
    {
        memcpy(&localPlayer->body[i], buffer + offset, sizeof(Vector2));
        offset += sizeof(Vector2);
    }

    // Deserialize Snake size (Vector2)
    memcpy(&localPlayer->size, buffer + offset, sizeof(Vector2));
    offset += sizeof(Vector2);

    // Deserialize Snake speed (Vector2)
    memcpy(&localPlayer->speed, buffer + offset, sizeof(Vector2));
    offset += sizeof(Vector2);

    // Deserialize Snake color (Color, an int)
    memcpy(&localPlayer->color, buffer + offset, sizeof(Color));
    offset += sizeof(Color);
    // print_snake(localPlayer);
    return offset;
}

void print_snake(Snake *localPlayer)
{
    printf("Deserialized Snake Information:\n");
    printf("Message: %s \n", localPlayer->message);
    printf("ID: %d\n", localPlayer->id);
    printf("Length: %d\n", localPlayer->length);
    printf("Color: rgba(%d, %d, %d, %d)\n",
           localPlayer->color.r,
           localPlayer->color.g,
           localPlayer->color.b,
           localPlayer->color.a);
    printf("Size: (%.2f, %.2f)\n", localPlayer->size.x, localPlayer->size.y);
    printf("Speed: (%.2f, %.2f)\n", localPlayer->speed.x, localPlayer->speed.y);

    // Print the body positions of the snake
    printf("Body:\n");
    for (int i = 0; i < localPlayer->length; i++)
    {
        printf("  Segment %d: (%.2f, %.2f)\n", i, localPlayer->body[i].x, localPlayer->body[i].y);
    }
}

size_t serialize_game_state(GameState *state, unsigned char *buffer)
{
    size_t offset = 0;

    memcpy(buffer + offset, &state->message, sizeof(state->message));
    offset += sizeof(state->message);

    memcpy(buffer + offset, &state->winner_id, sizeof(state->winner_id));
    offset += sizeof(state->winner_id);

    // Serialize the number of snakes
    memcpy(buffer + offset, &state->num_snakes, sizeof(state->num_snakes));
    offset += sizeof(state->num_snakes);

    // Serialize each snake
    for (int i = 0; i < state->num_snakes; i++)
    {
        size_t serialized_snake_size = serialize_snake(&state->snakes[i], buffer + offset);
        offset += serialized_snake_size;
    }

    // Serialize the food
    // printf("food position in gamestateserializer is (%f,%f)\n", state->food.position.x, &state->food.position.y);
    memcpy(buffer + offset, &state->food.position, sizeof(Vector2)); // Serialize position (float x, y)
    offset += sizeof(Vector2);

    // printf("food size in gamestateserializer is (%f,%f)\n", state->food.size.x, &state->food.size.y);
    memcpy(buffer + offset, &state->food.size, sizeof(Vector2)); // Serialize size (float x, y)
    offset += sizeof(Vector2);
    // print_populated_buffer(buffer, offset);

    memcpy(buffer+offset, &state->food.color, sizeof(Color));
    offset += sizeof(Color);

    return offset; // Return the total size of the serialized data
}

size_t deserialize_game_state(unsigned char *buffer, GameState *gameState)
{
    size_t offset = 0;

    memcpy(&gameState->message, buffer + offset, sizeof(gameState->message));
    offset += sizeof(gameState->message);

    memcpy(&gameState->winner_id, buffer + offset, sizeof(gameState->winner_id));
    offset += sizeof(gameState->winner_id);

    // Deserialize the number of snakes
    memcpy(&gameState->num_snakes, buffer + offset, sizeof(gameState->num_snakes));
    offset += sizeof(gameState->num_snakes);
    // printf("Deserialized num_snakes: %d\n", gameState->num_snakes);

    // Deserialize each snake
    printf("Deserialized game state: num_snakes = %d\n", gameState->num_snakes);
    for (int i = 0; i < gameState->num_snakes; i++)
    {
        size_t deserialized_snake_size = deserialize_snake(buffer + offset, &gameState->snakes[i]);
        offset += deserialized_snake_size;
        // printf("printing deserialized snake \n");
        // printf("\n");
    }

    memcpy(&gameState->food.position, buffer + offset, sizeof(Vector2));
    offset += sizeof(Vector2);
    // printf("Deserialized Food Size: (%f, %f)\n", gameState->food.position.x, gameState->food.position.y);
    // Deserialize the food size
    memcpy(&gameState->food.size, buffer + offset, sizeof(Vector2));
    offset += sizeof(Vector2);
    // printf("Deserialized Food Size: (%f, %f)\n", gameState->food.size.x, gameState->food.size.y);

    memcpy(&gameState->food.color, buffer + offset, sizeof(Color));
    offset += sizeof(Color);

    return offset; // Return the total size of the deserialized data
}

void print_populated_buffer(unsigned char *buffer, size_t size)
{
    printf("Serialized buffer contents:\n");
    for (size_t i = 0; i < size; i++)
    {
        printf("%04d ", buffer[i]);
        if ((i + 1) % 16 == 0)
        { // Print 16 bytes per line
            printf("\n");
        }
    }
    printf("\n");
    // endianess will be converted by htons htonl etc
}

// void update_game_state(GameState *state) {
//     for (int i = 0; i < state->num_snakes; i++) {
//         Snake *snake = &state->snakes[i];

//         // Move the snake
//         for (int j = snake->length - 1; j > 0; j--) {
//             snake->body[j] = snake->body[j - 1];
//         }
//         snake->body[0].x += snake->speed.x;
//         snake->body[0].y += snake->speed.y;

//         // // Check for wall collisions
//         // if (snake->body[0].x < 0 || snake->body[0].x >= screenWidth ||
//         //     snake->body[0].y < 0 || snake->body[0].y >= screenHeight) {
//         //     printf("Snake %d hit a wall!\n", snake->id);
//         //     remove_snake_from_game_state(snake->id, state);
//         //     continue;
//         // }

//         // // Check for collisions with itself
//         // for (int j = 1; j < snake->length; j++) {
//         //     if (snake->body[0].x == snake->body[j].x && snake->body[0].y == snake->body[j].y) {
//         //         printf("Snake %d collided with itself!\n", snake->id);
//         //         remove_snake_from_game_state(snake->id, state);
//         //         break;
//         //     }
//         // }

//         // // Check for food collision
//         // if (snake->body[0].x == state->food.position.x &&
//         //     snake->body[0].y == state->food.position.y) {
//         //     printf("Snake %d ate the food!\n", snake->id);
//         //     grow_snake(snake);
//         //     generate_new_food(state);
//         // }
//     }
// }