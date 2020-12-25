#ifndef BUTTON_TRACKER_H
#define BUTTON_TRACKER_H

#define NUISANCE_PRESS -1
#define SHORT_PRESS 0
#define LONG_PRESS 1

namespace PIT{
    
    typedef char PRESS_TYPE;
    using ButtonCallback = void(*)(PRESS_TYPE); 

    class ButtonTracker{
        private:
                int button_pin;
                bool button_press_detected;
                unsigned long press_detection_time;
                unsigned long button_block_timer;
                PRESS_TYPE press_result;

                TaskHandle_t cbAction_handle;
                SemaphoreHandle_t cbAction_semaphore;
                ButtonCallback onChangeCallback;

                void buttonChangeISR();
                void callbackAction();
        public:
                ButtonTracker(int button_pin, ButtonCallback onChangeCallback);
                void start();
                void stop();
    };

}

#endif