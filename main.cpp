#include <iostream>
#include <conio.h>
#include <Windows.h>

#define FRAMES_PER_SECOND 40
#define MAX_SNAKE_LEN 30

// Game Elements
COORD snake_head;
COORD fruit;
int map_width = 63;
int map_height = 20;
bool is_game_over = false;
bool is_game_paused = false;
int score;
int snake_len;
COORD snake_tail[MAX_SNAKE_LEN];
COORD tail_end;
enum snake_movement
{
    STOP = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
snake_movement cur_direction = STOP;

//Utitity functions

//Overwrite the characters at given position by resetting cursor position back to {0, 0}
void overwriteMapArea(HANDLE h_console)
{
    COORD map_start;
    map_start.X = 1;
    map_start.Y = 1;
    SetConsoleCursorPosition(h_console, map_start);
}

void cls(HANDLE h_console)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD console_top_left;
    console_top_left.X = 0;
    console_top_left.Y = 0;
    SetConsoleCursorPosition(h_console, console_top_left);
    // GetConsoleScreenBufferInfo(h_console, &csbi);

    //This loop limit obtained by trial and error
    //TODO: Find proper number
    for (int i = 0; i < map_width * 2 * (map_height + 2); i++)
    {
        std::cout << ' ';
    }
    // std::cout << csbi.dwSize.X << "   " << csbi.dwSize.Y;
    SetConsoleCursorPosition(h_console, console_top_left);
}

void ShowConsoleCursor(bool showFlag, HANDLE h_console)
{
    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(h_console, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(h_console, &cursorInfo);
}

// Game Render Functions

void setup()
{
    is_game_over = false;
    score = 0;
    snake_len = 0;
    cur_direction = STOP;
    for (int i = 0; i < map_width / 2 + 2; i++)
    {
        std::cout << "# ";
    }
    std::cout << std::endl;

    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width + 2; j++)
        {
            if (j == 0 || j == map_width + 1)
            {
                std::cout << '#';
            }
            else if (i == fruit.Y && j == fruit.X)
            {
                std::cout << 'X';
            }
            else if (i == snake_head.Y && j == snake_head.X)
            {
                std::cout << "O";
            }
            else
            {
                std::cout << ' ';
            }
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < map_width / 2 + 2; i++)
    {
        std::cout << "# ";
    }
    std::cout << std::endl;
    std::cout << "Score: " << score << std::endl;
}

void draw()
{
    for (int i = 0; i < map_height; i++)
    {
        for (int j = 0; j < map_width; j++)
        {
            if (i == fruit.Y && j == fruit.X)
            {
                std::cout << 'X';
                continue;
            }
            else if (i == snake_head.Y && j == snake_head.X)
            {
                std::cout << "O";
                continue;
            }
            bool is_tail_piece = false;
            for (int k = 0; k < snake_len; k++)
            {
                if (snake_tail[k].X == j && snake_tail[k].Y == i)
                {
                    std::cout << 'o';
                    is_tail_piece = true;
                    break;
                }
            }
            if (!is_tail_piece)
            {
                std::cout << ' ';
            }
        }
        std::cout << std::endl
                  << '#';
    }
    COORD game_status_bar;
    game_status_bar.X = 0;
    game_status_bar.Y = map_height + 2;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), game_status_bar);
    std::cout << "Score: " << score << "\t";
    if (is_game_paused)
    {
        std::cout << "Status: Paused ";
    }
    else
    {
        std::cout << "Status: Running";
    }
}

// Game Logic Functions

void addTailNode(int head_x, int head_y)
{
    if (snake_len == 0)
    {
        switch (cur_direction)
        {
        case UP:
            snake_tail[0].Y = (head_y - 1 + map_height) % map_height;
            snake_tail[0].X = head_x;
            break;
        case LEFT:
            snake_tail[0].X = (head_x - 1 + map_width) % map_width;
            snake_tail[0].Y = head_y;
            break;
        case DOWN:
            snake_tail[0].Y = (head_y + 1) % map_height;
            snake_tail[0].X = head_x;
            break;
        case RIGHT:
            snake_tail[0].X = (head_x + 1) % map_width;
            snake_tail[0].Y = head_y;
            break;
        default:
            break;
        }
        snake_len++;
    }
    else
    {
        for (int i = snake_len; i > 0; i--)
        {
            snake_tail[i].X = snake_tail[i - 1].X;
            snake_tail[i].Y = snake_tail[i - 1].Y;
        }
        snake_tail[0].X = head_x;
        snake_tail[0].Y = head_y;
        snake_len++;
    }
}

