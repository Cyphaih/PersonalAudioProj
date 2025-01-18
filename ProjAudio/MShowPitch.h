#pragma once
#include "Mode.h"

//CHECK FOR MEMORY LEAK


class MShowPitch :	public Mode
{
public:
	MShowPitch();
	
protected:

	
	void MStart() override;
	float noteView;
	const std::vector<float>* fundHistory;
	void MVisualise(const SDL_Rect& rect) override;
	int notesOnScreen = 12; // the note amount displayed on window for FundamentalToNote, this is not based on time so it will vary depending on fps but I am fine with that for now
	void ShowNote(const SDL_Rect& rect);

	float noteViewSpeed = 2; // note per second as base speed after noteView is at least  noteViewMoveLowerLimit amount away from current note, going up to 4 times this amount 
	float noteViewMoveLowerLimit = 1;
};

