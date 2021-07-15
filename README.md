# Virtual led

Application which listens to state changes coming from a WebSocket as JSON messages. Out of the incoming state events a virtual status led is controlled.

The application uses the following libraries:
- [IXWebSocket](https://github.com/machinezone/IXWebSocket)
- [JSON](https://github.com/nlohmann/json)

Use this for compilation:
```
g++ -std=c++11 client.cpp -lixwebsocket -lz -pthread -lssl -lcrypto
```

The controlled status led supports the following properties:

-   Color: `red`, `yellow`, `blue`, `white`, `off`
-   Luminance: `(int) 0 - 100`

The behavior of the status led is according and with prioritization corresponding to the order on the list. For example, if in a given time, two or more led status could happen, the selected status will be the one higher on the list. 

-   system:error -> red@100
-   system:updating -> off -> yellow@100 (flashing with 1Hz)
-   system:booting -> red@10
-   playback:volume(changed) -> white@volume for 3s (fade out to off)
-   bluetooth:pairing -> off -> blue@100 (flashing with 2Hz)
-   playback:inactive -> off
-   playback:playing && bluetooth:connected -> blue@10
-   playback:playing -> white@10
-   playback:paused -> white@50