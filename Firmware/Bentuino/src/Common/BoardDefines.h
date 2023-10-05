#pragma once

#ifndef MAIN_INCLUDE // Avoid showing twice

#pragma message("Compiling For " ARDUINO_BOARD)

#ifdef POWER_KEEP_PIN
#pragma message("Using Power PIN " XSTR(POWER_KEEP_PIN))
#define BOARD_INIT_POWER \
pinMode(POWER_KEEP_PIN, OUTPUT); \
digitalWrite(POWER_KEEP_PIN, HIGH); // keep pin12 high to keep board awake

#define BOARD_SHUTDOWN_POWER \
digitalWrite(POWER_KEEP_PIN, LOW);
#else

#define BOARD_INIT_POWER
#define BOARD_INIT_SHUTDOWN

#endif //POWER_KEEP_PIN

#endif // MAIN_INCLUDE


#define BoardInit \
    BOARD_INIT_POWER

#define BoardShutdown \
    BOARD_SHUTDOWN_POWER
