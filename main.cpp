#include <SDL2/SDL.h>
#include <SDL2/SDL_hints.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <algorithm>
//#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>

struct Window {
    int width{};
    int height{};
    SDL_Renderer* renderer = nullptr;
    SDL_Window* win = nullptr;
//    TTF_Font* score_font = nullptr;
};

struct Inputs {
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

struct Vec2f {
    float x;
    float y;
};

struct Ball {
    Vec2f pos;
    Vec2f dir;
    float radius;
    float speed;

};

enum collision_type {
    None,
    Top,
    Middle,
    Bottom,
    Left,
    Right
};

struct Contact {
   collision_type type;
   float penetration;
};

bool init_sdl_win (SDL_Renderer *&renderer, SDL_Window *&win, int screen_width, int screen_height/*, TTF_Font *&score_font*/) {
    int render_flags, win_flags;
    render_flags = SDL_RENDERER_ACCELERATED;
    win_flags = 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("Couldn't init SDL: %s", SDL_GetError());
        return false;
    }
/*
    TTF_Init();
    //create font
    score_font = TTF_OpenFont("DejaVuSans.ttf",40);
    if (TTF_Init() == -1) {
        printf("Couldn't initialize TTF");
        return false;
    }
    */
    win = SDL_CreateWindow("Pong",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, win_flags);
    if(!win) {
        printf("Failed to open %d x %d window: %s\n", screen_width, screen_height, SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(win, -1, render_flags);

    if(!renderer) {
        printf("Failed to create Renderer: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void collide_with_paddle(Ball &ball,Contact const& contact) {
    ball.pos.x += contact.penetration;
    ball.dir.x = -ball.dir.x;
    if(contact.type == collision_type::Top) {
        ball.dir.y = -0.75f * ball.speed;
    } else if (contact.type == collision_type::Bottom) {
        ball.dir.y = 0.75f * ball.speed;
    }
}

Contact check_ball_wall_collision (Ball const &ball, int screen_height, int screen_width) {
    float ball_left = ball.pos.x;
    float ball_right = ball.pos.x + (ball.radius * 3);
    float ball_top = ball.pos.y;
    float ball_bottom = ball.pos.y + (ball.radius * 3);

    Contact contact{};
    if(ball_left < 0.0f) {
        contact.type = collision_type::Left;
    } else if (ball_right > static_cast<float>(screen_width)) {
        contact.type = collision_type::Right;
    } else if (ball_top < 0.0f) {
        contact.type = collision_type::Top;
        contact.penetration = -ball_top;
    } else if(ball_bottom > static_cast<float>(screen_height)) {
        contact.type = collision_type::Bottom;
        contact.penetration = static_cast<float>(screen_height) - ball_bottom;
    }
    return contact;
}

Contact check_paddle_collision (Ball const &ball, SDL_Rect const& paddle) {
    //using SAT instead of AABB collision
    float ball_left = ball.pos.x;
    float ball_right = ball.pos.x + (ball.radius * 3);
    float ball_top = ball.pos.y;
    float ball_bottom = ball.pos.y + (ball.radius * 3);

    auto paddle_left = static_cast<float>(paddle.x);
    auto paddle_right = static_cast<float>(paddle.x) + static_cast<float>(paddle.w);
    auto paddle_top = static_cast<float>(paddle.y);
    float paddle_bottom = static_cast<float>(paddle.y) + static_cast<float>(paddle.h);

    Contact contact{};

    if( ball_left >= paddle_right) {
        return contact;
    }
    if(ball_right <= paddle_left) {
        return contact;
    }
    if(ball_top >= paddle_bottom) {
        return contact;
    }
    if(ball_bottom <= paddle_top) {
        return contact;
    }
    float paddle_range_upper = paddle_bottom - (2.0f * static_cast<float>(paddle.h) / 3.0f);
    float paddle_range_middle = paddle_bottom - (static_cast<float>(paddle.h) / 3.0f);

    if(ball.dir.x < 0) {
        contact.penetration = paddle_right - ball_left;
    } else if (ball.dir.x > 0) {
        contact.penetration = paddle_left - ball_right;
    }
    if ((ball_bottom > paddle_top) &&
        (ball_bottom < paddle_range_upper)) {
        contact.type = collision_type::Top;
    }
    else if ((ball_bottom > paddle_range_upper) &&
             (ball_bottom < paddle_range_middle)) {
        contact.type = collision_type::Middle;
    } else {
        contact.type = collision_type::Bottom;
    }
    return contact;
}

void collide_with_wall (Contact const &contact, Ball &ball, int screen_width, int screen_height) {
    if((contact.type == collision_type::Top) || contact.type == collision_type::Bottom) {
        ball.pos.y = contact.penetration;
        ball.dir.x = -ball.dir.x;
    }  else if (contact.type == collision_type::Left) {
        ball.pos.x = static_cast<float>(screen_width) / 2.0F;
        ball.pos.y = static_cast<float>(screen_height) / 2.0F;
        ball.dir.x = ball.speed;
        ball.dir.y = 0.75f * ball.speed;
    } else if(contact.type == collision_type::Right) {
        ball.pos.x = static_cast<float>(screen_width) / 2.0F;
        ball.pos.y = static_cast<float>(screen_height) / 2.0F;
        ball.dir.x = -ball.speed;
        ball.dir.y = 0.75f * ball.speed;
    }
}

void check_player_collision(Ball &ball, SDL_Rect &paddle_p1, SDL_Rect &paddle_p2, int screen_height, int screen_width) {
    Contact contact_l = check_paddle_collision(ball, paddle_p1);
    if (contact_l.type != collision_type::None)
    {
        collide_with_paddle(ball, contact_l);
        return;
    }

    contact_l = check_paddle_collision(ball, paddle_p2);
    if (contact_l.type != collision_type::None)
    {
        collide_with_paddle(ball, contact_l);
        return;
    }

    contact_l = check_ball_wall_collision(ball, screen_height, screen_width);
    if (contact_l.type != collision_type::None)
    {
        collide_with_wall(contact_l,ball,screen_width, screen_height);
        return;
    }
}

void move_ball(Ball &ball, float dt) {
    ball.pos.x += ball.dir.x * dt * ball.speed;
    ball.pos.y += ball.dir.y * dt * ball.speed;

}

void move_player(Inputs key_press, float *player_y_pos, float player_speed, float delta_time) {
    char dir = 0;
    if (key_press.UP)   dir -= 1;
    if (key_press.DOWN) dir += 1;
    *player_y_pos += static_cast<float>(dir) * (player_speed * delta_time);
}

void handle_input(bool *is_running, Inputs *key_press) {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:   key_press->UP   = (event.type == SDL_KEYDOWN); break;
                case SDLK_DOWN: key_press->DOWN = (event.type == SDL_KEYDOWN); break;
            }
        }

        if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            *is_running = false;
        }
    }
}

