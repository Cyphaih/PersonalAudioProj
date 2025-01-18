#include "MExercise.h"
#include "Visualizer.h"
#include <random>
void MExercise::InitButtons()
{
	Button lowNote;
	lowNote.buttonRect = { 0,0,0,0 };
	lowNote.OnClick = [this](bool isLeftButton) {
		if (isLeftButton) {
			AdjustLRange(-1);
		}
		else {
			AdjustLRange(+1);
		}
	};
	btns.push_back(lowNote);

	Button lowOctave;
	lowOctave.buttonRect = { 0,0,0,0 };
	lowOctave.OnClick = [this](bool isLeftButton) {
		if (isLeftButton) {
			AdjustLRange(-12);
		}
		else {
			AdjustLRange(+12);
		}
		};
	btns.push_back(lowOctave);

	Button highNote;
	highNote.buttonRect = { 0,0,0,0 };
	highNote.OnClick = [this](bool isLeftButton) {
		if (isLeftButton) {
			AdjustHRange(-1);
		}
		else {
			AdjustHRange(+1);
		}
		};
	btns.push_back(highNote);

	Button highOctave;
	highOctave.buttonRect = { 0,0,0,0 };
	highOctave.OnClick = [this](bool isLeftButton) {
		if (isLeftButton) {
			AdjustHRange(-12);
		}
		else {
			AdjustHRange(+12);
		}
		};
	btns.push_back(highOctave);

}

void MExercise::UpdateBtnInfo(const SDL_Rect& rect)
{
	if (btns.size() < 3) return;
	// We take the rigth halve of the module  and put the buttons into one corner (not really corner but in that direction)
	// btn 0 is top left, btn 1 is bot left, btn 2 i top right, btn 3 is bot right (all meant on the right halve of this rect)

	Note lNote = ToNote(rangeLow);
	Note hNote = ToNote(rangeHigh);

	btns[0].buttonRect.h = rect.h * 0.4;
	btns[0].buttonRect.w = btns[0].buttonRect.h;
	btns[0].buttonRect.x = rect.x + rect.w * 0.65 - btns[0].buttonRect.w/2;
	btns[0].buttonRect.y = rect.y + (1.0f / 4.0f) * float(rect.h);
	btns[0].text = lNote.noteName;

	btns[2].buttonRect.h = rect.h * 0.4;
	btns[2].buttonRect.w = btns[2].buttonRect.h;
	btns[2].buttonRect.x = rect.x + rect.w * 0.85 - btns[2].buttonRect.w / 2;
	btns[2].buttonRect.y = rect.y + (1.0f / 4.0f) * float(rect.h);
	btns[2].text = hNote.noteName;

	std::ostringstream lOss;

	lOss << lNote.octave;
	std::string lString = lOss.str();

	

	btns[1].buttonRect.h = rect.h * 0.4;
	btns[1].buttonRect.w = btns[1].buttonRect.h;
	btns[1].buttonRect.x = rect.x + rect.w * 0.65 - btns[1].buttonRect.w / 2;
	btns[1].buttonRect.y = rect.y + (3.0f / 4.0f) * float(rect.h);
	btns[1].text = lString;


	std::ostringstream hOss;

	hOss << hNote.octave;
	std::string hString = hOss.str();

	btns[3].buttonRect.h = rect.h * 0.4;
	btns[3].buttonRect.w = btns[3].buttonRect.h;
	btns[3].buttonRect.x = rect.x + rect.w * 0.85 - btns[3].buttonRect.w / 2;
	btns[3].buttonRect.y = rect.y + (3.0f / 4.0f) * float(rect.h);
	btns[3].text = hString;




}

MExercise::MExercise() 
{
	NewNote();
	name = ModeName::Exercise;
	InitButtons();
}

void MExercise::MVisualise(const SDL_Rect& rect)
{
	ProgressExercise();
	DrawExercise(rect);
}

void MExercise::DrawExercise(const SDL_Rect& rect)
{
	//This displays the current random note to the screen;
	//It is convoluted but because I want this specific look I cant do it another way


	SDL_Color textColor = { 255,255,0,255 };

	Note rNote = ToNote(currentRandomNote);

	SDL_Surface* textSurface = TTF_RenderText_Solid(vis->GetBigFont(), rNote.noteName.c_str(), rNote.noteName.length(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);


	//this writes the octave number into a string, possible other ways but this works fine
	std::ostringstream oss;
	oss << rNote.octave;
	std::string octave = oss.str();


	SDL_Surface* octaveSurface = TTF_RenderText_Solid(vis->GetBigFont(), octave.c_str(), octave.length(), textColor);
	SDL_Texture* octaveTexture = SDL_CreateTextureFromSurface(renderer, octaveSurface);

	std::string con = " - ";
	SDL_Surface* conSurface = TTF_RenderText_Solid(vis->GetBigFont(), con.c_str(), con.length(), textColor);
	SDL_Texture* conTexture = SDL_CreateTextureFromSurface(renderer, conSurface);

	SDL_FRect textRect;
	SDL_FRect octaveRect;
	SDL_FRect conRect;

	textRect.x = rect.w / 4 - textSurface->w + rect.x;
	textRect.y = (rect.h - textSurface->h) / 2 + rect.y;
	textRect.w = textSurface->w;
	textRect.h = textSurface->h;

	octaveRect.x = rect.w / 4 + octaveSurface->w + rect.x;
	octaveRect.y = textRect.y ;
	octaveRect.w = octaveSurface->w;
	octaveRect.h = octaveSurface->h;

	conRect.x = rect.w *3/4 - conSurface->w/2  + rect.x;
	conRect.y = textRect.y;
	conRect.w = conSurface->w;
	conRect.h = conSurface->h;


	SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
	SDL_RenderTexture(renderer, octaveTexture, NULL, &octaveRect);
	SDL_RenderTexture(renderer, conTexture, NULL, &conRect);

	SDL_DestroyTexture(textTexture);
	SDL_DestroySurface(textSurface);
	

	SDL_DestroyTexture(octaveTexture);
	SDL_DestroySurface(octaveSurface);

	SDL_DestroyTexture(conTexture);
	SDL_DestroySurface(conSurface);

	UpdateBtnInfo(rect);

}

void MExercise::AdjustLRange(int amount)
{
	rangeLow += amount;
	if (rangeLow < minRange) 
	{
		rangeLow = minRange;
	}
	else if (rangeLow - 1 > maxRange) 
	{
		rangeLow = maxRange - 1;
	}
	if (rangeLow > rangeHigh)rangeHigh = rangeLow +1;
	NewNote();
}

void MExercise::AdjustHRange(int amount)
{
	rangeHigh+= amount;
	if (rangeHigh < minRange + 1) 
	{
		rangeHigh = minRange + 1;
	}
	else if (rangeHigh > maxRange) 
	{
		rangeHigh = maxRange;
	}
	if (rangeHigh < rangeLow)rangeLow = rangeHigh - 1;
	NewNote();
}

void MExercise::ProgressExercise()
{
	float Fq = processor->GetFundFreq();
	if (Fq == 0) return;
	int note = round(12 * log2(Fq / 27.5));

	if (note == currentRandomNote) {
		
		if (onNoteTime < timeToClear) {
			onNoteTime += vis->GetDSeconds();
		}
		else {
			onNoteTime = 0;
			NewNote();
		}
	}
	else {
		onNoteTime = 0;
	}
}

void MExercise::NewNote()
{
	// Create a random note in range
	// Will look into this more later, using it for now because it works fine

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(rangeLow, rangeHigh);
	currentRandomNote = distrib(gen);

}
