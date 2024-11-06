#include <SDL.h>
#include <vector>
#include <iostream>
#include <utility>
#include <ctime>
#include <cstdlib>

using namespace std;

// Define the size of the maze
const int MAZE_WIDTH = 10;
const int MAZE_HEIGHT = 10;
const int WALL_DENSITY = 5; // Lower is denser

// Cell class to represent each cell in the maze
class Cell 
{
public:
    bool isWall;
    Cell() : isWall(false) {}
};

// Maze class to handle maze generation and operations
class Maze 
{
private:
    std::vector<std::vector<Cell> > maze; // Fix the space between '>'
public:
    Maze() 
    {
        maze.resize(MAZE_HEIGHT, std::vector<Cell>(MAZE_WIDTH));
        for (int y = 0; y < MAZE_HEIGHT; ++y) 
        {
            for (int x = 0; x < MAZE_WIDTH; ++x) 
            {
                maze[y][x].isWall = (rand() % WALL_DENSITY == 0); // Randomly place walls
            }
        }
    }

    bool isWall(int x, int y) const 
    {
        return maze[y][x].isWall;
    }

    std::pair<int, int> placeExit() 
    { // Use std::pair
        int exitX, exitY;
        do {
            exitX = rand() % MAZE_WIDTH;
            exitY = rand() % MAZE_HEIGHT;
        } while (isWall(exitX, exitY)); // Ensure exit is not placed on a wall
        return std::make_pair(exitX, exitY); // Use std::make_pair
    }

    void display(SDL_Renderer* renderer) 
    {
        for (int y = 0; y < MAZE_HEIGHT; ++y) 
        {
            for (int x = 0; x < MAZE_WIDTH; ++x) 
            {
                SDL_Rect cellRect = { x * 40, y * 40, 40, 40 };
                if (isWall(x, y)) 
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black for walls
                } 
                else 
                {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for paths
                }
                SDL_RenderFillRect(renderer, &cellRect);
            }
        }
    }
};

// Main game class
class Game 
{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Maze maze;
    SDL_Rect player;
    std::pair<int, int> exitPosition;

public:
    Game() 
    {
        srand(static_cast<unsigned>(time(nullptr))); // Seed random number generator

        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        
        player = SDL_Rect{ 40, 40, 30, 30 }; // Correct initialization for SDL_Rect
        placeExit();
    }

    void placeExit() 
    {
        auto exitCoords = maze.placeExit();
        exitPosition = std::make_pair(exitCoords.first * 40, exitCoords.second * 40); // Use std::make_pair for exitPosition
    }

    void run() 
    {
        bool running = true;
        while (running) {
            processEvents(running);
            update();
            render();
        }
        cleanup();
    }

    void processEvents(bool& running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = false;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W]) movePlayer(0, -1);
        if (state[SDL_SCANCODE_S]) movePlayer(0, 1);
        if (state[SDL_SCANCODE_A]) movePlayer(-1, 0);
        if (state[SDL_SCANCODE_D]) movePlayer(1, 0);
    }

    void update() 
    {
        // Check for win condition
        if (player.x == exitPosition.first && player.y == exitPosition.second) 
        {
            std::cout << "You Win!" << std::endl;
            resetGame();
        }
    }

    void movePlayer(int dx, int dy) 
    {
        int newX = (player.x / 40) + dx;
        int newY = (player.y / 40) + dy;

        if (newX >= 0 && newX < MAZE_WIDTH && newY >= 0 && newY < MAZE_HEIGHT && !maze.isWall(newX, newY))
        {
            player.x += dx * 40;
            player.y += dy * 40;
        }
    }

    // Function to draw a circle
    void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius)
    {
        int x = radius - 1;
        int y = 0;
        int dx = 1;
        int dy = 1;
        int err = dx - (radius << 1);

        while (x >= y)
        {
            SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
            SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
            SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);

            if (err <= 0)
            {
                y++;
                err += dy;
                dy += 2;
            }

            if (err > 0)
            {
                x--;
                dx += 2;
                err += dx - (radius << 1);
            }
        }
    }

    void render() 
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Clear color
        SDL_RenderClear(renderer);
        
        maze.display(renderer);

        // Draw exit
        SDL_Rect exitRect = { exitPosition.first, exitPosition.second, 40, 40 };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for exit
        SDL_RenderFillRect(renderer, &exitRect);

        // Draw the player as a circle
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for player
        drawCircle(renderer, player.x + player.w / 2, player.y + player.h / 2, player.w / 2);

        SDL_RenderPresent(renderer);
    }

    void resetGame() 
    {
        maze = Maze(); // Create a new maze
        placeExit(); // Place a new exit
        player.x = 40; // Reset player position
        player.y = 40;
    }

    void cleanup() 
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
};

int main() 
{
    Game game;
    game.run();
    return 0;
}