void destroy_window(SDL_Renderer *renderer, SDL_Window *win/*, TTF_Font *score_font*/) {
    SDL_DestroyRenderer(renderer);
    //TTF_CloseFont(score_font);
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

void draw_player(SDL_Renderer *renderer, SDL_Rect *player) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, player);
}

void draw_ball(SDL_Renderer *renderer, const Ball &ball) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect ball_rect = {
            (int)(ball.pos.x - ball.radius),
            (int)(ball.pos.y - ball.radius),
            (int)(ball.radius * 3),
            (int)(ball.radius * 3)
    };

    SDL_RenderFillRect(renderer, &ball_rect);
}

float clamp(float value, float min, float max) {
    return std::min(std::max(value, min), max);
}

void player_wall_collision(int screen_height, float &player_y, int paddle_height) {
    player_y = clamp(player_y, 0.0f, static_cast<float>(screen_height) - static_cast<float>(paddle_height));
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
    bool is_running =  init_sdl_win(application.renderer, application.win, application.width, application.height/*, application.score_font*/);

    //player params
    const int player_1_start_x_pos = 50;
    const int player_start_y_pos = application.height / 2;
    const int paddle_height = 75;
    const int paddle_width = 15;
    const int player_2_start_x_pos = application.width - player_1_start_x_pos;

    //ball params
    const int ball_start_x_pos = application.width / 2;
    const int ball_start_y_pos = application.height / 2;

    SDL_Rect p1 = init_player(player_1_start_x_pos, player_start_y_pos - (paddle_height / 2), paddle_height, paddle_width);
    SDL_Rect p2 = init_player(player_2_start_x_pos, player_start_y_pos - (paddle_height / 2), paddle_height, paddle_width);

    const float PADDLE_SPEED = 500.0f;
    float BALL_SPEED = PADDLE_SPEED / 6.0f;
    Inputs keys = {};

    auto player_y = static_cast<float>(player_start_y_pos);

    Ball ball = { {(float)ball_start_x_pos, (float)ball_start_y_pos}, {-1.0f, 0.0f}, 5.0f, BALL_SPEED };

    Uint32 now = SDL_GetTicks();
    srand(time(nullptr));
    while (is_running) {
        Uint32 last = now;
        now = SDL_GetTicks();
        const float dt = (static_cast<float>(now - last)) / 1000.0f;

        handle_input(&is_running, &keys);
        p1.y = static_cast<int>(player_y);
        move_player(keys, &player_y, PADDLE_SPEED, dt);
        move_ball(ball, dt);
        check_player_collision(ball, p1, p2, application.height, application.width);

        player_wall_collision(application.height, player_y, p1.h);
        SDL_SetRenderDrawColor(application.renderer, 0, 0, 0, 255);
        SDL_RenderClear(application.renderer);
        draw_player(application.renderer, &p1);
        draw_player(application.renderer, &p2);
        draw_ball(application.renderer, ball);

        SDL_RenderPresent(application.renderer);
    }
    destroy_window(application.renderer, application.win/*, application.score_font*/);

    return 0;
}
