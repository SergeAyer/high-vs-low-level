#include "mbed.h"

int main()
{
    // constant definition 
    static constexpr std::chrono::milliseconds kBlinkingRate = 500ms;

    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);

    while (true) {
        // toggle the led state
        led = !led;

        // sleep for the blinking rate interval
        ThisThread::sleep_for(kBlinkingRate);
    }
}
