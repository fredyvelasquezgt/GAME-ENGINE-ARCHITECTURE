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

void Game::loadTextures() {
    heartTexture1 = TextureLoader::loadTexture("assets/sprites/heart1.png", renderer);
    heartTexture2 = TextureLoader::loadTexture("assets/sprites/heart2.png", renderer);
    bombTexture = TextureLoader::loadTexture("assets/sprites/bomb.png", renderer);
}

void Game::createEntities() {
    EntityFactory::createHeart(registry, heartTexture1, 100, 100);
    EntityFactory::createHeart(registry, heartTexture2, 200, 100);
    EntityFactory::createBomb(registry, bombTexture, 300, 100);
}

void Game::generateMaze()
{
    mazeWidth = 20;    // Número de columnas del laberinto
    mazeHeight = 15;   // Número de filas del laberinto
    cellSize = 30;     // Tamaño de cada celda del laberinto en píxeles

    // Inicializa la matriz del laberinto con paredes (1)
    maze.resize(mazeHeight, std::vector<int>(mazeWidth, 1));

    // Algoritmo para abrir caminos aleatorios
    srand(static_cast<unsigned int>(time(0))); // Inicializa la semilla aleatoria
    for (int y = 1; y < mazeHeight - 1; y += 2) {
        for (int x = 1; x < mazeWidth - 1; x += 2) {
            maze[y][x] = 0; // Abre un camino
            if (x < mazeWidth - 2 && rand() % 2) maze[y][x + 1] = 0;  // Abre camino a la derecha
            if (y < mazeHeight - 2 && rand() % 2) maze[y + 1][x] = 0;  // Abre camino hacia abajo
        }
    }

    // Crear entrada y salida con un camino directo
    entranceX = 0;
    entranceY = rand() % (mazeHeight / 2);  // Entrada en una fila aleatoria en la mitad superior
    exitX = mazeWidth - 1;    // Salida en la última columna
    exitY = rand() % (mazeHeight / 2) + (mazeHeight / 2); // Salida en una fila aleatoria en la mitad inferior

    // Asegura que la entrada y salida estén libres
    maze[entranceY][entranceX] = 0;
    maze[exitY][exitX] = 0;

    // Generar un camino directo entre la entrada y la salida (construcción de caminos)
    // Crear un camino horizontal directo hacia la salida
    for (int x = entranceX; x <= exitX; x++) {
        maze[entranceY][x] = 0;
    }

    // Crear un camino vertical desde la entrada hacia la salida
    for (int y = entranceY; y <= exitY; y++) {
        maze[y][exitX] = 0;
    }
}

void Game::renderMaze()
{
    for (int y = 0; y < mazeHeight; y++) {
        for (int x = 0; x < mazeWidth; x++) {
            SDL_Rect tileRect = {x * cellSize, y * cellSize, cellSize, cellSize};

            if (maze[y][x] == 1) {
                // Es una pared: crea un patrón de color
                int shade = 50 + (x * 10 + y * 5) % 50;  // Varía el color ligeramente
                SDL_SetRenderDrawColor(renderer, shade, shade, shade, 255); // Gris oscuro con variación
            } else {
                // Es un camino: usa un color claro constante
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Color claro para el camino
            }

            // Dibuja el tile en la pantalla
            SDL_RenderFillRect(renderer, &tileRect);
        }
    }

    // Renderiza la entrada (inicio)
    SDL_Rect entranceRect = {entranceX * cellSize, entranceY * cellSize, cellSize, cellSize};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Verde para la entrada
    SDL_RenderFillRect(renderer, &entranceRect);

    // Renderiza la salida (meta)
    SDL_Rect exitRect = {exitX * cellSize, exitY * cellSize, cellSize, cellSize};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rojo para la salida
    SDL_RenderFillRect(renderer, &exitRect);
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

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);  // Color de fondo
    SDL_RenderClear(renderer);

    renderMaze();  // Renderiza el laberinto

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
    init();
    loadTextures();
    createEntities();
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
