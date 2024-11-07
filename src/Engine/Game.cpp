#include "Game.h"
#include <sstream>
#include <SDL2/SDL.h>
#include <cmath>    // Para efectos de onda y color
#include <vector>   // Para almacenar el laberinto
#include <cstdlib>  // Para generación de números aleatorios
#include <ctime>    // Para la semilla aleatoria

Game::Game(const char* title, int width, int height)
{
    int maxFPS = 60;
    frameDuration = (1.0f / maxFPS) * 1000.0f;  // milisegundos por fotograma

    // Inicialización de variables de conteo de cuadros
    frameCount = 0;
    lastFPSUpdateTime = 0;
    FPS = 0;

    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow(title, 0, 0, width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);

    screen_width = width;
    screen_height = height;

    isRunning = true;

    frameStartTimestamp = 0;
    frameEndTimestamp = 0;

    currentScene = nullptr;

    generateMaze(); // Genera el laberinto al iniciar el juego
}

Game::~Game()
{
    clean();
}

void Game::generateMaze()
{
    mazeWidth = 20;    // Número de columnas del laberinto
    mazeHeight = 15;   // Número de filas del laberinto
    cellSize = 30;     // Tamaño de cada celda del laberinto en píxeles

    // Inicializa la matriz del laberinto con paredes (1)
    maze.resize(mazeHeight, std::vector<int>(mazeWidth, 1));

    // Algoritmo simple para abrir caminos aleatorios
    srand(static_cast<unsigned int>(time(0))); // Inicializa la semilla aleatoria
    for (int y = 1; y < mazeHeight - 1; y += 2) {
        for (int x = 1; x < mazeWidth - 1; x += 2) {
            maze[y][x] = 0; // Abre un camino
            if (x < mazeWidth - 2 && rand() % 2) maze[y][x + 1] = 0;  // Abre camino a la derecha
            if (y < mazeHeight - 2 && rand() % 2) maze[y + 1][x] = 0;  // Abre camino hacia abajo
        }
    }
}

void Game::renderMaze()
{
    for (int y = 0; y < mazeHeight; y++) {
        for (int x = 0; x < mazeWidth; x++) {
            if (maze[y][x] == 1) { // Es una pared
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Color gris para paredes
            } else {
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Color claro para caminos
            }
            SDL_Rect cellRect = {x * cellSize, y * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
}

void Game::frameStart()
{
    frameStartTimestamp = SDL_GetTicks();
    if (frameEndTimestamp)
    {
        dT = (frameStartTimestamp - frameEndTimestamp) / 1000.0f;
    }
    else
    {
        dT = 0;
    }
}

void Game::frameEnd()
{
    frameEndTimestamp = SDL_GetTicks();

    float actualFrameDuration = frameEndTimestamp - frameStartTimestamp;

    if (actualFrameDuration < frameDuration) {
        SDL_Delay(frameDuration - actualFrameDuration);
    }

    frameCount++;
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFPSUpdateTime > 1000) {
        FPS = frameCount / ((currentTime - lastFPSUpdateTime) / 1000.0f);
        lastFPSUpdateTime = currentTime;

        if (FPS > 0) {
            std::ostringstream titleStream;
            titleStream << " FPS: " << static_cast<int>(FPS);
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
        frameCount = 0;
    }
}

void Game::handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (currentScene != nullptr) {
            currentScene->processEvents(event);
        }
    }
}

void Game::update()
{
    if (currentScene != nullptr) {
        currentScene->update(dT);
    }
}

void Game::render()
{
    // Efecto de fondo dinámico con gradiente
    float time = SDL_GetTicks() / 1000.0f;
    int r = static_cast<int>((sin(time * 0.5f) + 1.0f) * 127);
    int g = static_cast<int>((cos(time * 0.7f) + 1.0f) * 127);
    int b = static_cast<int>((sin(time * 0.3f) + 1.0f) * 127);

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);  // Establece el color de fondo
    SDL_RenderClear(renderer);

    renderMaze();  // Renderiza el laberinto como fondo

    if (currentScene != nullptr) {
        currentScene->render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool Game::running()
{
    return isRunning;
}

void Game::run()
{
    if (currentScene == nullptr) {
        exit(1);
    }

    while (running() && currentScene != nullptr)
    {
        frameStart();
        handleEvents();
        update();
        render();
        frameEnd();
    }

    clean();
}

void Game::setScene(Scene* newScene) {
    if (newScene != nullptr) {
        newScene->setup();
    }
    currentScene = newScene;
}

Scene* Game::getCurrentScene() const {
    return currentScene;
}
