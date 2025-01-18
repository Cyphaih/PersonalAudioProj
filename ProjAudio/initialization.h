#pragma once
#ifndef INITIALIZATION_H
#define INITIALIZATION_H



#ifndef M_PI
#define M_PI 3.14159265358979323846 
#endif

#include <fftw3.h>
#include <cmath> 
#include <iostream>
#include <portaudio.h>
#include <SDL3/SDL.h>
#include <SDL_ttf.h>
#include <cstring> 
#include "AudioProcessor.h"
#include <vector>
#include "Visualizer.h"


bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font);



#endif