#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
struct window {
    int FPS;
    int width;
    int height;
    SDL_Renderer *renderer;
    SDL_Window *win;
};
struct {
    int width;
    int height;
} resolutions[] = {
                {640, 480},
                {800, 600},
                {1280, 720}
};

bool init_sdl_win (SDL_Renderer *renderer, SDL_Window *win, int screen_width, int screen_height) {
    int render_flags, win_flags;
    render_flags = SDL_RENDERER_ACCELERATED;
    win_flags = 0;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't init SDL", SDL_GetError());
        return false;
    }
    win = SDL_CreateWindow("Pong",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, win_flags);
    if(!win) {
        printf("Failed to open %d x %d window: %s\n", screen_width, screen_height, SDL_GetError());
        return false;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    renderer = SDL_CreateRenderer(win, -1, render_flags);
    
    if(!renderer) {
        printf("Failed to create Renderer: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void handle_input(bool *state) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_QUIT : *state = false;
                break;
        }

    }
}


int main() {
    const SDL_MessageBoxButtonData buttons[] = {
            { /* .flags, .button id, .text */        0, 0, "640x480" },
            { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "800x600" },
            { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "1280x720" },
    };
    const SDL_MessageBoxColorScheme colorScheme = {
            { /* .colors (.r, .g, .b) */
                    /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
                    { 255,   0,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_TEXT] */
                    {   0, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
                    { 255, 255,   0 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
                    {   0,   0, 255 },
                    /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
                    { 255,   0, 255 }
            }
    };
    const SDL_MessageBoxData message_box_data = {
            SDL_MESSAGEBOX_INFORMATION, /* .flags */
            nullptr, /* window */
            "Set screen resolution", /*  title of menu box */
            "Pick a Resolution of your choice :", /* Display message */
            SDL_arraysize(buttons), /* Number of Buttons */
            buttons, /* .buttons */
            &colorScheme /* .colorScheme */
    };
    int button_id;

    if (SDL_ShowMessageBox(&message_box_data, &button_id) < 0) {
        SDL_Log("error displaying message box");
    }
    else
    {
        printf("resolution is %d x %d", resolutions[button_id].width, resolutions[button_id].height);
    }
    if (button_id == -1) {
        SDL_Log("no resolution was picked");
    }

    window application;
    application.width = resolutions[button_id].width;
    application.height = resolutions[button_id].height;
    bool is_running =  init_sdl_win(application.renderer, application.win, application.width, application.height);
    while(is_running) {
        //for some reason the handle input function doesn't want to poll the events.
        handle_input(&is_running);
        SDL_SetRenderDrawColor(application.renderer, 255,0,0,255);
        SDL_RenderPresent(application.renderer);
        SDL_Delay(1000/application.FPS);
    }
    SDL_DestroyRenderer(application.renderer);
    SDL_DestroyWindow(application.win);
    SDL_Quit();
    
    printf("\nExecution ended");
    return 0;
}
