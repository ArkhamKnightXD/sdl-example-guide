#include "sdl_starter.h"
#include "sdl_assets_loader.h"
#include <time.h>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController *controller = nullptr;

Mix_Chunk *actionSound = nullptr;
Mix_Music *music = nullptr;

Sprite playerSprite;

const int PLAYER_SPEED = 600;

bool isGamePaused;
bool shouldClearScreen;

SDL_Texture *pauseTexture = nullptr;
SDL_Rect pauseBounds;

SDL_Texture *scoreTexture = nullptr;
SDL_Rect scoreBounds;

int score;

int gameStatus = -1;

TTF_Font *fontSquare = nullptr;

SDL_Rect ball = {SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2, 32, 32};

int ballVelocityX = 400;
int ballVelocityY = 400;

int colorIndex;

SDL_Color colors[] = {
    {128, 128, 128, 0}, // gray
    {255, 255, 255, 0}, // white
    {255, 0, 0, 0},     // red
    {0, 255, 0, 0},     // green
    {0, 0, 255, 0},     // blue
    {255, 255, 0, 0},   // brown
    {0, 255, 255, 0},   // cyan
    {255, 0, 255, 0},   // purple
};

void quitGame()
{
    Mix_FreeMusic(music);
    Mix_FreeChunk(actionSound);
    SDL_DestroyTexture(playerSprite.texture);
    SDL_DestroyTexture(pauseTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
        {
            quitGame();
            exit(0);
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE)
        {
            shouldClearScreen = !shouldClearScreen;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
        {
            isGamePaused = !isGamePaused;
            Mix_PlayChannel(-1, actionSound, 0);
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_KP_PLUS && gameStatus < 4)
        {
            gameStatus++;
        }

        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_KP_MINUS && gameStatus > -1)
        {
            gameStatus--;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
        {
            shouldClearScreen = !shouldClearScreen;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
        {
            isGamePaused = !isGamePaused;
            Mix_PlayChannel(-1, actionSound, 0);
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER && gameStatus < 4)
        {
            gameStatus++;
        }

        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER && gameStatus > 0)
        {
            gameStatus--;
        }
    }
}

int rand_range(int min, int max)
{
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void update(float deltaTime)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_R])
    {
        score = 0;

        playerSprite.textureBounds.w = 38;
        playerSprite.textureBounds.h = 34;
    }

    if (currentKeyStates[SDL_SCANCODE_LEFT])
    {
        playerSprite.textureBounds.w--;
    }

    else if (currentKeyStates[SDL_SCANCODE_RIGHT])
    {
        playerSprite.textureBounds.w++;
    }

    if (currentKeyStates[SDL_SCANCODE_UP])
    {
        playerSprite.textureBounds.h--;
    }

    else if (currentKeyStates[SDL_SCANCODE_DOWN])
    {
        playerSprite.textureBounds.h++;
    }

    if (currentKeyStates[SDL_SCANCODE_W] && playerSprite.textureBounds.y > 0)
    {
        playerSprite.textureBounds.y -= PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_S] && playerSprite.textureBounds.y < SCREEN_HEIGHT - playerSprite.textureBounds.h)
    {
        playerSprite.textureBounds.y += PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_A] && playerSprite.textureBounds.x > 0)
    {
        playerSprite.textureBounds.x -= PLAYER_SPEED * deltaTime;
    }

    else if (currentKeyStates[SDL_SCANCODE_D] && playerSprite.textureBounds.x < SCREEN_WIDTH - playerSprite.textureBounds.w)
    {
        playerSprite.textureBounds.x += PLAYER_SPEED * deltaTime;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_BACK))
    {
        score = 0;

        playerSprite.textureBounds.w = 38;
        playerSprite.textureBounds.h = 34;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_X))
    {
        playerSprite.textureBounds.w--;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_B))
    {
        playerSprite.textureBounds.w++;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_Y))
    {
        playerSprite.textureBounds.h--;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A))
    {
        playerSprite.textureBounds.h++;
    }

    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && playerSprite.textureBounds.y > 0)
    {
        playerSprite.textureBounds.y -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && playerSprite.textureBounds.y < SCREEN_HEIGHT - playerSprite.textureBounds.h)
    {
        playerSprite.textureBounds.y += PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && playerSprite.textureBounds.x > 0)
    {
        playerSprite.textureBounds.x -= PLAYER_SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && playerSprite.textureBounds.x < SCREEN_WIDTH - playerSprite.textureBounds.w)
    {
        playerSprite.textureBounds.x += PLAYER_SPEED * deltaTime;
    }

    if (ball.x < 0 || ball.x > SCREEN_WIDTH - ball.w)
    {
        ballVelocityX *= -1;

        colorIndex = rand_range(0, 5);
    }

    else if (ball.y < 0 || ball.y > SCREEN_HEIGHT - ball.h)
    {
        ballVelocityY *= -1;

        colorIndex = rand_range(0, 5);
    }

    else if (SDL_HasIntersection(&playerSprite.textureBounds, &ball))
    {
        ballVelocityX *= -1;
        ballVelocityY *= -1;

        colorIndex = rand_range(0, 5);

        if (gameStatus > 1)
        {
            Mix_PlayChannel(-1, actionSound, 0);
        }

        if (gameStatus > 2)
        {
            score++;

            std::string scoreString = "score: " + std::to_string(score);

            updateTextureText(scoreTexture, scoreString.c_str(), fontSquare, renderer);
        }
    }

    ball.x += ballVelocityX * deltaTime;
    ball.y += ballVelocityY * deltaTime;
}

