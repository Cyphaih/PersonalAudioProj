#include "MShowPitch.h"

#include "Visualizer.h" 

float getXPcnt(float noteToDraw, float middleNote, int notesOnScreen);


void MShowPitch::MVisualise(const SDL_Rect& rect)
{
	ShowNote(rect);
}


void MShowPitch::ShowNote(const SDL_Rect& rect)
{

    if (fundHistory->empty())
        return;
    
    float averageFF = processor->GetFundFreq();

    if (averageFF == 0) return;
    
    float noteFromFreq = 12 * log2(averageFF / 27.5);
  
    Note exp = ToNote(noteFromFreq);

    
    float distance = noteFromFreq - noteView;
    if (abs(distance) > (notesOnScreen  - 1) / 2) {
       if (distance > 0)     noteView  = noteFromFreq - (notesOnScreen - 1) / 2;
       else if(distance < 0) noteView  = noteFromFreq + (notesOnScreen - 1) / 2;
       distance = noteFromFreq - noteView;
      
        
    }
    if (abs(distance) > noteViewMoveLowerLimit) {
        if (distance > 0)
        {
            noteView += noteViewSpeed * vis->GetDSeconds() *  (1 + 4 * (abs(distance) - noteViewMoveLowerLimit) / (notesOnScreen - 1 / 2));
        }                                                          
        else if (distance < 0)                                     
        {                                                          
            noteView -= noteViewSpeed * vis->GetDSeconds() *  (1 + 4 * (abs(distance) - noteViewMoveLowerLimit)/ (notesOnScreen - 1 / 2));
        }

    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    SDL_Color textColor = { 255, 255, 0, 255 };

    for (int i = 0; i < notesOnScreen + 1;i++) {
        float recNote = std::round(noteView) + (i - notesOnScreen / 2);

        float noteX = getXPcnt(recNote,noteView,notesOnScreen) * rect.w + rect.x;
        float x = noteX - rect.w / (notesOnScreen * 2);

        
        SDL_RenderLine(renderer, x , rect.y, x , rect.y +rect.h);
    

        Note rNote = ToNote(recNote);

        SDL_Surface* textSurface = TTF_RenderText_Solid(vis->GetFont(), rNote.noteName.c_str(), rNote.noteName.length(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

        std::ostringstream oss;
        oss << rNote.octave;
        std::string octave = oss.str();


        SDL_Surface* octaveSurface = TTF_RenderText_Solid(vis->GetFont(), octave.c_str(), octave.length(), textColor);
        SDL_Texture* octaveTexture = SDL_CreateTextureFromSurface(renderer, octaveSurface);

        SDL_FRect textRect;
        SDL_FRect octaveRect;
        textRect.x = noteX - textSurface->w / 2;
        textRect.y = textSurface->h / 2 + rect.y;
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        octaveRect.x = noteX - octaveSurface->w / 2;
        octaveRect.y = textRect.y + octaveSurface->h;
        octaveRect.w = octaveSurface->w;
        octaveRect.h = octaveSurface->h;

       

        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
        SDL_RenderTexture(renderer, octaveTexture, NULL, &octaveRect);

        SDL_DestroyTexture(textTexture);
        SDL_DestroySurface(textSurface);

        SDL_DestroyTexture(octaveTexture);
        SDL_DestroySurface(octaveSurface);
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    SDL_RenderLine(renderer, getXPcnt(noteFromFreq,noteView, notesOnScreen)*rect.w + rect.x , rect.y , getXPcnt(noteFromFreq, noteView, notesOnScreen) * rect.w + rect.x, rect.y + rect.h);
    

}

MShowPitch::MShowPitch() 
{  
    name = ModeName::ShowPitch;
	fundHistory = &processor->GetFundHistory(); 
    noteView = 0; // I dont want to call a function from another object during 
}
void MShowPitch::MStart()
{
    noteView = 12 * log2(processor->GetFundFreq() / 27.5);
}

float getXPcnt(float noteToDraw, float middleNote, int notesOnScreen)
{
    return ((noteToDraw - middleNote) - (1 / 2)) / notesOnScreen + 0.5;
}