void movementAndCollisionDetection()
{
    int current_head_X = snake_head.X;
    int current_head_Y = snake_head.Y;
    switch (cur_direction)
    {
    case STOP:
        is_game_paused = true;
        return;

    case UP:
        snake_head.Y = (snake_head.Y - 1 + map_height) % map_height;
        is_game_paused = false;

        break;

    case DOWN:
        snake_head.Y = (snake_head.Y + 1) % map_height;
        is_game_paused = false;
        break;

    case LEFT:
        snake_head.X = (snake_head.X - 1 + map_width) % map_width;
        is_game_paused = false;
        break;

    case RIGHT:
        snake_head.X = (snake_head.X + 1) % map_width;
        is_game_paused = false;
        break;

    default:
        break;
    }
    for (int i = snake_len; i > 0; i--)
    {
        snake_tail[i].X = snake_tail[i - 1].X;
        snake_tail[i].Y = snake_tail[i - 1].Y;
    }
    snake_tail[0].X = current_head_X;
    snake_tail[0].Y = current_head_Y;
    if (snake_head.X == fruit.X && snake_head.Y == fruit.Y)
    {
        score += 10;
        fruit.X = rand() % map_width;
        fruit.Y = rand() % map_height;
        if (snake_len != MAX_SNAKE_LEN)
        {
            addTailNode(current_head_X, current_head_Y);
        }
        return;
    }
    for (int i = 0; i < snake_len; i++)
    {
        if (snake_head.X == snake_tail[i].X && snake_head.Y == snake_tail[i].Y)
        {
            is_game_over = true;
            return;
        }
    }
}

int main()
{
    HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    bool is_game_exit = false;
    std::cout << "(:  Press any key to begin! (Other than X) :)" << std::endl;
    std::cout << "---------------Press X to exit---------------";
    char ch = _getch();
    if (ch == 'x' || ch == 'X')
    {
        is_game_exit = true;
    }
    while (!is_game_exit)
    {
        // Clear console and set random initial values
        cls(h_console);
        fruit.X = (rand() % map_width) + 1;
        fruit.Y = (rand() % map_height);
        snake_head.X = (rand() % map_width) + 1;
        snake_head.Y = (rand() % map_height);

        // Initial map, fruit and snake head rendering
        setup();

        // Start motion and keep writing to console
        while (!is_game_over)
        {
            ShowConsoleCursor(false, h_console);
            if (_kbhit())
            {
                char ch = _getch();
                switch (ch)
                {
                case 'x':
                case 'X':
                    cur_direction = STOP;
                    cls(h_console);
                    is_game_over = true;
                    break;

                case ' ':
                    cur_direction = STOP;
                    break;

                case 'w':
                case 'W':
                    cur_direction = UP;
                    break;

                case 's':
                case 'S':
                    cur_direction = DOWN;
                    break;

                case 'a':
                case 'A':
                    cur_direction = LEFT;
                    break;

                case 'd':
                case 'D':
                    cur_direction = RIGHT;
                    break;

                default:
                    break;
                }
            }
            if (is_game_over)
                break;
            overwriteMapArea(h_console);
            movementAndCollisionDetection();
            draw();
            Sleep(1000 / FRAMES_PER_SECOND);
        }

        // Game Over Screen
        cls(h_console);
        std::cout << "Game Over :(" << std::endl;
        std::cout << "That snake was teeny! (Press any key to restart)" << std::endl;
        std::cout << "That's enough for now! (Press X to exit)";

        char ch = _getch();
        if (ch == 'x' || ch == 'X')
        {
            is_game_exit = true;
        }
    }
    return 0;
}

/* 
Clear Entire Console Window by taking console dimensions as parameters using handle(garbage collected pointer) to the console window

void cls(HANDLE hConsole)
{
    COORD coordScreen = {0, 0}; // home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer.

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with blanks.

    if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
                                    (TCHAR)' ',      // Character to write to the buffer
                                    dwConSize,       // Number of cells to write
                                    coordScreen,     // Coordinates of first cell
                                    &cCharsWritten)) // Receive number of characters written
    {
        return;
    }

    // Get the current text attribute.

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Set the buffer's attributes accordingly.

    if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute
                                    coordScreen,      // Coordinates of first cell
                                    &cCharsWritten))  // Receive number of characters written
    {
        return;
    }

    // Put the cursor at its home coordinates.

    SetConsoleCursorPosition(hConsole, coordScreen);
}
*/