void renderSprite(Sprite &sprite)
{
    SDL_RenderCopy(renderer, sprite.texture, NULL, &sprite.textureBounds);
}

void render()
{
    if (shouldClearScreen)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    if (gameStatus > -1 && gameStatus < 4)
    {
        SDL_RenderFillRect(renderer, &playerSprite.textureBounds);
    }

    SDL_SetRenderDrawColor(renderer, colors[colorIndex].r, colors[colorIndex].g, colors[colorIndex].b, 255);

    if (gameStatus > 0)
    {
        SDL_RenderFillRect(renderer, &ball);
    }

    if (gameStatus > 3)
    {
        renderSprite(playerSprite);
    }

    if (isGamePaused)
    {
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseBounds);
    }

    if (gameStatus > 2)
    {
        SDL_QueryTexture(scoreTexture, NULL, NULL, &scoreBounds.w, &scoreBounds.h);
        scoreBounds.x = 500;
        scoreBounds.y = scoreBounds.h / 2 - 10;
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreBounds);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *args[])
{
    window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (startSDL(window, renderer) > 0)
    {
        return 1;
    }

    if (SDL_NumJoysticks() < 1)
    {
        printf("No game controllers connected!\n");
        // return -1;
    }
    else
    {
        controller = SDL_GameControllerOpen(0);
        if (controller == NULL)
        {
            printf("Unable to open game controller! SDL Error: %s\n", SDL_GetError());
            return -1;
        }
    }

    fontSquare = TTF_OpenFont("res/fonts/square_sans_serif_7.ttf", 36);

    updateTextureText(scoreTexture, "Score: 0", fontSquare, renderer);

    updateTextureText(pauseTexture, "Game Paused", fontSquare, renderer);

    SDL_QueryTexture(pauseTexture, NULL, NULL, &pauseBounds.w, &pauseBounds.h);
    pauseBounds.x = SCREEN_WIDTH / 2 - pauseBounds.w / 2;
    pauseBounds.y = 100;

    playerSprite = loadSprite(renderer, "res/sprites/alien_1.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    actionSound = loadSound("res/sounds/magic.wav");

    Mix_VolumeChunk(actionSound, MIX_MAX_VOLUME / 10);

    music = loadMusic("res/music/music.wav");

    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

    // Mix_PlayMusic(music, -1);

    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime = previousFrameTime;
    float deltaTime = 0.0f;

    while (true)
    {
        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f;
        previousFrameTime = currentFrameTime;

        SDL_GameControllerUpdate();

        handleEvents();

        if (!isGamePaused)
        {
            update(deltaTime);
        }

        render();

        // capFrameRate(currentFrameTime);
    }
}