#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <unordered_set>
#include <chrono>

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

//Current time in milliseconds
//#define getTime() duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

class State{
public:
    string system, playback, bluetooth, metadata;
    int volume, playbackPosition;
    unordered_set<string> changed;

    void updateState(json j){
        if(!j["system"].is_null()) {
            system = j["system"];
            changed.insert("system");
        }else changed.erase("system");

        if(!j["playback"].is_null()){
            playback = j["playback"];
            changed.insert("playback");
        }else changed.erase("playback");

        if(!j["bluetooth"].is_null()) {
            bluetooth = j["bluetooth"];
            changed.insert("bluetooth");
        }else changed.erase("bluetooth");

        if(!j["volume"].is_null()){
            volume = j["volume"];
            changed.insert("volume");
        }else changed.erase("volume");

        if(!j["playbackPosition"].is_null()) playbackPosition = j["playbackPosition"];
        if(!j["metadata"].is_null()) metadata = j["metadata"].dump();
    }

    State(){
        system = playback = bluetooth = metadata = "";
        volume = playbackPosition = 0;
    }

    void print(){
        cout << "system" << ":" << system << ", playback" << ":" << playback << ", bluetooth" << ":" << bluetooth << endl;
        cout << "volume" << ":" << volume << ", playbackPosition" << ":" << playbackPosition << endl;
        cout << metadata << endl;
        cout << "--------" << endl;
    }
};

class Led{
public:
    string color;
    int luminance;

    Led(){
        color = "off";
        luminance = 0;
    }

    void print(){
        if(color == "off") cout << color << endl;
        else cout << color << "@" << luminance << endl;
    }

    void update(State &state){
        
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
                led.update(localState);
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
    //unsigned long long timeNow, lastPrint = 0;
    while (true)
    {
        /*
        timeNow = getTime();
        if(timeNow - lastPrint > 500) {
            cout << localState.changed.size() << (timeNow % 1000) <<endl;
            lastPrint = getTime();
        }
        */
    }

    return 0;
}