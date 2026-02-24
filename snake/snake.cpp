#include "raylib.h"
#include <iostream>
#include <cstring>
#include <time.h>
#define GREY          CLITERAL(Color){0x6F, 0x6F, 0x6F, 0x6F}
#define GRID_SIZE           20
#define HEIGHT_OFFSET       45
#define TARGET_FPS          144

#define DEFAULT_SNAKE_LEN   3

// cells
#define EMPTY_CELL          0
#define SNAKE_HEAD_CELL     1
#define SNAKE_SECOND_CELL   2
#define APPLE_CELL          -1

// directions
#define DIR_STOP            0
#define DIR_UP              1
#define DIR_DOWN            2
#define DIR_LEFT            3
#define DIR_RIGHT           4

using namespace std;

int width = 900;
int height = 900;
int rect_width = width / GRID_SIZE;
int rect_height = height / GRID_SIZE;

class GameState {
    private:
    int grid_data[GRID_SIZE][GRID_SIZE];
    Rectangle grid_drawing[GRID_SIZE][GRID_SIZE];
    Rectangle snake_drawing;
    int direction = DIR_STOP;
    int snake_length = DEFAULT_SNAKE_LEN;
    int apples_eaten = 0;
    Texture2D apple_texture;

    public:
    GameState(Texture2D apple_texture) {
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                grid_data[x][y] = 0;
                // rectangle for drawing the grid
                grid_drawing[x][y] = (Rectangle) {(float) x * rect_width,
                    (float) y * rect_height + HEIGHT_OFFSET,
                    (float) rect_width,
                    (float) rect_height};
            }
        }
        snake_drawing = (Rectangle) {0, 0, (float) rect_width - 1, (float) rect_height - 1};
        this->apple_texture = apple_texture;

        // snake spawn position

        grid_data[6][10] = 1;
        grid_data[5][10] = 2;
        grid_data[4][10] = 3;

        // first apple start position
        grid_data[13][10] = APPLE_CELL;
    }

    void draw_grid() {
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                DrawRectangleLinesEx(grid_drawing[x][y], 2, BROWN);
            }
        }
    }

    void change_direction(int direction) {
        // don't let the snake go into itself from stop pos
        // don't let the snake collide into the wall from stop pos
        if (this->direction == DIR_STOP) {
            for (int x = 0; x < GRID_SIZE; x++) {
                for (int y = 0; y < GRID_SIZE; y++) {
                    if (grid_data[x][y] == SNAKE_SECOND_CELL) {
                        switch (direction) {
                            case DIR_UP:
                                if (y != GRID_SIZE - 1) {
                                    if (grid_data[x][y+1] == 1) {
                                        return;
                                    }
                                    break;
                                } else {
                                    return;
                                }
                            case DIR_DOWN:
                                if (y != 0) {
                                    if (grid_data[x][y+1] == 1) {
                                        return;
                                    }
                                    break;
                                } else {
                                    return;
                                }
                            case DIR_LEFT:
                                if (x != GRID_SIZE - 1) {
                                    if (grid_data[x+1][y] == 1) {
                                        return;
                                    }
                                    break;
                                } else {
                                    return;
                                }
                            case DIR_RIGHT:
                                if (x != 0) {
                                    if (grid_data[x-1][y] == 1) {
                                        return;
                                    }
                                    break;
                                } else {
                                    return;
                                }
                        }
                    }
                }
            }
        }

        // verify to not change direction into itself
        switch (direction) {
            case DIR_UP:
                if (this->direction == DIR_DOWN) return;
                break;
            case DIR_DOWN:
                if (this->direction == DIR_UP) return;
                break;
            case DIR_LEFT:
                if (this->direction == DIR_RIGHT) return;
                break;
            case DIR_RIGHT:
                if (this->direction == DIR_LEFT) return;
                break;
        }

        this->direction = direction;
    }

    // returns 1 on illegal collision
    int move_snake() {
        int next_x, next_y;
        // don't endlessly find the piece you move
        bool found_snake_piece = false;
        bool ate_apple = false;
        if (direction != DIR_STOP) {
            for (int x = 0; x < GRID_SIZE; x++) {
                for (int y = 0; y < GRID_SIZE; y++) {
                    // find head, check for collision
                    if (grid_data[x][y] == SNAKE_HEAD_CELL && found_snake_piece == false) {
                        switch (direction) {
                            case DIR_UP:
                                if (y == 0) {
                                    return 1;
                                }
                                if (grid_data[x][y-1] == APPLE_CELL) {
                                    ate_apple = true;
                                } else if (grid_data[x][y-1] != 0 && grid_data[x][y-1] != snake_length) {
                                    return 1;
                                }
                                grid_data[x][y-1] = 1;
                                break;
                            case DIR_DOWN:
                                if (y == GRID_SIZE - 1) {
                                    return 1;
                                }
                                if (grid_data[x][y+1] == APPLE_CELL) {
                                    ate_apple = true;
                                } else if (grid_data[x][y+1] != 0 && grid_data[x][y+1] != snake_length) {
                                    return 1;
                                }
                                grid_data[x][y+1] = 1;
                                break;
                            case DIR_LEFT:
                                if (x == 0) {
                                    return 1;
                                }
                                if (grid_data[x-1][y] == APPLE_CELL) {
                                    ate_apple = true;
                                } else if (grid_data[x-1][y] != 0 && grid_data[x+1][y] != snake_length) {
                                    return 1;
                                }
                                grid_data[x-1][y] = 1;
                                break;
                            case DIR_RIGHT:
                                if (x == GRID_SIZE - 1) {
                                    return 1;
                                }
                                if (grid_data[x+1][y] == APPLE_CELL) {
                                    ate_apple = true;
                                } else if (grid_data[x+1][y] != 0 && grid_data[x+1][y] != snake_length) {
                                    return 1;
                                }
                                grid_data[x+1][y] = 1;
                                break;
                        }

                        // move head
                        next_x = x;
                        next_y = y;
                        grid_data[x][y] = EMPTY_CELL;
                        found_snake_piece = true;
                    }
                }
            }

            // for all snake body cells
            for (int l = SNAKE_SECOND_CELL; l <= snake_length; l++) {
                found_snake_piece = false;
                for (int x = 0; x < GRID_SIZE; x++) {
                    for (int y = 0; y < GRID_SIZE; y++) {
                        if (grid_data[x][y] == l && found_snake_piece == false) {
                            // move to previous cell
                            grid_data[x][y] = EMPTY_CELL;
                            grid_data[next_x][next_y] = l;
                            next_x = x;
                            next_y = y;
                            found_snake_piece = true;

                            // remember if apple was eaten
                            if (ate_apple == true && l == snake_length) {
                                grid_data[x][y] = snake_length + 1;
                            }
                        }
                    }
                }
                // tail collision special case
                if (l == snake_length && found_snake_piece == false) {
                    grid_data[next_x][next_y] = l;
                    found_snake_piece = true;
                }
            }
            if (ate_apple == true) {
                apples_eaten += 1;
                snake_length += 1;
            }
        }
        return 0;
    }

    void draw_snake() {
        // 3 and 0.3 are arbitrary numbers that look nice
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (grid_data[x][y] > 0) {
                    // calculate snake cells x and y
                    snake_drawing.x = x * rect_height;
                    snake_drawing.y = y * rect_width + HEIGHT_OFFSET;
                    DrawRectangleRounded(snake_drawing, 0.3, 3, DARKGREEN);
                    // move inside
                    snake_drawing.x += 3;
                    snake_drawing.y += 3;
                    // make inside rectangle smaller
                    snake_drawing.height -= 3 * 2;
                    snake_drawing.width -= 3 * 2;
                    DrawRectangleRounded(snake_drawing, 0.3, 3, GREEN);
                    // restore original rectangle size
                    snake_drawing.height += 3 * 2;
                    snake_drawing.width += 3 * 2;
                    // draw red point in center of head cell
                    if (grid_data[x][y] == 1) {
                        DrawRectangle(x * rect_height + rect_height / 2 - 4,
                            y * rect_height + rect_height / 2 - 4 + HEIGHT_OFFSET,
                            8, 8, RED);
                    }
                }
            }
        }
    }

    void spawn_apple() {
        int x, y;
        while (true) {
            x = GetRandomValue(0, GRID_SIZE);
            y = GetRandomValue(0, GRID_SIZE);
            if (grid_data[x][y] == EMPTY_CELL) {
                grid_data[x][y] = APPLE_CELL;
                return;
            }
        }
    }

    void draw_apples() {
        bool apple_drawn = false;
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (grid_data[x][y] == APPLE_CELL) {
                    // TODO: make it slightly smaller and keep it centered while doing so
                    DrawTextureEx(apple_texture,
                        (Vector2){(float) x * rect_width,
                            (float) y * rect_height + HEIGHT_OFFSET},
                        0,
                        (float) max(rect_width, rect_height) / (max(apple_texture.width, apple_texture.height)),
                        WHITE);
                    apple_drawn = true;
                }
            }
        }
        // always have one apple
        if (apple_drawn == false) {
            spawn_apple();
            draw_apples();
        }
    }

    int get_apples_eaten() {
        return apples_eaten;
    }
}; // GameState class end

