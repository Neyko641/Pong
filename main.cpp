#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <stdio.h>

struct Window {
    int FPS;
    int width;
    int height;
    SDL_Renderer *renderer;
    SDL_Window *win;
};

struct inputs {
    bool UP;
    bool DOWN;
    bool PAUSE;
};

struct {
    int width;
    int height;
} Resolutions[] = {
                {640, 480},
                {800, 600},
                {1280, 720}
};



bool init_sdl_win (SDL_Renderer *&renderer, SDL_Window *&win, int screen_width, int screen_height) {
    int render_flags, win_flags;
    render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    win_flags = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
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
void move_player(inputs *key_press, int *player_y_pos, int player_speed, float delta_time) {
    short int dir;
    if (key_press->UP == true) {
        dir = -1;
    } else if (key_press->DOWN == true) {
        dir = 1;
    }
    *player_y_pos += dir * (player_speed * delta_time);
}

void handle_input(bool *state, int *player_y_pos, inputs *key_press, int player_speed, float dt) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            *state = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    key_press->UP = true;
                    break;
                case SDLK_DOWN:
                    key_press->DOWN = true;
                    break;
                case SDLK_ESCAPE:
                    *state = false;
                    break;
                default:
                    break;
            }
        } else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_UP :
                    key_press->UP = false;
                    break;
                case SDLK_DOWN :
                    key_press->DOWN = false;
                    break;
                case SDLK_ESCAPE:
                    break;
                default:
                    break;
            }
        }
    }
    move_player(key_press, player_y_pos, player_speed, dt);
}

void destroy_window(SDL_Renderer *renderer, SDL_Window *win) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

SDL_Rect init_player(int paddle_x_pos, int paddle_y_pos, int paddle_height, int paddle_width) {
    SDL_Rect player;
    player.h = paddle_height;
    player.w = paddle_width;
    player.x = paddle_x_pos;
    player.y = paddle_y_pos;

    return player;
}
void draw_player(SDL_Renderer *&renderer, SDL_Rect player) {
    SDL_RenderDrawRect(renderer, &player);
    SDL_RenderFillRect(renderer, &player);
}

void player_wall_collision (int screen_height, int *player_y_pos, int paddle_height) {
    const int top_paddle_hit_box = screen_height;
    const int bottom_paddle_hit_box = 75;
    const int max_top_pos = screen_height - 75;
    const int max_bot_pos = screen_height - screen_height + 5;
    if(*player_y_pos + paddle_height >= top_paddle_hit_box) {
        *player_y_pos = max_top_pos;
    } else if (*player_y_pos + paddle_height <= bottom_paddle_hit_box) {
        *player_y_pos =  max_bot_pos;
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
    /*
    else
    {
        printf("resolution is %d x %d", Resolutions[button_id].width, Resolutions[button_id].height);
    }
    if (button_id == -1) {
        SDL_Log("no resolution was picked");
    }
*/
    Window application;
    application.width = Resolutions[button_id].width;
    application.height = Resolutions[button_id].height;

    bool is_running =  init_sdl_win(application.renderer, application.win, application.width, application.height);

    const int player_1_start_x_pos = 20;
    const int player_start_y_pos = application.height / 2;
    const int player_height = 70;
    const int player_width = 15;
    const int player_2_start_x_pos = application.width - ((player_1_start_x_pos * 2));
    SDL_Rect p1 = init_player(player_1_start_x_pos, player_start_y_pos, player_height, player_width);
    SDL_Rect p2 = init_player(player_2_start_x_pos, player_start_y_pos, player_height, player_width);
    const int SPEED = 1250;

    inputs keys;
    while (is_running) {
        Uint32 start = SDL_GetTicks();
        SDL_RenderClear(application.renderer);
        SDL_SetRenderDrawColor(application.renderer, 255,255,255,255);
        draw_player(application.renderer, p1);
        draw_player(application.renderer, p2);
        SDL_SetRenderDrawColor(application.renderer, 0, 0, 0, 255);
        SDL_RenderPresent(application.renderer);
        Uint32 end = SDL_GetTicks();
        float elapsed_seconds = (end - start) / 1000.0F;
        handle_input(&is_running,&p1.y, &keys, SPEED, elapsed_seconds);
        player_wall_collision(application.height, &p1.y, p1.h);
        //SDL_Delay(1000/application.FPS);
    }
    destroy_window(application.renderer, application.win);

    return 0;
}
