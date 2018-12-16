<b> Real-Time ATmega328p thread-based scheduler for multi-threading with AVR controllers </b> <br />
<br />
This project has academic purposes, the objetive was to understand how a scheduler is made and how it works.
The objetive was reached with success, working in this project provided me a great understand about how a RTOS works.
<br />
<br />
This kernel is cooperative for now, i.e, each thread calls yield() when finished its processing block to 
free the microcontroller to be used in another threads. <br />
Totally compatible and tested in Arduino board with ATmega328p microcontroller. <br />
 <br />
 <b>Build</b>
 <br />
 To build this project, run the script faztudo.sh. <br />