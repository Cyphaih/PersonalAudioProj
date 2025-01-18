#pragma once
#include <vector>
#include <string>
#include "Structs.h"
#include <SDL3/SDL.h>
#include <SDL_ttf.h>
#include <cstring> 
#include "AudioProcessor.h"
#include "AllModes.h"
#include "Structs.h"
#include <functional>

class Visualizer
{
    

public:
    

    
    int windowBaseHeight = 50;

    int windowHeight = 0;
    int windowWidth = 0;

    int mHeight = 50;
    int mWidth = 300;

    float mMargin = 20;
    

	static Visualizer& GetInstance();

    void Process();
    void ClearSDL();

    SDL_Renderer* GetRenderer();
    TTF_Font* GetFont();
    TTF_Font* GetBigFont(); 

    std::vector<Mode*> modes;

    float GetDSeconds(); 

    bool GetRunning();
    void DrawButton(const Button& button);
   

private:

    void CheckAllButtonsHover(SDL_FPoint mouse);
    void CheckAllButtonsDownLeft();
    void CheckAllButtonsDownRight();
    void CheckAllButtonsUpLeft();
    void CheckAllButtonsUpRight();

    void ToggleButtonColor(Button& btn, bool toBase);
    
    bool running = true;
    std::vector<Button> buttons;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* bigFont;
    AudioProcessor* processor;

    float fontSize = 16;
    float dSeconds = 0;

    //Could probably do this different but its fine for this project
    
    void CreateMode(Mode::ModeName ModeToAdd);
    void DeleteMode(Mode::ModeName ModeToDelete);

    void SetWindowSize();

    std::chrono::time_point<std::chrono::high_resolution_clock> last;
    std::chrono::time_point<std::chrono::high_resolution_clock> now;

    Visualizer(); 
    ~Visualizer();
    bool InitializeSDL();
    Visualizer(const Visualizer&) = delete;
    Visualizer& operator=(const Visualizer&) = delete;
    void SetDeltaSeconds();

    bool moveWindow = false;
    float offsetMouseX, offsetMouseY;
   
    void InitButtons();
   

    void SDLEvents();

};

