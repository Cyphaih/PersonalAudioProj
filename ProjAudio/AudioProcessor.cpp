#include "AudioProcessor.h"

#include "aubio.h"
#include <iostream>
AudioProcessor& AudioProcessor::GetInstance() {
    static AudioProcessor instance;
    return instance;
}



int AudioProcessor::AudioCallback(const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData) {


    //need this to handle the static/non static differences of audioprocessor and audiocallback
    AudioProcessor& processor = AudioProcessor::GetInstance();

    // The following is just to test with a moving frequency for a Sin wave
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - processor.startTime);
    float playtimeSeconds = float(duration.count())/1000;
    float freqToSin = sin(playtimeSeconds) * 900 + 1000;

    
    float* in = (float*)inputBuffer;
    std::vector<float> inVec(in, in + framesPerBuffer);
    
    
    //The following sin signal I just created for testing purposes
    //std::vector<float> sinVec = processor.GenerateSineWave(1000, 10.0, 44100, 1024);
    //float* in = sinVec.data(); 


    processor.PitchDetection(framesPerBuffer,in);
    
 
    
    processor.AddToHistory(in, framesPerBuffer);

    
    std::vector<std::vector<float>> copies(processor.filters.size(), processor.sampleHistory);

   

    std::vector<FreqElement> dominantfrequencies; //These are the dominant frequencies per filtered copy
    
    for (int i = 0; i < processor.filters.size() && i < copies.size() ; i++) {

        //apply filter to every copy to reduce redundant frequencies
        //applying a filter on multiple copies of 44100+ sample arrays seems inefficient
        processor.ApplyFilter(i, copies[i]);
        
        //reducing the sample amount for better fft results especially in lower freq areas
        processor.Decimate(copies[i], processor.fInfos[i].decimation);
        
        //This applies a window and shortens the decimated array 
        processor.WindowSamples(copies[i], i); 

        
        
        //Doing the fft
        fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * copies[i].size());
       
        fftwf_plan plan = fftwf_plan_dft_r2c_1d(copies[i].size(), copies[i].data(), out, FFTW_ESTIMATE);
        
        fftwf_execute(plan);
        
        float sr = processor.fInfos[i].samplerate / processor.fInfos[i].decimation;

        processor.freqData[i].resize( float(copies[i].size()) / 2);
        
        int domFreqBinID = 0;
        FreqElement domFreqBin = { 0,0 }; 
        
        for (int j = 0; j < copies[i].size() / 2; ++j) {
            float magnitude = sqrt(out[j][0] * out[j][0] + (out[j][1] * out[j][1]));
            float frequency = (float)(j) * sr / copies[i].size();
           
            if (domFreqBin.magnitude < magnitude) {
                domFreqBin = { frequency,magnitude };
                domFreqBinID = j;
            }

            processor.freqData[i][j]={frequency,magnitude};
            // Print or use the magnitude and frequency
            
        }
        
       if (domFreqBin.magnitude > 1) {
           FreqElement el = processor.ParaInterpolForDF(i, domFreqBinID);
           
           dominantfrequencies.push_back(el);
       }
        
        //Freeing space to avoid memory leaks
        fftwf_destroy_plan(plan);
        fftwf_free(out);
    }
    
    for (std::vector<float> copy : copies) {
        copy.clear();
    }
    copies.clear();
    
    
  
    return paContinue;
}

void AudioProcessor::RefreshFilter(float newSampleate, int newSampleSize, float newMinFreq, float newMaxFreq)
{
    if (newSampleate != samplerate || newSampleSize != sampleSize
        || newMinFreq != minFreq || newMaxFreq != maxFreq) {
        samplerate = newSampleate;
        sampleSize = newSampleSize;
        minFreq = newMinFreq;
        maxFreq = newMaxFreq;


        InitProcessor();
    }

}

float AudioProcessor::GetFundFreq()
{
    if (fundFreqHistory.empty())return 0.0f;
    float sum = 0.0;
    for (int i = 0; i < fundFreqHistory.size() / 2;i++) {
        sum += fundFreqHistory[i];
    }
    sum = sum / fundFreqHistory.size();
    return sum;
}

const std::vector<float>& AudioProcessor::GetSampleHistory()
{
    return sampleHistory;
}

const std::vector<FilterInfo>& AudioProcessor::GetFInfos()
{
    return fInfos;
}

