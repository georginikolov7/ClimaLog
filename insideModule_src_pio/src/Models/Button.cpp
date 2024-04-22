#include "Button.h"

Button::Button(int pinNumber)
{
    this->buttonPin = pinNumber;
    pinMode(buttonPin, INPUT_PULLUP);
}
int Button::getPinNumber()
{
    return buttonPin;
}
void Button::stateChanged()
{
    if (millis() - timeSinceLastDebounce > DEBOUNCE_DELAY) {

        currentButtonState = !currentButtonState;

        if (!currentButtonState) { // button is pressed
            lastPressTime = millis();
            wasLongPressed = 0;
        } else { // button is released
            lastReleaseTime = millis();
            wasShortPressed = 0;
        }
        timeSinceLastDebounce = millis();
    }
}
bool Button::isLongPressed()
{
    if ((millis() - lastPressTime >= LONGPRESS_DURATION) && !currentButtonState && !wasLongPressed) {
        wasLongPressed = 1;
        return true;
    }
    return false;
}
bool Button::isShortPressed()
{
    if (lastReleaseTime - lastPressTime < LONGPRESS_DURATION && currentButtonState && !wasShortPressed) { // button is released and wasn't shortpressed
        wasShortPressed = 1;
        return true;
    }
    return false;
}