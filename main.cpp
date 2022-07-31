#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *win;
} app_win;

void init_sdl (void) {
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;
    app_win app;
    int render_flags, win_flags;
    render_flags = SDL_RENDERER_ACCELERATED;
    win_flags = 0;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't init SDL", SDL_GetError());
        exit(1);
    }
    app.win = SDL_CreateWindow("Pong",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, win_flags);
    if(!app.win) {
        printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
        exit(1);
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    app.renderer = SDL_CreateRenderer(app.win, -1, render_flags);
    
    if(!app.renderer) {
        printf("Failed to create Renderer: %s\n", SDL_GetError());
        exit(1);
    }
}

int main() {

}
