#ifndef STATE_H
#define STATE_H

using json = nlohmann::json;
using namespace std;

//Class to represent the state of the system
class State{
public:
    string system, playback, bluetooth, metadata;
    int volume, playbackPosition;
    bool volumeChanged;

    State(){
        system = playback = bluetooth = metadata = "";
        volume = playbackPosition = 0;
        volumeChanged = false;
    }
    
    //Updates attributes according to the new message
    void updateState(json j){
        if(!j["system"].is_null()) system = j["system"];
        if(!j["playback"].is_null()) playback = j["playback"];
        if(!j["bluetooth"].is_null()) bluetooth = j["bluetooth"];

        //Updates and keeps track if volume was changed
        if(!j["volume"].is_null()){
            volume = j["volume"];
            volumeChanged = true;
        }else volumeChanged = false;

        if(!j["playbackPosition"].is_null()) playbackPosition = j["playbackPosition"];
        if(!j["metadata"].is_null()) metadata = j["metadata"].dump();
    }

    //Ignores metadata and returns all the other attributes
    string data_string(){
        string out = "(system:" + system + ", playback:" + playback + ", bluetooth:" + bluetooth +
            ", volume :" + to_string(volume) + ", playbackPosition:" + to_string(playbackPosition) + ")";
        return out;
    }
};

#endif /*STATE_H*/
