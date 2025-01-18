#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL_ttf.h>
#include <functional>
struct FilterInfo {
    float samplerate;
    float centerFrequency;
    float bandwidth;

    int decimation = 1; // this will definitely be changed, but 1 in case of division when unchanged
    int window = 128; //Initialise it as 128 because it works well and is most likely the number that will be calculated to be put here
};

struct FreqElement {

    float frequency;
    float magnitude;

};

struct Note {
    std::string noteName;
    int octave;
};

struct Button {
    bool isPressedLeft = false;
    bool isPressedRight = false;
    bool isHovered = false;
    SDL_FRect buttonRect = {};
    SDL_Color txtColor = { 0,0,0,255 };        //default text color of buttons
    SDL_Color btnColor = { 200,200,200,255 }; //default btn color
    std::string text;
    std::function<void(bool isLeftButton)> OnClick;
};

//these will always be the same and might wand to be accessed multiple times
const static std::string noteNames[12] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
