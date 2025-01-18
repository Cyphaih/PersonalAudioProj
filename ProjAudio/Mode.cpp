#include "Mode.h"
#include "Visualizer.h"

Mode::Mode()
{
    name = ModeName::noMode;
    state = State::Start;
    processor = &AudioProcessor::GetInstance();
    vis = &Visualizer::GetInstance();
    renderer = vis->GetRenderer();
}

Mode::ModeName Mode::GetModeName()
{
    return name;
}

Mode::State Mode::GetState()
{
    return state;
}

void Mode::MProcess(const SDL_Rect& rect)
{
	if (state == State::Start) {
		MStart();
		state = State::Update;
	}

	if (state == State::Update) {
		MUpdate(rect);
	}
	if (state == State::End) {
		MEnd();
        state = State::Done;
	}

	

}

std::vector<Button>& Mode::GetBtns()
{
    return btns;
}

void Mode::MUpdate(const SDL_Rect& rect)
{
    MVisualise(rect);
}

void Mode::MStart()
{
	
}

void Mode::MEnd()
{
}

void Mode::MVisualise(const SDL_Rect& rect)
{
}

Note Mode::ToNote(float n) {
    //Gets the note with octave from the note steps from A1 (27.5Hz)

    int rounded = int(std::round(n));

    //get octave von n notes from base (27.5), +2 because I move down the scale  with -3
    float octave = ((rounded - 3.0) / 12.0) + 2.0;

    int noteIndex = rounded % 12;


    //The following wont be neccessary usually as the saved notes are all 27.5 or abouve and this wont be negative, but just in case
    if (noteIndex < 0) {
        noteIndex += 12;
    }

    return { noteNames[noteIndex], int(floor(octave)) };
}