Real-Time ATmega328p thread-based scheduler for multi-threading with AVR controllers.
This kernel is cooperative for now, i.e, each thread calls yield() when finished its processing block to free the cpu to be used in another threads.
Totally compatible and tested in Arduino board with ATmega328p microcontroller.
