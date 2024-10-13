

#include "OpenGL.h"
#include "Device.hpp"
#include "Utils.hpp"
#include "OS.hpp"
#include "Input.hpp"


double GetTime()
{
    return  static_cast<double> (SDL_GetTicks())/1000;
}


void glDebugOutput(u32 source,
                   u32 type,
                   u32 id,
                   u32 severity,
                   s32 length,
                   const char *message,
                   const void *userParam)
{
    (void)length;
    (void)userParam;
    (void)severity;
    GLuint ignore_ids[1] = { 131185 }; // "will use video memory..."

    for (int i = 0 ; i <(int) ARRAY_SIZE_IN_ELEMENTS(ignore_ids) ; i++) 
    {
        if (id == ignore_ids[i]) {
            return;
        }
    }

  //  SDL_LogWarn(1,"!!! Debug callback !!!\n");
    SDL_LogDebug(2,"Debug message: id %d, %s", id, message);

 //   SDL_LogWarn(1,"Message source: ");
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             SDL_LogWarn(1,"API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   SDL_LogWarn(1,"Window System"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: SDL_LogWarn(1,"Shader Compiler"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     SDL_LogWarn(1,"Third Party"); break;
    case GL_DEBUG_SOURCE_APPLICATION:     SDL_LogWarn(1,"Application"); break;
    case GL_DEBUG_SOURCE_OTHER:           SDL_LogWarn(1,"Other"); break;
    }

  //  printf("Error type: ");
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               SDL_LogError(2,"Error"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: SDL_LogError(2,"Deprecated Behaviour"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  SDL_LogError(2,"Undefined Behaviour"); break;
    case GL_DEBUG_TYPE_PORTABILITY:         SDL_LogError(2,"Portability"); break;
    case GL_DEBUG_TYPE_PERFORMANCE:         SDL_LogError(2,"Performance"); break;
    case GL_DEBUG_TYPE_MARKER:              SDL_LogError(2,"Marker"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          SDL_LogError(2,"Push Group"); break;
    case GL_DEBUG_TYPE_POP_GROUP:           SDL_LogError(2,"Pop Group"); break;
    case GL_DEBUG_TYPE_OTHER:               SDL_LogError(2,"Other"); break;
    }

}


Device::Device(): m_width(0), m_height(0)
{
    m_shouldclose = false;
    m_window = NULL;
    m_context = NULL;
    m_current = 0;
    m_previous = 0;
    m_update = 0;
    m_draw = 0;
    m_frame = 0;
    m_target = 0;
    m_ready = false;
    
}

bool Device::Create(int width, int height,const char* title,bool vzync)
{
   
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
       LogError( "SDL could not initialize! SDL_Error: %s" , SDL_GetError());
       return false;
    }
    else
    {

            m_current = 0;
            m_previous = 0;
            m_update = 0;
            m_draw = 0;
            m_frame = 0;
            //SetTargetFPS(60);

            m_current = GetTime();
            m_draw = m_current - m_previous;
            m_previous = m_current;
            m_frame = m_update + m_draw;


        // bool debugContext = false;
        // bool forceGLCompatibilityProfile = false;
        // int majorVersion = 3;
        // int minorVersion = 3;
       
		// 	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, majorVersion );
		// 	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, minorVersion );

		// 	if ( debugContext ) SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
		// 	else if ( forceGLCompatibilityProfile ) SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
		// 	else SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

        m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (m_window == NULL)
        {
            LogError( "Window could not be created! SDL_Error: %s" , SDL_GetError());
            return false;
        }
            LogInfo( "[DEVICE] Created.");

            Logger::Instance();
            System::Instance();


            m_context = SDL_GL_CreateContext(m_window);
            if (m_context == NULL)
            {
                LogError( "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
                return false ;
            } else
            {
                 LogInfo( "[DEVICE] OpenGL context created!");
                           
                if (vzync)
                {
                    SDL_GL_SetSwapInterval(1);
                    SetTargetFPS(60);
                }
                else
                {
                    SDL_GL_SetSwapInterval(0) ;
                    SetTargetFPS(0);
                }

                   if (initOpenGLExtensions(false))
                    {

                  
                                glEnable(GL_DEBUG_OUTPUT);
                                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                            //    glDebugMessageCallback(glDebugOutput, nullptr);
                           //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
                   

                                LogInfo("[DEVICE] Vendor  :  %s",glGetString(GL_VENDOR));
                                LogInfo("[DEVICE] Renderer:  %s",glGetString(GL_RENDERER));
                                LogInfo("[DEVICE] Version :  %s",glGetString(GL_VERSION));

                                LogInfo("[DEVICE] GLSL Version: %s",glGetString(GL_SHADING_LANGUAGE_VERSION));

                                

                        


                    } else 
                    {
                        LogError( "Failed to load OpenGL extensions");
                        return false;
                    }
                                
                }         
              m_ready = true;
   

    }

   



    return true;

}

Device::~Device()
{
    Close();
}

void Device::Wait(float ms)
{
    Uint32 time = static_cast<Uint32>(ms*1000.0f);   
    SDL_Delay(time);
}




int Device::GetFPS(void)
{
    #define FPS_CAPTURE_FRAMES_COUNT    30      // 30 captures
    #define FPS_AVERAGE_TIME_SECONDS   0.5f     // 500 millisecondes
    #define FPS_STEP (FPS_AVERAGE_TIME_SECONDS/FPS_CAPTURE_FRAMES_COUNT)

    static int index = 0;
    static float history[FPS_CAPTURE_FRAMES_COUNT] = { 0 };
    static float average = 0, last = 0;
    float fpsFrame = GetFrameTime();

    if (fpsFrame == 0) return 0;

    if ((GetTime() - last) > FPS_STEP)
    {
        last = (float)GetTime();
        index = (index + 1)%FPS_CAPTURE_FRAMES_COUNT;
        average -= history[index];
        history[index] = fpsFrame/FPS_CAPTURE_FRAMES_COUNT;
        average += history[index];
    }

    return (int)roundf(1.0f/average);
}

void Device::SetTargetFPS(int fps)
{
    if (fps < 1) m_target = 0.0;
    else m_target = 1.0/(double)fps;

    
}

float Device::GetFrameTime(void)
{
    return (float)m_frame;
}

double Device::GetTime(void)
{
    unsigned int ms = SDL_GetTicks();    // Elapsed time in milliseconds since SDL_Init()
    double time = (double)ms/1000;
    return time;
}

u32 Device::GetTicks(void)
{
    return SDL_GetTicks();
}


bool Device::Run()
{       
   if (!m_ready) return false;
    Mouse::Update();
    Keyboard::Update();
    m_current = GetTime();            // Number of elapsed seconds since InitTimer()
    m_update = m_current - m_previous;
    m_previous = m_current;

    return !m_shouldclose;
}

void Device::Update()
{
    SDL_Event event;
 
    while (SDL_PollEvent(&event))
    {
        
        switch (event.type)
        {
            case SDL_QUIT:
            {
                m_shouldclose = true;
                return;
            }
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    {
                        m_width = event.window.data1;
                        m_height = event.window.data2;
                       // Driver::Instance().resize(m_width, m_height);
                        break;
                    }
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym==SDLK_ESCAPE)
                {
                    m_shouldclose = true;
                    break;
                }
               
               Keyboard::setKeyState(event.key.keysym.scancode, true);
        
                break;
            }
            
            case SDL_KEYUP:
            {
               
                Keyboard::setKeyState(event.key.keysym.scancode, false);
                break;
            
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
               
                int btn = event.button.button - 1;
                if (btn == 2) btn = 1;
                else if (btn == 1) btn = 2;
                Mouse::setMouseButton(btn, true);
            }break;
            case SDL_MOUSEBUTTONUP:
            {
                int btn = event.button.button - 1;
                if (btn == 2) btn = 1;
                else if (btn == 1) btn = 2;
                Mouse::setMouseButton(btn, false);
                
                break;
            }
            case SDL_MOUSEMOTION:
            {
               Mouse::setMousePosition(event.motion.x, event.motion.y,event.motion.xrel, event.motion.yrel);
                break;   
            }
            
            case SDL_MOUSEWHEEL:
            {
                Mouse::setMouseWheel(event.wheel.x, event.wheel.y);
                break;
            }
            default:
            {
                break;
            }
        }

    }
    
}

void Device::Close()
{ 
    if (!m_ready) return;

    m_ready = false;

    Logger::Instance().Drop();
    System::Instance().Drop();
   // Driver::Instance().Drop();

    SDL_GL_DeleteContext(m_context);
    SDL_DestroyWindow(m_window);

    m_window = NULL;
    LogInfo( "[DEVICE]  Destroyed" );
    SDL_Quit();
}



void Device::Flip()
{
   

    SDL_GL_SwapWindow(m_window);

    m_current = GetTime();
    m_draw = m_current - m_previous;
    m_previous = m_current;
    m_frame = m_update + m_draw;



    // Wait for some milliseconds...
    if (m_frame < m_target)
    {
        Wait((float)(m_target -m_frame));

        m_current = GetTime();
        double waitTime = m_current - m_previous;
        m_previous = m_current;

        m_frame += waitTime;      // Total frame time: update + draw + wait
    }

  //  String fps(GetFPS());
    //String title = String("FPS: ") + fps;
  //  SDL_SetWindowTitle(m_window,  fps.c_str());

}

