#pragma once
#include "Mode.h"

class MShowAudioSpectrum : public Mode
{
public:
	MShowAudioSpectrum();
protected:
	void DrawAudioSpectrum(const std::vector<FreqElement>& filterData, const SDL_Rect& rect);
	
	void MStart() override;

	void DrawAllSpec(const SDL_Rect& rect);
	void MVisualise(const SDL_Rect& rect) override;
	const std::vector<std::vector<FreqElement>>* data;
	float highestFreq;
	float minFreq;
};

