#include "Controller.h"

// Base Controller class implementations (if any)
void Controller::handleButton(int channel, boolean state, unsigned long timestamp) {
    auto it = _buttonClicks.find(channel);
    if(it != _buttonClicks.end()) {

        if(state != it->second.lastState) {
            it->second.debounceTime = timestamp;
            it->second.lastState = state;
        }

        if(timestamp - it->second.debounceTime > _buttonDebounceDelay) {

            if(state != it->second.lastStableState) {
                boolean wasPressed = it->second.lastStableState;
                it->second.lastStableState = state;
                if(!state && wasPressed) { // Was pressed and now released
                    it->second.clicked = !it->second.clicked;
                }
            }
        }
    }
}