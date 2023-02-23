#include<SDL_render.h>
#include<SDL_log.h>
#include<SDL_events.h>
//SDL2 window wrapper
//
class nano{
    public:
    SDL_Event eve;
    SDL_Window *p_window;
    SDL_Renderer *p_renderer;
    nano(const char *title){
        p_window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                1200,600,
                SDL_WINDOW_RESIZABLE//|SDL_WINDOW_FULLSCREEN_DESKTOP
                );
        if(!p_window)
            SDL_Log("The window could not be created");
        else{
            p_renderer = SDL_CreateRenderer(p_window,-1,SDL_RENDERER_SOFTWARE);
            if(!p_renderer)
                SDL_Log("The Renderer could not be created");
        }
    }
    
    void close();
    void render();
    SDL_Point getSize();
    void process(bool(*execute)(const SDL_Event*));
    bool fullscreen(){
        return p_fullscreen;
    }
    ~nano(){
        close();
    }

    private : bool p_fullscreen = false;
};
// process window events
void nano::process(bool(*execute)(const SDL_Event*)=nullptr){
    while(SDL_PollEvent(&eve)){
            if(execute!=nullptr)execute(&eve);
            switch(eve.type){
                case SDL_QUIT:
                    close();
                case SDL_KEYDOWN:
                {
                    const Uint8* key = SDL_GetKeyboardState(NULL);
                    if(key[SDL_SCANCODE_F11]){
                        SDL_SetWindowFullscreen(p_window,p_fullscreen?0:SDL_WINDOW_FULLSCREEN_DESKTOP);
                        p_fullscreen=!p_fullscreen;
                    }
                }
                default:
                    break;
            }
        }
}
// free resources
void nano::close(){
    SDL_DestroyWindow(p_window);
    SDL_DestroyRenderer(p_renderer); 
    p_window   = nullptr;
    p_renderer = nullptr;
}

// render present and then clear
void nano::render(){
    SDL_RenderPresent(p_renderer);
    SDL_RenderClear(p_renderer);
}
SDL_Point nano::getSize(){
    int w, h;
    SDL_GetWindowSize(p_window,&w,&h);
    return SDL_Point{w,h};
}