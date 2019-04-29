#include "game.h"
#include <iostream>
using namespace std;
typedef mulan_missle mlm;
char *missile_code;
typename game::fp game::G_cy[GCNUM]={&game::eve,&game::drawR};
game::game(): width            (800), height           (600),
              pWindow      (nullptr), pRenderer   (nullptr),
              bGameIsRunning  (true),
              FPS              (100), FramePerSec (1000/FPS),
              debug           (false), super_mode    (false),
              V          (750.0/FPS), mlm_eve             (),
              players      (nullptr),
              StartingTime        (), EndingTime          (),
              ElapsedMicroseconds (), Frequency           (),
              FACTORY (factory::get_factory())
{
    players=FACTORY->get_player();
    if(!initSDL())
        cout << "Init SDL failed.\n"<<SDL_GetError()<<'\n';
    missile_code=new char;
    missile_code[0]='n';
}
game::~game()
{
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    delete players;
    delete FACTORY;
    mlm_eve.clear();
    delete [] missile_code;
    DEBUG(factory::check_products();)
}
void game::game_event()
{
    QueryPerformanceFrequency(&Frequency);

    for(int current=0;
		bGameIsRunning;
		current++,
		current%=GCNUM,
		GetTicks())
        QueryPerformanceCounter(&StartingTime),(this->*(game::G_cy[current]))();
}
bool game::initSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        pWindow = SDL_CreateWindow("Your \"Ball\"",                      // �������D
                                 SDL_WINDOWPOS_UNDEFINED,    // X �y��
                                 SDL_WINDOWPOS_UNDEFINED,    // Y �y��
                                 width, height,                                         // �e�B��
                                 SDL_WINDOW_SHOWN|SDL_RENDERER_PRESENTVSYNC);               // ��L�]�w�X��
        if(pWindow != NULL)
        {
            pRenderer = SDL_CreateRenderer(pWindow,
                                     -1,                                                           // �ϥβĤ@�Ӥ䴩��driver
                                     SDL_RENDERER_ACCELERATED); // �ϥεw��[�t
            if(pRenderer != NULL)
            {
                return true;
            }
        }
    }
    return false;
}
inline void game::GetTicks()
{
    do{
            QueryPerformanceCounter(&EndingTime);
            ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
            ElapsedMicroseconds.QuadPart *= 1000000;
            ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    }while(ElapsedMicroseconds.QuadPart <= (FramePerSec*1000));  // ms
}
void game::eve()
{
    uncon_key_eve();
    con_key_eve();
    mouse_eve();
}
void game::drawR()
{
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 127, 0);  // R, G, B, A
    SDL_RenderClear(pRenderer);
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 127, 0);// R, G, B, A
    for(vector<object>::iterator t=mlm_eve.begin(); t!=mlm_eve.end();)
    {
        t->draw(pRenderer);
        t->update(V);
        if(t->is_out_of_boarder(width,height))
            t=mlm_eve.erase(t);
        else
            ++t;
    }
    players->draw(pRenderer);
    // �N�I�������Ϥ��A��ܨ�e��
    SDL_RenderPresent(pRenderer);
}
inline void game::uncon_key_eve()
{
    static bool pause=0;
    SDL_Event event;

    uncon_key_event:

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            bGameIsRunning = false;
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    bGameIsRunning = false;
                    break;

                case SDLK_r:
                    players->pos_reset(width,height);
                    break;

                case SDLK_u:
                    debug=!debug;
                    break;

                case SDLK_p:
                    pause=!pause;
                    break;

                case SDLK_v:
                {
                    std::cout<<"please enter the code of missile you want 2 produce:";
                    std::string input;
                    std::cin>>input;
                    delete [] missile_code;

                    missile_code=new char[input.length()];

                    for(unsigned int u=0;u!=input.length();++u)
                        missile_code[u]=input[u];

                    break;
                }
                default:
                    break;
            }
        }
    }
    if(pause)
    {
        std::cout<<"pause\r";
        goto uncon_key_event;
    }
}
inline void game::con_key_eve()
{
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    if(state[SDL_SCANCODE_F])
    {
        V+=2;
        SDL_Delay(100);
        cout<<"fast!!!\n";
    }
    if(state[SDL_SCANCODE_G])
    {
        V-=4;
        SDL_Delay(100);
        cout<<((V<=1)? "I can't Baby...\n":"slow...\n");
        if(V<=1) V=0;
    }
    /*
        0->right
        1->left
        2->down
        3->up
    */
    bool dir[4]={(bool)state[SDL_SCANCODE_D],(bool)state[SDL_SCANCODE_A],(bool)state[SDL_SCANCODE_S],(bool)state[SDL_SCANCODE_W]};
    players->update_status(V,dir);
    if(debug)
    {
        int pre=0;
        for(int u=0;u!=9;u++)
            if(state[SDL_SCANCODE_A+u])
                pre++;
        cout<<pre<<'\n';
    }
}

inline void game::mouse_eve()
{
    int m_x,m_y;
    SDL_GetMouseState(&m_x,&m_y);

    if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        pos p = players->get_pos();
        vec m;
        m.x=m_x-p.x;   m.y=m_y-p.y;

        object o(p,m,missile_code);
        mlm_eve.push_back(o);
        if(IF_DELAY)
            SDL_Delay(100);
    }
}