const std::vector<std::vector<FreqElement>>& AudioProcessor::GetFreqData()
{
    return freqData;
}

const std::vector<float>& AudioProcessor::GetFundHistory()
{
    return fundFreqHistory;
}

void AudioProcessor::ApplyFilter(int filterID, std::vector<float>& samples)
{
    Iir::Butterworth::BandPass<4> pass = filters[filterID];
    
    pass.reset();
    
    
    
    for (float& sample : samples) {
        sample = pass.filter(sample);
        
    }
    
    
}

FreqElement AudioProcessor::ParaInterpolForDF(int copyNumber, int binNumber)
{
    FreqElement peak = {};
    if (binNumber == 0 || binNumber == freqData[copyNumber].size() - 1) {
        return { 0,0 };
    }


   
    //Get the equally spaced by x axis FreqElements of y0 highest meassured freq bin and the adjacent bins
    FreqElement y0 = freqData[copyNumber][binNumber];
    FreqElement y1 = freqData[copyNumber][binNumber - 1];
    FreqElement y2 = freqData[copyNumber][binNumber + 1];


    //spacing between y2 and y1 after dividing by freqscale will be 2
    float freqScale = (y2.frequency - y1.frequency) / 2;


    //Need to divide the frequencies first by the delta per step to get to a spacing of 1 
    y0.frequency *= 1 / freqScale;
    y1.frequency *= 1 / freqScale;
    y2.frequency *= 1 / freqScale;

    
    //// xPeak = 1/2 * (y1 - y2) / (y1 - 2 * y0 + y2) + x    ||<- formula for calculating the x position (frequency) of the peak, x is y0.frequency, the frequency of the highest magnitude bin
    if ((y1.magnitude - 2 * y0.magnitude + y2.magnitude) != 0) {
        peak.frequency = (1.0 / 2.0) * (y1.magnitude - y2.magnitude) / (y1.magnitude - 2 * y0.magnitude + y2.magnitude) + y0.frequency;
        
    }
    else {
        
    }
   

    peak.frequency *= freqScale;
    y0.frequency *= freqScale;
    //peak.frequency += y0.frequency;
    y1.frequency *= freqScale;
    y2.frequency *= freqScale;

    
    ////y = y1 + (x - x1) * ((x2 - x1) * (y3 - y1) * (x - x2) - (x3 - x1) * (y2 - y1) * (x - x3)) / ((x2 - x1) * (x3 - x1) * (x3 - x2))
    //// The above is just putting variables into the Lagrange interpolation formula, I dont know another forumla fitting to this yet
    
    float a = (peak.frequency - y0.frequency);
    float b = (y1.frequency - y0.frequency) * (y2.magnitude - y0.magnitude) * (peak.frequency - y1.frequency);
    float c = (y2.frequency - y0.frequency) * (y1.magnitude - y0.magnitude) * (peak.frequency - y2.frequency);
    float d = ((y1.frequency - y0.frequency) * (y2.frequency - y0.frequency) * (y2.frequency - y1.frequency));

    peak.magnitude = y0.magnitude + a * (b - c) / d;

    

    //This was a pain but it works
    return peak;

}

void AudioProcessor::AddToHistory(const float* toAdd, int amount) {
    sampleHistory.insert(sampleHistory.end(), toAdd, toAdd + amount);
    ReduceSampleHistory();
}

void AudioProcessor::ReduceSampleHistory()
{
    
    if (sampleHistory.size() > maxMult * sampleSize) 
    {
        int toErase = sampleHistory.size() - maxMult * sampleSize;
        sampleHistory.erase(sampleHistory.begin(), sampleHistory.begin() + toErase);
       
    }
  
}

void AudioProcessor::AddFundFreq(float freqToAdd)
{

    if (freqToAdd < 27.5 || freqToAdd > 4698.63) {
        
        return;
    }
    fundFreqHistory.push_back(freqToAdd);
    if(fundFreqHistory.size() > maxDFHSize) 
    {
        fundFreqHistory.erase(fundFreqHistory.begin(), fundFreqHistory.end() - maxDFHSize);
    }

    
}

