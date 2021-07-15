#ifndef LED_H
#define LED_H

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

//Current time in milliseconds
#define getTime() duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

//Class to represent the virtual led
class Led{
private:
    string color;
    int luminance;

    //Variables used for fading off implementation 
    bool isFading;
    unsigned long long fadingEndTime;
    int fadingDuration;

    //Variables used for flashing implementation 
    bool isFlashing;
    float flashHz;
    unsigned long long flashingStartTime;
    string flashingColor;
    
public:
    Led(){
        color = "off";
        luminance = 0;
        isFading = false;
        fadingDuration = 0;
        fadingEndTime = 0;
        isFlashing = false;
        flashHz = 0;
        flashingStartTime = 0;
        flashingColor = "off";
    }

    //Updates led status according to the system state
    //Priority of status is according to the requirements list
    void update(State &state){
        if(!isFading){
            if(state.playback == "paused") setColorAndLuminance("white",50);
            if(state.playback == "playing"){
                if(state.bluetooth == "connected") setColorAndLuminance("blue",10);
                else setColorAndLuminance("white",10);
            }
            if(state.playback == "inactive") setColorAndLuminance("off",0);

            if(state.bluetooth == "pairing") flash("blue", 100, 2);
        }

        if(state.volumeChanged){
            setColorAndLuminance("white", state.volume, true);
            fadeOff(3);
        }

        if(state.system == "booting") setColorAndLuminance("red",10); 
        if(state.system == "updating") flash("yellow",100,1);
        if(state.system == "error") setColorAndLuminance("red",100);
    }

    //Sets color, luminance and helps with flashing and fading 
    void setColorAndLuminance(string col, int lum, bool fading = false, bool flashing = false){
        if(!fading) isFading = false;
        if(!flashing) isFlashing = false;
        else flashingColor = col;
        color = col;
        luminance = lum;
    }

    //Sets whats needed for fading off, fading duration and ending time
    void fadeOff(int time){
        isFading = true;
        fadingDuration = time * 1000;
        fadingEndTime = getTime() 
        fadingEndTime += fadingDuration;
    }

    //Keeps track of the fading status 
    void fadingManager(int volume){
        if(!isFading) return;
        long long currentTime = getTime();
        //If fading time is over, stop fading and turn led off
        if(currentTime >= fadingEndTime){
            setColorAndLuminance("off", 0);
            isFading = false;
        //If still fading, update the led luminance accordingly
        }else{
            int fadedLuminance = (fadingEndTime - currentTime) * volume / (float)fadingDuration;
            setColorAndLuminance(color, fadedLuminance, true);
        }
    }

    //Sets the flashing conditions
    void flash(string col, int lum, float hz){
        if(!isFlashing){
            flashingStartTime = getTime();
            isFlashing = true;
        }
        setColorAndLuminance(col,lum, false, true);
        flashHz = hz;
    }

    //Keeps track of flashing status
    void flashingManager(){
        if(!isFlashing || flashHz <= 0) return;
        unsigned long long currentTime = getTime();
        unsigned long timeSinceFlashing = currentTime - flashingStartTime;

        //Determines flashing cycle
        unsigned int flashingCycle = 1000 * (1.0/flashHz);
        if(timeSinceFlashing % flashingCycle > flashingCycle / 2) color = flashingColor;
        else color = "off";
        
    }

    //Return data as requested string
    string data_string(){
        string out = color;
        if(color != "off") out += "@" + to_string(luminance);
        return out;
    }
};

#endif /*LED_H*/