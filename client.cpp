#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <unordered_set>
#include <chrono>
#include <queue>
#include "state.h"
#include "led.h"

using json = nlohmann::json;
using namespace std;
using namespace std::chrono;

//Activate to see the received messages and the current local state
const bool PRINT_MESSAGES_AND_STATE = false;

//Current time in milliseconds
#define getTime() duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

int main()
{
    //Keep track of local state
    State localState;

    //Virutal led representation
    Led led;

    //Queue for new messages arriving
    queue<json> newJsonStates;

    // Websocket object
    ix::WebSocket webSocket;

    // Connect to the server
    string url("http://0.0.0.0:8808/ws");
    webSocket.setUrl(url);

    cout << "Connecting to " << url << "..." << endl;

    // Callback when message or event (open, close, error) is received
    webSocket.setOnMessageCallback([&newJsonStates](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                if(PRINT_MESSAGES_AND_STATE) cout << "received message: " << msg->str << endl;
                newJsonStates.push(json::parse(msg->str));
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

        //Every 250 milliseconds
        if(timeNow - lastPrint > 250) {

            //Process new messages
            while(!newJsonStates.empty()){

                //Update local state and led state
                localState.updateState(newJsonStates.front());
                newJsonStates.pop();
                led.update(localState);

                if(PRINT_MESSAGES_AND_STATE) cout << localState.data_string() << endl;
            }

            //Led flashing and fading is updated
            led.fadingManager(localState.volume);
            led.flashingManager();

            cout << led.data_string() << endl;
            lastPrint = getTime();
        }
    }

    return 0;
}