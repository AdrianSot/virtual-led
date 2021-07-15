#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <unordered_set>
#include <chrono>

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

//Current time in milliseconds
#define getTime() duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

class State{
public:
    string system, playback, bluetooth, metadata;
    int volume, playbackPosition;
    bool volumeChanged;

    void updateState(json j){
        if(!j["system"].is_null()) system = j["system"];
        if(!j["playback"].is_null()) playback = j["playback"];
        if(!j["bluetooth"].is_null()) bluetooth = j["bluetooth"];

        if(!j["volume"].is_null()){
            volume = j["volume"];
            volumeChanged = true;
        }else volumeChanged = false;

        if(!j["playbackPosition"].is_null()) playbackPosition = j["playbackPosition"];
        if(!j["metadata"].is_null()) metadata = j["metadata"].dump();
    }

    State(){
        system = playback = bluetooth = metadata = "";
        volume = playbackPosition = 0;
        volumeChanged = false;
    }

    void print(){
        cout << "system" << ":" << system << ", playback" << ":" << playback << ", bluetooth" << ":" << bluetooth;
        cout << ", volume" << ":" << volume << ", playbackPosition" << ":" << playbackPosition << endl;
    }
};

class Led{
public:
    string color;
    int luminance;
    bool isFading;
    long long fadingEndTime;
    int fadingDuration;

    Led(){
        color = "off";
        luminance = 0;
        isFading = false;
        fadingDuration = 0;
        fadingEndTime = 0;
    }

    void print(){
        if(color == "off") cout << color << endl;
        else cout << color << "@" << luminance << endl;
    }

    void update(State &state){
        if(!isFading){
            if(state.playback == "paused") setColorAndLuminance("white",50);
            if(state.playback == "inactive") setColorAndLuminance("off",0);
            if(state.playback == "playing"){
                if(state.bluetooth == "connected") setColorAndLuminance("blue",10);
                else setColorAndLuminance("white",10);
            }

            if(state.bluetooth == "pairing") flash("blue", 100, 2);
            else if(state.bluetooth == "connected" && state.playback == "playing"){
                setColorAndLuminance("blue",10);
            }
        }

        if(state.volumeChanged){
            setColorAndLuminance("white", state.volume, true);
            fadeOff(3);
        }

        if(state.system == "error") setColorAndLuminance("red",100);
        if(state.system == "updating") flash("yellow",100,1);
        if(state.system == "booting") setColorAndLuminance("red",10); 
    }

    void setColorAndLuminance(string col, int lum, bool fading = false){
        if(!fading) isFading = false;
        //cout << "setting color : " << col << endl;
        color = col;
        luminance = lum;
    }

    void fadeOff(int time){
        isFading = true;
        fadingDuration = time * 1000;
        fadingEndTime = getTime() 
        fadingEndTime += fadingDuration;
    }

    void fadingManager(int volume){
        if(!isFading) return;
        long long currentTime = getTime();
        //cout << "FM,  currentTime : " << currentTime << ", fadingEndTime :" << fadingEndTime << endl;
        if(currentTime >= fadingEndTime){
            setColorAndLuminance("off", 0);
            isFading = false;
        }else{
            int fadedLuminance = (fadingEndTime - currentTime) * volume / (float)fadingDuration;
            //cout << "fadedLuminance : " << fadedLuminance << endl;
            if(isFading) setColorAndLuminance(color, fadedLuminance, true);
        }
    }

    void flash(string col, int lum, float hz){
        setColorAndLuminance(col,lum);
    }
};

int main()
{
    //Keep track of local state
    State localState;

    Led led;

    // Websocket object
    ix::WebSocket webSocket;

    // Connect to the server
    string url("http://0.0.0.0:8808/ws");
    webSocket.setUrl(url);

    cout << "Connecting to " << url << "..." << endl;

    // Callback when message or event (open, close, error) is received
    webSocket.setOnMessageCallback([&localState, &led](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                cout << "received message: " << msg->str << endl;
                localState.updateState(json::parse(msg->str));
                localState.print();
                led.update(localState);
                led.print();
                cout << "---------" << endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Open)
            {
                cout << "Connection established" << endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Error)
            {
                cout << "Connection error: " << msg->errorInfo.reason << endl;
            }
        }
    );

    // Background thread to receive messages
    webSocket.start();
    unsigned long long timeNow, lastPrint = 0;
    while (true)
    {
        timeNow = getTime();
        if(timeNow - lastPrint > 250) {
            led.fadingManager(localState.volume);
            led.print();
            lastPrint = getTime();
        }
    }

    return 0;
}