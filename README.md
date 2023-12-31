# Universal-Project-Interface
A handheld, ESP32 based platform intended to serve as a remote interface for all personal projects via a tile based GUI and facilitated by ESPnow/Bluetooth communications. The devices secondary purpose is to serve as an educational tool, inspired by devices like the Flipper Zero, and contains functions that allow users to sniff WiFi packets, SSID spoofing, constructing deauther frames etc

![PCB 3D](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/fafc0e64-b52c-4293-90b8-a23b6e5edec2)

# Code Structure
![v2](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/21202384-74c7-40a1-ba60-90bd14f9678d)

Before v0.3.07, the code structure was predicated upon polling the PCF8574P connected to the I2C bus, as the original hand-soldered prototype (made using prototype board and solder traces) did not have the interrupt pin connected to the ESP32 development board. 
All UI interaction is built upon the 'Primary Keypad Read Logic' function which reads inbound data from the I2C bus, originating from the PCF8574, and selects an appropriate 'Index Manager' as well as managing a number of Boolean flags. The Index Managers, when selected, provide simple rulesets that correspond to a selected function and these rulesets are reflected to the user via the 'Interface Logic' functions which are responsible for the drawn graphics.
The GUI is stored as a series of matrices in the 'graphics_definitions.h' header file and are used by the Adafruit_GFX.h library functions. As a tile based UI, the tiles contain 25px x 25px bitmap images that are stored as char arrays in the 'bimap-images.h' header file and are programmatically accessed via an array of memory pointers. Each tile is assigned a bmp image and a colour inverted bmp image, the inverted image exists to communicate to the user when a particular tile is selected or rather, the coordinate that corresponds to a bmp image is rewritten with its inverted counterpart when the index that corresponds to that tile is selected via the appropriate Index Manager function.
When a tile is selected from the Main Menu interface via the center keypad key, the index of the selected key will direct the code to the appropriate Index Manager function, deselect the previous Index Manager function and draw the graphics for the currently selected function. The tiles from the Main Menu represent different applications, each with their own respective Index Managers, Interface Logic and Drawn Graphics.

# Hardware

The first prototype was constructed from two stacked prototype boards. The first board contains five momentary contact switches connected by solder traces to a PCF8574P DIP that breaks out into a four pin male DuPont connector. 
The second board contains the female DuPont headers for connecting an ESP32 development board, a LiPo charge board and a voltage regulator. The displays selected were a 128x32 SSD1306 and a 128x64 SSD1306, both connected to the same I2C bus as the PCF8574P

![IMG_20231208_191744708sfs](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/2b7ffde4-6b3f-4f85-903a-8e0f474876e2)

The second prototype was designed in EasyEDA and the pcb was manufactored by JLCPCB. This prototype was designed to include a TP4056 SOP IC for charging a 500mAh LiPo cell, a TPS73633DBVR SOP IC voltage regulator and a number of supporting 0603 SMD resistors and capacitors. The PCF8574P remained as a DIP and the contact switches also remained THT, due only to the fact that I already possessed a number of these components. 
This pcb contained a number of design errors, first the enable pin of the TPS73633DBVR was connected to gnd when it should be connected to the voltage source and second, the 'left' momentary contact switch was shorted to ground. Both of these design errors were bypassed by disconnecting the enable pin on the TPS73633DBVR from its respective pad and shorting it to Vin and the contact of the momentary contact switch which was shorted to ground was severed and a small length of tinned copper wire was connected to the correct pin instead.

![PCB 3D front v0 2a](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/e95e17d7-638f-47de-b72e-c219be9c7191)
![IMG_20231208_191833617_HDRd](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/5467353d-a14d-4b5f-acfb-ff1c0075dc17)


The third prototype pcb was again designed in EasyEDA and manufactured by JLCPCB. This iteration addressed the design errors of the previous version, but unlike the previous version which used an ESP32 development board, this version uses an integrated ESP32 module with a USB to UART IC and voltage regulator. The QOP components are assembled by JCLPCB whereas the LEDs, 0603 resistors/capacitors, switches, TP4056, TPS73633DBVR, PCF8574P and MOLEX micro USB connectors are hand soldered.
This prototype, unlike the previous two, has the interrupt pin of the PCF8574P connected to a GPIO pin of the ESP32 module.
An issue arose during flashing the device once assembled, for proper boot mode configuration of the chosen ESP32 module, GPIO 12 needed to be pulled high. Without pulling GPIO 12 high, flashing the device resulted in a MD5 checksum error. This error was resolved by depressing any of the five tactile push buttons during the flashing process, on account of the PCF8574p's interrupt pin being connected to GPIO 12. 

![PCB 2D front v0 4gf](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/db7896dc-02e6-4c13-94e7-f65703e29fdc)
![sch](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/219fcd7b-4cd6-4e77-b125-9cc81552c333)
![v4 banner medium](https://github.com/CountZero1066/Universal-Project-Interface/assets/32957102/ee333800-904b-45a2-ac2b-52022872964b)

