#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
struct app_win {
    bool is_running;
    bool close_request;
    int FPS;
    int current_width;
    int current_height;
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

bool init_sdl_win (int screen_width, int screen_height) {
    app_win app;
    int render_flags, win_flags;
    render_flags = SDL_RENDERER_ACCELERATED;
    win_flags = 0;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Couldn't init SDL", SDL_GetError());
        return 1;
    }
    app.win = SDL_CreateWindow("Pong",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, win_flags);
    if(!app.win) {
        printf("Failed to open %d x %d window: %s\n", screen_width, screen_height, SDL_GetError());
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    app.renderer = SDL_CreateRenderer(app.win, -1, render_flags);
    
    if(!app.renderer) {
        printf("Failed to create Renderer: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}

/*
bool handle_events(bool game_state, SDL_Event event) {
    while(SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT :
                game_state = 1;
                printf("Game will end");
                break;
        }
        if()
    }
    return game_state;
}
 */
int main() {
    const SDL_MessageBoxButtonData buttons[] = {
            { /* .flags, .buttonid, .text */        0, 0, "640x480" },
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
    const SDL_MessageBoxData messageboxdata = {
            SDL_MESSAGEBOX_INFORMATION, /* .flags */
            nullptr, /* window */
            "Set screen resolution", /*  title of menu box */
            "Pick a Resolution of your choice :", /* Display message */
            SDL_arraysize(buttons), /* Number of Buttons */
            buttons, /* .buttons */
            &colorScheme /* .colorScheme */
    };
    int button_id;

    if (SDL_ShowMessageBox(&messageboxdata, &button_id) < 0) {
        SDL_Log("error displaying message box");
        return 1;
    }
 /*   else
    {
        printf("resolution is %d x %d", resolutions[button_id].width, resolutions[button_id].height);
    }
    if (button_id == -1) {
        SDL_Log("no resolution was picked");
    }
    */
   bool keys[322]; // number of ALL SDL_KEYDOWN Events
    for(int key = 0; key < 322; key++) {
        keys[key] = false; // initialize them to false
    }
    SDL_Event ev;
  // bool is_running =  init_sdl_win(resolutions[button_id].width, resolutions[button_id].height);
   // while(is_running) {
        //handle_events(is_running, ev);
       // }
    printf("\nExecution ended");
    return 0;
}
