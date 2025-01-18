#pragma once

#include <fftw3.h>
#include "portaudio.h"
#include "vector"
#include "Iir.h"
#include "Structs.h"
#include "chrono"


#ifndef M_PI
#define M_PI 3.14159265358979323846 
#endif






class AudioProcessor
{
public:
    //static functions 
    //This returns the existing instance or calls the private constructor to create the new instance and then returns it
    static AudioProcessor& GetInstance(); 

    //AudioCallBack function for portAudio implementation in here because I use AudioProcessor to do most of the audio stuff
    static int AudioCallback(const void* inputBuffer, void* outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void* userData);


    //In case filter information needs to be updated because frequency scope changes or input device information is different
    void RefreshFilter(float sampleRate, int sampleSize, float newMinFreq, float newMacFreq);   //this still needs actuall PATermination and restart of AudioCallBack to work properly
                                                                                                //but not my concern currently



    //variables necessary to do all the audio functionality
    //numbers are defaults and might need to be refreshed
    float minFreq = 27.5;
    float maxFreq = 4186;
    float samplerate = 44100;
    int sampleSize = 1024;


    //Not used right now but might use later for another exercise
    std::vector<float> GenerateSineWave(float frequency, float amplitude, int sampleRate, int numSamples);


    void PitchDetection(int framesPerBuffer, float* in);

    //I want all the Audio stuff to be done here for clear overview and decided to move the init stuff here as well
    bool initializePortAudio();
    

    float GetFundFreq();
    const std::vector<float>& GetSampleHistory();
    const std::vector<FilterInfo>& GetFInfos();
    const std::vector<std::vector<FreqElement>>& GetFreqData();
    const std::vector<float>& GetFundHistory();


    //Following functions are just test for pitch detection that have not worked well yet, might look at it later again but for now it is not used
    //using Aubio pitchdetection instead
    float ACM(std::vector<float>& sig, int w, int t, int lag);
    float DetectPitch(std::vector<float>& sig, int w, int t, float sampleRate, int minBounds, int maxBounds, float thresh = 0.1);
    float DF(std::vector<float>& sig, int w, int t, int lag);
    float CMNDF(std::vector<float>& sig, int w, int t, int lag);
    
private:


    PaStream* stream;
    PaStreamParameters inputParameters, outputParameters;

    //Private Constructors
    AudioProcessor(); //Will initialise the filters and portAudio stuff at start 
    ~AudioProcessor();//Clears PA stuff
    AudioProcessor(const AudioProcessor&) = delete;
    AudioProcessor& operator=(const AudioProcessor&) = delete;

    //Filtering and fft 
    std::vector<Iir::Butterworth::BandPass<4>> filters;
    std::vector<FilterInfo> fInfos;
    void InitProcessor();
    void ClearProcessor();
    void WindowSamples(std::vector<float>& samples, int filternumber);
    void ApplyWindowFilter(std::vector<float>& samples);
    void Decimate(std::vector<float>&, int decimation_factor);
    void ApplyFilter(int filterID, std::vector<float>& samples);
    
    //sampleHistory to get accurate low frequencies (low frequencies will take longer to update)
    std::vector<float> sampleHistory;

    //contains the history of the last dominant frequencies measured 
    int maxDFHSize = 8; //max domFreqHistory size;
    std::vector<float> fundFreqHistory; 
    
    //This contains the freqElements we get from the fft implementation in audioCallBack
    //ít is split into the different filters used for better controll
    std::vector<std::vector<FreqElement>> freqData;

    FreqElement ParaInterpolForDF(int copyNumber, int binNumber); // do a parable - interpolation to determine dominant frequency

    //Add and remove from sampleHistory
    void AddToHistory(const float* toAdd, int amount);
    void ReduceSampleHistory(); //cutoff old elements 
    float maxMult; //How many samples multiplications I want to keep

    void AddFundFreq(float freqToAdd); //Add new values to domFreqHistory 
    
    bool output = true; // In case I want to output just one iteration of the audioCallBack loop
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime; // start
    
    
   
};


