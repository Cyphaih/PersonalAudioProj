#include "MShowAudioSpectrum.h"




void MShowAudioSpectrum::MVisualise(const SDL_Rect& rect)
{
    DrawAllSpec(rect);
}

MShowAudioSpectrum::MShowAudioSpectrum()
{
    name = ModeName::ShowAudioSpectrum;
    data = &processor->GetFreqData();
    minFreq = processor->minFreq;
    highestFreq = (processor->GetFInfos()[processor->GetFInfos().size() - 1].centerFrequency) * 3 / 2; //this gets the centerFreq of the highest filter and multiplies by 1.5, 
                                                                                                //which is the highest value I want to show
}

void MShowAudioSpectrum::DrawAudioSpectrum(const std::vector<FreqElement>& filterData, const SDL_Rect& clipRect)
{
    float maxX = log2(highestFreq) - log2(minFreq);
    float minX = log2(minFreq);


    for (FreqElement el : filterData) {

        if (el.frequency < minFreq)continue;
        float pX = (log2(el.frequency) - minX) / maxX;

        SDL_FRect  rect;
        float height = el.magnitude * 20; //first this because I use this to calculate y
        float x = pX * clipRect.w + clipRect.x;
        float y = clipRect.y + clipRect.h - height;
        float width = 4;
        


        rect = { x,y,width,height };
        SDL_SetRenderDrawColor(renderer, 255 * pX, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);

    }
}

void MShowAudioSpectrum::MStart()
{
    Mode::Start();
    

}

void MShowAudioSpectrum::DrawAllSpec(const SDL_Rect& rect) {

    if (data->empty())  return;
    for (int i = 0; i < data->size(); i++) {
        DrawAudioSpectrum(data->at(i), rect);
    }
}