void AudioProcessor::InitProcessor()
{
    fInfos.clear();
    filters.clear();
    //Initialise filters for the audioCallback function to use
    int i = 0; // this is just to get the amount of filters and possibly the step size increase
    
    
    //This will iterate until it created the necessary amount of filters to cover the necessary range
    //Keep in mind that the Sample amount per AudioCallback defines what the lowest Frequency meassurable is with a fft 
    // (Samplerate / SampleSize) in this case 44100 / 2048, 1024 is to small to get to A0 and I think 2048 is still reasonable
    // I decided to safe the sample history of 44100+(until multiple of sampleSize) and apply a fft on a decimated copy to calculate lower frequencies
    do{
        
        float currentBase = minFreq * float(pow(2, i)); //this sets the frequency to the next octave
        
        //calculates the center frequency for the current base frequency

        float centerFreq = currentBase * 1.5;
        int stepsize = int(pow(2, i));
        
        int decimateFactor = int(floor(samplerate / (currentBase * 4)));


        // the following basically results in the next power of two from the minFreq * 4, which I could use instead of the calculation term, 
        // but in case I miss calculated I let it be there, since it is only called once
        int windowSize = (int)pow(2,ceil(log2(samplerate / (decimateFactor * stepsize*2)   )   )   ); 
        
        // I could do a fft for every note but that seems highly performance heavy
        // Also I do not know how to use a dft for specific Frequency ranges and decided to go with per octave 
        fInfos.push_back({samplerate, centerFreq, centerFreq/2, decimateFactor, windowSize}); 
        
        Iir::Butterworth::BandPass<4> newBPFilter;
        newBPFilter.setup(samplerate, centerFreq,centerFreq/2);
        filters.push_back(newBPFilter);
        

        
        //Checking on stuff
        //std::cout
        //    << fInfos.back().samplerate << " "
        //    << fInfos.back().centerFrequency << " "
        //    << fInfos.back().bandwidth << " "
        //    << fInfos.back().decimation << " "
        //    << fInfos.back().window << " "
        //    << std::endl;
        //
        i++;
    } while (minFreq * pow(2, i) < maxFreq);
   

    //Reserves as manny FreqData elements as filter +1 to be used later
    for (std::vector<FreqElement> data : freqData) {
        data.clear();
        //Just to be sure
    }
    freqData.clear();
    freqData = std::vector<std::vector<FreqElement>>(i);
    //Unsure how necessary this is. but I like starting clean
    
    fundFreqHistory.reserve(maxDFHSize + 1);
    

    maxMult = ceil(samplerate / sampleSize);
}

void AudioProcessor::ClearProcessor()
{
    Pa_StopStream(stream);
    Pa_Terminate();
}

void AudioProcessor::WindowSamples(std::vector<float>& samples, int filternumber)
{
    //std::cout << "pre windowed size " << samples.size();

    int targetSampleSize = fInfos[filternumber].window; 
    int windowSize = targetSampleSize; 
    if (samples.size() < targetSampleSize) {
        windowSize = samples.size();
    }
    
    samples.erase(samples.begin(), samples.end() -  windowSize );
    ApplyWindowFilter(samples);
    
    
    //This has a potential risk of being out of scope if stepSize or minFreq in InitFilters are picked poorly 
    // or just trying to get 1hz resolution at the low frequencies might lead to this, so size check is necessary
    
}

void AudioProcessor::ApplyWindowFilter(std::vector<float>& samples) {

    // This applies the hamming window filter to decrease spikes at the start and end of the sample
    int size = samples.size();
    if (size < 2) return; // avoid division by 0 
    for (int i = 0; i < size; ++i) {
        float windowValue = 0.54 - 0.46 * cos(2 * M_PI * i / (size - 1));

        
    }
        
    
}

void AudioProcessor::Decimate(std::vector<float>& samples, int decimation_factor) {
   
    
    
    std::vector<float> decimatedSamples;
    int j = 0;
    for (int i = 0; i < samples.size(); i += decimation_factor) {
        decimatedSamples.push_back(samples[i]);
        
    }

    
    
    samples = decimatedSamples;
    decimatedSamples.clear();
}

//private constructor
AudioProcessor::AudioProcessor() {
    
    startTime = std::chrono::high_resolution_clock::now();
    sampleHistory = std::vector<float>(samplerate,0);
    InitProcessor();
    initializePortAudio();
    Pa_StartStream(stream);
    

}

AudioProcessor::~AudioProcessor() {
    ClearProcessor();
}

