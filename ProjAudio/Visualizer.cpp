#include "Visualizer.h"
#include <sstream>

Visualizer& Visualizer::GetInstance()
{
    static Visualizer instance;
    return instance;
}



void Visualizer::Process()
{
    SetDeltaSeconds();
    //if (modes.empty()) {
    //
    //
    //    CreateMode(Mode::ModeName::ShowPitch);
    //    
    //    CreateMode(Mode::ModeName::ShowAudioSpectrum);
    //    
    //    
    //}


     SDLEvents();




    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);


    

    int x = 0 + mMargin, y = windowBaseHeight+ 2* mMargin; //origin of the clip rectangle
    SDL_Rect clipRect = { x,y,mWidth,mHeight };
    for (Mode* mode : modes) {
        clipRect = { x,y,mWidth,mHeight };
        SDL_SetRenderClipRect(renderer, &clipRect);
        mode->MProcess(clipRect);
        SDL_SetRenderClipRect(renderer, NULL); // maybe I dont need this 
        y += mHeight + mMargin;
    }
    
    for (int i = 0; i < buttons.size(); i++) {
        DrawButton(buttons[i]);
    }

    for (Mode* mode : modes) {
        for (Button& btn : mode->GetBtns()) {
            DrawButton(btn);
        }
    }
    
   

    SDL_RenderPresent(renderer);
    
   

}

void Visualizer::ClearSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

SDL_Renderer* Visualizer::GetRenderer()
{
    return renderer;
}

TTF_Font* Visualizer::GetFont()
{
    return font;
}

TTF_Font* Visualizer::GetBigFont()
{
    return bigFont;
}

float Visualizer::GetDSeconds()
{
    return dSeconds;
}

bool Visualizer::GetRunning()
{
    return running;
}



//I would have liked to just send a pointer to the Class template but for now this is enough

//This creates the class after checking if it has no other mode of the same type
void Visualizer::CreateMode(Mode::ModeName ModeToAdd)
{
    //checks if there is a mode like this already and returns if true
    for (int i = modes.size() - 1; i >= 0; i--) {
        if (modes[i]->GetModeName() == ModeToAdd) {
            return;
        }
    }
    

    Mode* newMode = nullptr;

    switch (ModeToAdd) {
    case Mode::ModeName::ShowAudioSpectrum:
        newMode = new MShowAudioSpectrum(); 
        break;
    case Mode::ModeName::ShowPitch:
        newMode = new MShowPitch();  
        break;
    case Mode::ModeName::Exercise:
        newMode = new MExercise;
        break;
    default:
        break;

    }
    if (newMode != nullptr)
    {
        modes.push_back(newMode);
        
    }
    
    SetWindowSize();
}

void Visualizer::DeleteMode(Mode::ModeName ModeToDelete)
{
    for (int i = modes.size() - 1; i >= 0; i--) {
        if (modes[i]->GetModeName() == ModeToDelete) {
            delete modes[i];
            modes.erase(modes.begin() + i);
            break;
        }
    }
    SetWindowSize();
}

void Visualizer::SetWindowSize()
{
    windowWidth = 2 * mMargin + mWidth;
    windowHeight = windowBaseHeight + modes.size() * (mHeight + mMargin) + 2*mMargin;
    SDL_SetWindowSize(window, windowWidth, windowHeight);

}

Visualizer::Visualizer()
{
    running = true;
    processor = &AudioProcessor::GetInstance();
    InitializeSDL();
    InitButtons();
    SetWindowSize();
}

Visualizer::~Visualizer()
{
    ClearSDL();
    
}

