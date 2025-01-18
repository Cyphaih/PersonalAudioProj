#pragma once
#include "Mode.h"


class MExercise :	public Mode
{
public:
	MExercise();
protected:
	//settings for exercise
	int minRange = 0;
	int maxRange = 72;
	int rangeLow = 10;
	int rangeHigh = 39;

	//Doing the exercise
	int currentRandomNote = round((rangeHigh - rangeLow) / 2);
	float onNoteTime = 0;
	float timeToClear = 1;

	//Mode stuff
	void MVisualise(const SDL_Rect& rect) override;
	void DrawExercise(const SDL_Rect& rect);
	void InitButtons();
	void UpdateBtnInfo(const SDL_Rect& rect);
	

	//Functions for exercise itself

	void AdjustLRange(int amount);
	void AdjustHRange(int amount);
	void ProgressExercise();
	void NewNote();


};

