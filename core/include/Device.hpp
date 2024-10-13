#pragma once



#include "Config.hpp"


class    Device 
{
public:
    Device();
    virtual ~Device();

    bool Create(int width, int height,const char* title,bool vzync=false);
 
    bool Run();

    void Update();

    void Close();

    void Flip();

   
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }


    void Wait(float ms);
    void SetTargetFPS(int fps);
    int  GetFPS(void);
    float GetFrameTime(void);
    double GetTime(void);
    u32  GetTicks(void) ;


    SDL_Window*   GetWindow() const { return m_window; }

   
private:
    int m_width;
    int m_height;    
    bool m_shouldclose ;
    SDL_Window   *m_window;
    SDL_GLContext m_context;
    double m_current;                 
    double m_previous;                  
    double m_update;                    
    double m_draw;                       
    double m_frame;   
    double m_target;
    bool m_ready;


};