bool Visualizer::InitializeSDL() {



    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initializing SDL_ttf
    if (!TTF_Init()) {
        std::cerr << "Failed to initialize SDL_ttf: " << std::endl;
        return false;
    }

    // Creating a window
    window = SDL_CreateWindow("Audio Spectrum Visualizer", windowWidth, windowHeight, SDL_WINDOW_BORDERLESS|SDL_WINDOW_ALWAYS_ON_TOP);
    if (window == nullptr) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Creating a renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Loading a font
    font = TTF_OpenFont("friendly_sans/FriendlySans.ttf", fontSize); 
    if (font == nullptr) {
        std::cerr << "Font loading failed: " << std::endl;
        return false;
    }
    // Loading another font because I want big characters in some cases
    bigFont = TTF_OpenFont("friendly_sans/FriendlySans.ttf", fontSize * 2); 
    if (bigFont == nullptr) {
        std::cerr << "Font loading failed: " << std::endl;
        return false;
    }

    return true;
}

void Visualizer::SetDeltaSeconds()
{
    
    now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last);
    dSeconds = duration.count() / 1000000.0f; // Convert to seconds

    last = now; //update for next call

}

void Visualizer::InitButtons()
{
    
    SDL_FRect bR = { mMargin,mMargin,mWidth,windowBaseHeight };

    //I decided to put numbers here because I will configure the settings bar here


    Button bShowAudioSpectrum;

    
    bShowAudioSpectrum.buttonRect = {bR.x + mMargin/2,bR.y,0.2f*bR.w - mMargin/2,bR.h};
    bShowAudioSpectrum.txtColor = { 0,0,0,255 };
    bShowAudioSpectrum.text = "S";
    bShowAudioSpectrum.OnClick = [this](bool isLeftButton) {
        if (isLeftButton) {
            CreateMode(Mode::ModeName::ShowAudioSpectrum);
            
        }
        else {
            DeleteMode(Mode::ModeName::ShowAudioSpectrum);
            
        }
    };
    buttons.push_back(bShowAudioSpectrum);

    Button bShowPitch;

    bShowPitch.buttonRect = { bR.x + 0.2f* bR.w  + mMargin / 2,  bR.y,  0.2f * bR.w - mMargin / 2,     bR.h };
    bShowPitch.txtColor = { 0,0,0,255 };
    bShowPitch.text = "P";
    bShowPitch.OnClick = [this](bool isLeftButton) {
        if (isLeftButton) {
            CreateMode(Mode::ModeName::ShowPitch);
           
        }
        else {
            DeleteMode(Mode::ModeName::ShowPitch);
            
        }
        };
    buttons.push_back(bShowPitch);

    Button bExercise;

    bExercise.buttonRect = { bR.x + 0.4f * bR.w + mMargin / 2,  bR.y,  0.2f * bR.w - mMargin / 2,     bR.h };
    bExercise.txtColor = { 0,0,0,255 };
    bExercise.text = "E";
    bExercise.OnClick = [this](bool isLeftButton) {
        if (isLeftButton) {
            CreateMode(Mode::ModeName::Exercise);
           
        }
        else {
            DeleteMode(Mode::ModeName::Exercise);
            
        }
        };
    buttons.push_back(bExercise);

    
    //Button for dragging here maybe


    Button bQuit;

    bQuit.buttonRect = { bR.x + 0.8f * bR.w + mMargin / 2,  bR.y,  0.2f * bR.w - mMargin / 2,     bR.h };
    bQuit.txtColor = { 255,0,0,255 };
    bQuit.text = "X";
    bQuit.OnClick = [this](bool isLeftButton) {
        if (isLeftButton) {
            running = false;
        }
        else {
            
        }
        };
    buttons.push_back(bQuit);

}