std::vector<float>  AudioProcessor::GenerateSineWave(float frequency, float amplitude, int sampleRate, int numSamples) {
    std::vector<float> samples(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        float time = (float)i / sampleRate;
        samples[i] = amplitude * sin(2 * M_PI * frequency * time);
    }
    return samples;
}


void AudioProcessor::PitchDetection(int framesPerBuffer, float* in) {
    
    uint_t win_s = framesPerBuffer;    
    uint_t hop_s = win_s / 4; 
    aubio_pitch_t* pitch_object = new_aubio_pitch("yinfast", win_s, hop_s, samplerate);

    fvec_t* vout = new_fvec(1); 
    fvec_t* vin = new_fvec(framesPerBuffer);  
    vin->length = framesPerBuffer; 

    std::copy(in, in + framesPerBuffer, vin->data);
   
    aubio_pitch_do(pitch_object, vin, vout);

    
    float pitch = fvec_get_sample(vout, 0);

    AddFundFreq(pitch); 
    
    
    del_aubio_pitch(pitch_object);
    del_fvec(vin);
    
    del_fvec(vout);
    
}



bool AudioProcessor::initializePortAudio() {
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    // Get default input and output devices
    PaDeviceIndex inputDevice = Pa_GetDefaultInputDevice();
    PaDeviceIndex outputDevice = Pa_GetDefaultOutputDevice();

    if (inputDevice == paNoDevice || outputDevice == paNoDevice) {
        std::cerr << "Error: No default input or output device found." << std::endl;
        return false;
    }

    inputParameters.device = inputDevice;
    inputParameters.channelCount = 1; // Mono input
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputDevice)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = nullptr;

    outputParameters.device = outputDevice;
    outputParameters.channelCount = 1; // Mono output
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputDevice)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = nullptr;

    

    RefreshFilter(Pa_GetDeviceInfo(inputDevice)->defaultSampleRate, sampleSize, minFreq, maxFreq);

    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        &outputParameters,
        samplerate, // Sample rate
        sampleSize, // Frames per buffer
        paClipOff,
        AudioCallback,
        NULL  // No user data 
    );
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }

    return true;
}

//All below I am struggling with still, got another library to perfrom pitch detection(Aubio), but might want to revisit this later

float AudioProcessor::ACM(std::vector<float>& sig, int w, int t, int lag) {

    float sum = 0.0;
    for (int i = 0; i < w; ++i) {

        sum += sig[t + i] * sig[lag + t + i];
    }
    return sum;
}

float AudioProcessor::DetectPitch(std::vector<float>& sig, int w, int t, float sampleRate, int minBounds, int maxBounds, float thresh) {
    int sample = 0;
    bool markPeak = false;
    float last = 1;

    std::vector<float>CMNDFValues(maxBounds - 2*minBounds);
    for (int i = minBounds; i < maxBounds - minBounds; ++i) {

        
        CMNDFValues[i - minBounds] = CMNDF(sig, w, t, i);

    }





    for (int i = 0; i < CMNDFValues.size();i++) {
        if (markPeak) {
            if (last < CMNDFValues[i]) {
                sample = CMNDFValues[i - 1];
                break;
            }
    
        }
        else if (CMNDFValues[i] < thresh) {
            
            markPeak = true;
            last = CMNDFValues[i];
            
        }
    }
    

    auto minS = std::min_element(CMNDFValues.begin(), CMNDFValues.end());
    sample = std::distance(CMNDFValues.begin(), minS) + minBounds;

    if (std::abs(sample) < 1e-6) {  // Using a small tolerance to check for near-zero values
        throw std::runtime_error("Potential division by zero in pitch detection.");
    }

    return sampleRate / sample;

}

float AudioProcessor::DF(std::vector<float>& sig, int w, int t, int lag)
{
    return (ACM(sig, w, t, 0) + ACM(sig, w, t + lag, 0) - 2 * ACM(sig, w, t, lag));
}


float AudioProcessor::CMNDF(std::vector<float>& sig, int w, int t, int lag) {
    if (lag == 0) {
        return 1;
    }
    float sum = 0.0;
    for (int j = 1; j <= lag; ++j) {
        sum += DF(sig, w, t, j+1);
    }

    return (DF(sig, w, t, lag) / sum) * lag;


}