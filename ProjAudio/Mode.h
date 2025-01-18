#pragma once

#include "AudioProcessor.h"
#include <SDL3/SDL.h>
#include <SDL_ttf.h>
#include <cstring> 
#include "Structs.h"

//Forward declaration
class Visualizer;
struct SDL_Renderer;

class Mode
{
public:
	Mode();
	enum ModeName {
		noMode,
		ShowAudioSpectrum,
		ShowPitch,
		Exercise
	};

	ModeName GetModeName();

	enum State { Start, Update, End, Done };
	State GetState();

	void MProcess(const SDL_Rect& rect);


	//This might be fine as a const but I dont know how that interacts with the std::functional stuff
	std::vector<Button>& GetBtns();

protected:
	


	State state;

	std::vector<Button> btns;

	AudioProcessor* processor;
	Visualizer* vis;
	SDL_Renderer* renderer;

	ModeName name;

	virtual void MUpdate(const SDL_Rect& rect);
	
	virtual void MStart();

	virtual void MEnd();

	virtual void MVisualise(const SDL_Rect& rect);

	

	Note ToNote(float n);

	

	
};