int main()
{
    InitWindow(width, height + HEIGHT_OFFSET, "Snake");
    SetTargetFPS(TARGET_FPS);
    SetRandomSeed(time(NULL));

    Texture2D apple_texture = LoadTexture("images/apple.png");

    GameState *game = new GameState(apple_texture);
    int key = 0;
    bool quit_game = false;

    while (!WindowShouldClose()) {
        // get input (and display it on stdout)
        key = GetKeyPressed();
        if (key) {
            cout << key << endl;
        }

        // send input for direction change
        switch (key) {
            case KEY_W:
                game->change_direction(DIR_UP);
                break;
            case KEY_A:
                game->change_direction(DIR_LEFT);
                break;
            case KEY_S:
                game->change_direction(DIR_DOWN);
                break;
            case KEY_D:
                game->change_direction(DIR_RIGHT);
                break;
            case KEY_SPACE:
                game->change_direction(DIR_STOP);
                break;
        }

        // on death
        if (game->move_snake() != 0) {
            BeginDrawing();
            ClearBackground(RED);
            EndDrawing();
            WaitTime(1);
            while (key != KEY_SPACE) {
                BeginDrawing();
                ClearBackground(RED);

                DrawText("You died!", (width - MeasureText("You died!", 100)) / 2, height / 2 - 60, 100, RAYWHITE);
                DrawText("Press SPACE to restart", (width - MeasureText("Press SPACE to restart", 50)) / 2, height / 2 + 40, 50, RAYWHITE);
                DrawText("Or ESC to quit", (width - MeasureText("Or ESC to quit", 30)) / 2, height / 2 + 100, 30, RAYWHITE);

                key = GetKeyPressed();
                if (key == KEY_ESCAPE) {
                    quit_game = true;
                    break;
                }
                EndDrawing();
            }
            if (quit_game != true) {
                // destroy game
                // generate new game
                game->~GameState();
                game = new GameState(apple_texture);
            } else {
                break;
            }
        }

        BeginDrawing();

        ClearBackground(DARKBROWN);
        game->draw_grid();
        game->draw_snake();
        game->draw_apples();

        DrawText(TextFormat("Apples Eaten: %03i", game->get_apples_eaten()), 6, 2, HEIGHT_OFFSET - 4, RAYWHITE);
        if (game->get_apples_eaten() == 10) {
            DrawText("Hmm... Looks like you're pretty hungry!", 6, 2 + rect_width, HEIGHT_OFFSET - 4, RAYWHITE);
        }

        EndDrawing();
    }

    UnloadTexture(apple_texture);

    CloseWindow();
}