void Visualizer::DrawButton(const Button& button)
{
    if (button.text.empty())return;
    // Draw button background
    SDL_SetRenderDrawColor(renderer,button.btnColor.r,  button.btnColor.g,  button.btnColor.b,  button.btnColor.a);
    SDL_RenderFillRect(renderer, &button.buttonRect);

    // Render button text
    SDL_Color textColor = button.txtColor; // White text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, button.text.c_str(), button.text.length(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    // Center the text within the button
    SDL_FRect textRect;
    textRect.x = button.buttonRect.x + (button.buttonRect.w - textSurface->w) / 2;
    textRect.y = button.buttonRect.y + (button.buttonRect.h - textSurface->h) / 2;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_RenderTexture(renderer, textTexture, NULL, &textRect);

    // Clean up
    SDL_DestroySurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Visualizer::CheckAllButtonsHover(SDL_FPoint mouse)
{
    for (Button& button : buttons) {
        button.isHovered = SDL_PointInRectFloat(&mouse, &button.buttonRect);
        if (!button.isHovered) { 
            button.isPressedLeft = false; 
            button.isPressedRight = false; 
        }
    }

    for (Mode* mode : modes) {
        for (Button& btn : mode->GetBtns()) {
            btn.isHovered = SDL_PointInRectFloat(&mouse, &btn.buttonRect);
            if (!btn.isHovered) {
                btn.isPressedLeft = false;
                btn.isPressedRight = false;
            }
        }
    }
}

void Visualizer::CheckAllButtonsDownLeft()
{
    for (Button& button : buttons) {
        if (button.isHovered) {
            button.isPressedLeft = true;
        }
    }

    for (Mode* mode : modes) {
        for (Button& btn : mode->GetBtns()) {
            if (btn.isHovered) {
                btn.isPressedLeft = true;
            }
        }
    }
}

void Visualizer::CheckAllButtonsDownRight()
{
    for (Button& button : buttons) {
        if (button.isHovered) {
            button.isPressedRight = true;
        }
    }

    for (Mode* mode : modes) {
        for (Button& btn : mode->GetBtns()) {
            if (btn.isHovered) {
                btn.isPressedRight = true;
            }
        }
    }
}

void Visualizer::CheckAllButtonsUpLeft()
{
    for (Button& button : buttons) {
        if (button.isPressedLeft && button.isHovered) {
            button.isPressedLeft = false;
            button.OnClick(true);
        }
    }
    for (Mode* mode : modes) {
        std::vector<Button>& btns = mode->GetBtns();
        for (int i = 0; i < btns.size();i++) {
            if (btns[i].isPressedLeft && btns[i].isHovered) {
                btns[i].isPressedLeft = false;
                btns[i].OnClick(true);
            }
        }
    }
}

void Visualizer::CheckAllButtonsUpRight()
{
    for (Button& button : buttons) {
        if (button.isPressedRight && button.isHovered) {
            button.isPressedRight = false;
            button.OnClick(false);
        }
    }
    for (Mode* mode : modes) {
        for (Button& btn : mode->GetBtns()) {
            if (btn.isPressedRight && btn.isHovered) {
                btn.isPressedRight = false;
                btn.OnClick(false);
            }
        }
    }
}



void Visualizer::ToggleButtonColor(Button& btn, bool toBase)
{
    SDL_Color baseColor = { 200,200,200,255 };
    SDL_Color toggleColor = { 255,0,255,255 };

    if (toBase) {
        btn.btnColor = baseColor;
    }
    else {
        btn.btnColor = toggleColor;
    }

    for (Button btns : buttons) {
        std::cout << btns.btnColor.b << " ";
    }
}

void Visualizer::SDLEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_MOUSE_MOTION: {
            float mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_FPoint mouse = { mouseX, mouseY };
            CheckAllButtonsHover(mouse);

            //for tmoving the window
            if (moveWindow) {
                
                
                int windowX, windowY;
                SDL_GetWindowPosition(window, &windowX, &windowY);

                int dX = offsetMouseX - mouseX;
                int dY = offsetMouseY - mouseY;
                

                SDL_SetWindowPosition(window, windowX - dX, windowY -dY);
                
            }



            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {

                CheckAllButtonsDownLeft();

                //For moveing the window
                moveWindow = true;
                SDL_GetMouseState(&offsetMouseX, &offsetMouseY);

            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                CheckAllButtonsDownRight();
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                CheckAllButtonsUpLeft();
            }
            if (event.button.button == SDL_BUTTON_RIGHT) {
                CheckAllButtonsUpRight();
            }
            moveWindow = false;

            break;
        }
    }
}





