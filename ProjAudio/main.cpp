#include "initialization.h"
#include "Visualizer.h"



int main() {

    
    AudioProcessor& proc = AudioProcessor::GetInstance(); //This initalises the instance it returns, so InitFilters gets called and PA stuff
    Visualizer& vis = Visualizer::GetInstance();//Visualizer is depndent on AudioProcessor, which might not be ideal but have to figure that out
    
    bool running = true; 
    while (vis.GetRunning())
    {
        vis.Process(); 
    
    }
    
    return 0;
}

