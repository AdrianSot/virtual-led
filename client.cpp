#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <unistd.h>

using json = nlohmann::json;
using namespace std;

class State{
public:
    void updateState(json j){
        if(!j["system"].is_null()) system = j["system"]; 
        if(!j["playback"].is_null()) playback = j["playback"];
        if(!j["bluetooth"].is_null()) bluetooth = j["bluetooth"];
        if(!j["metadata"].is_null()) metadata = j["metadata"].dump();
        if(!j["volume"].is_null()) volume = j["volume"];
        if(!j["playbackPosition"].is_null()) playbackPosition = j["playbackPosition"];
    }
    State(){
        system = playback = bluetooth = metadata = "";
        volume = playbackPosition = 0;
    }
    string system, playback, bluetooth, metadata;
    int volume, playbackPosition;

    void print(){
        cout << "system" << ":" << system << ", playback" << ":" << playback << ", bluetooth" << ":" << bluetooth << endl;
        cout << "volume" << ":" << volume << ", playbackPosition" << ":" << playbackPosition << endl;
        cout << metadata << endl;
        cout << "--------" << endl;
    }
};

int main()
{
    State localState;
    // Websocket object
    ix::WebSocket webSocket;

    // Connect to the server
    string url("http://0.0.0.0:8808/ws");
    webSocket.setUrl(url);

    cout << "Connecting to " << url << "..." << endl;

    // Callback when message or event (open, close, error) is received
    webSocket.setOnMessageCallback([&localState](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                cout << "received message: " << msg->str << endl;
                localState.updateState(json::parse(msg->str));
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

    while (true)
    {
        localState.print();
    }

    return 0;
}