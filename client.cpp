#include <ixwebsocket/IXWebSocket.h>
#include <iostream>

int main()
{
    // Websocket object
    ix::WebSocket webSocket;

    // Connect to a server
    std::string url("http://0.0.0.0:8808/ws");
    webSocket.setUrl(url);

    std::cout << "Connecting to " << url << "..." << std::endl;

    // Callback when message or event (open, close, error) is received
    webSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                std::cout << "received message: " << msg->str << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Open)
            {
                std::cout << "Connection established" << std::endl;
            }
            else if (msg->type == ix::WebSocketMessageType::Error)
            {
                std::cout << "Connection error: " << msg->errorInfo.reason << std::endl;
            }
        }
    );

    // Background thread to receive messages
    webSocket.start();

    while (true)
    {
        
    }

    return 0;
}