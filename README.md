# s(Arduino)ku
> Arduino for Sudoku!

This project implements a client server style sudoku game between
an arduino client and python server. A user is able to input their own sudoku
puzzle to be solved by the server or attempt to solve a randomly generated puzzle
(of varying difficulty levels) and have their solution checked.

## Accessories:

* 1x Arduino Mega Board (AMG)
* 1x Adafruit 1.8" 18-bit color TFT LCD
* 1x Sparkfun COM-09032 Thumb Joystick and breakout board
* 1x Kingston flash memory card - 4GB - microSDHC
* 3x Push Buttons
* 1x Potentiometer

## Wiring instructions:

#### TFT Display

>Note: LCD pins below are listed from left to right (LCD is portrait with pins facing user).

LCD GND <--> Arduino GND bus

LCD VCC <--> Arduino positive bus

LCD RESET <--> Arduino Pin 8

LCD D/C (Data/Command) <--> Arduino Pin 7

LCD CARD_CS (Card Chip Select) <--> Arduino Pin 5

LCD TFT_CS (TFT/screen Chip Select) <--> Arduino Pin 6

LCD MOSI (Master Out Slave In) <--> Arduino Pin 51

LCD SCK (Clock) <--> Arduino Pin 52

LCD MISO (Master In Slave Out) <--> Arduino 50

LCD LITE (Backlite) <--> Arduino positive bus

#### Joystick

>Note: Joystick pins below are listed from right to left (Joystick is portrait with pins facing away from user).

Joystick VCC <--> Arduino positive bus

Joystick VERT <--> Arduino Pin A0

Joystick HOR <--> Arduino Pin A1

Joystick SEL <--> Arduino Pin 9

Joystick GND <--> Arduino GND bus

#### Potentiometer

>Note: The potentiometer instructions below assume that the middle pin is facing away from the user

Middle potentiometer lead <--> Arduino pin A2

Left potentiometer lead <--> Arduino GND bus

Right potentiometer lead <--> Arduino Positive bus

#### Select button

One button pin <--> Arduino GND Bus

Other button pin <--> Arduino Pin 22

#### Solve button
One button pin <--> Arduino GND Bus

Other button pin <--> Arduino Pin 23

#### Check button
One button pin <--> Arduino GND Bus

Other button pin <--> Arduino Pin 24

Arduino positive bus <--> Arduino 5V pin

Arduino GND bus <--> Arduino GND pin

## Running the Code

Unzip the project file and navigate to the unzipped
folder from the commad line. Upload the appropriate client files to the arduino
by typing "make upload". Next run the server by typing "python3 sudokuServer.py"
while in the server_files directory.
The welcome screen should now be displayed on the Arduino's LCD screen. If not,
server and client may need to be re-synced which can be done by pressing the
Arduino's reset button. The welcome screen shows options to start the game or
read the instructions. Options can be navigated through by moving the joystick up
or down. If the start option is selcted by pressing the joystick, a second screen
with difficulty options will be displayed. These options control how many hints
are given in the randomly generated puzzle. A final option to create a custom
puzzle is also provided. This custom board allows you to enter a board of your
choosing. After selecting a difficulty, a puzzle is displayed on the
LCD screen. The given hints cannot be modified, but the other squares can be
assigned values by pressing the select button and then turning the potentiometer
to select a value from 1-9. Pressing the select button again exits the setting mode
and the user is free to move around the board once more. The board can be navigated
by using the joystick. The current board coordinates are displayed in the bottom
right hand corner.When the user has filled the board they can press the check
board button at which time the board will be received by the server and an message
indicating whether or not their solution is correct will be displayed on the LCD
screen. Alternatively, the user can have the board solved for them. The current
board will be sent to the server and the recursive backtracking algorithm will
be run. The solved board is then sent back to the arduino to be displayed. The
game will reset after the joystick is clicked.

The hashtable class used for the doNotDisturb hint hashtable is defined in
  hastable.cpp and hashtable.h files.
The diagnostic printing on the arduino side is defined in the dprintf.cpp and
  dprintf.h files.
Functions for the drawing of lcd images to the tft display is defined in the
  lcd_image.cpp and lcd_image.h files.
Arduino.h, ArduinoExtras.cpp, and ArduinoExtras.h are included for Arduino
  features as well as the assert() function.
The Arduino client uses functions from serial_handling.cppp and serial_handling.h
  to handle server communication.
The MakeFile included in this project is the same one used in the class; no
  changes have been made to it.
On the server side, cs_message contains functions that aide in the server
  client serial communication.
textserial.py provides a textwrapper class for the serial communication.
The graph class is provided in adjacencygraph.py.
The check, solve, and makegrpah algorithms are all in solver.py.cd_image.h files.
Arduino.h, ArduinoExtras.cpp, and ArduinoExtras.h are included for Arduino
 features as well as the assert() function.
The Arduino client uses functions from serial_handling.cppp and serial_handling.h
  to handle server communication.
The MakeFile included in this project is the same one used in the class; no
  changes have been made to it.
On the server side, cs_message contains functions that aide in the server
  client serial communication.
textserial.py provides a textwrapper class for the serial communication.
The graph class is provided in adjacencygraph.py.
The check, solve, and makegrpah algorithms are all in solver.py.

## Comments

NOTE: When starting up the python server, if the diagnostic message m0 does
   not get printed, the server and client are out of sync. You can either press
   the reset button on the Arduino itself or unplug/replug the Arduino and try
   running the server again.
   
NOTE: If any number higher than 9 gets printed when you are adjusting the
   potentiometer, it is likely the potentiometer isn't plugged into the analog
   port very well. Try reseting the Arduino and regplugging the potentiometer.
   
NOTE: This code assumes that the hardware is set up as follows:
  Arduino is landscape with breadboard facing user, screen is portrait with pins
  facing user, and joystick is portrait with pins facing away from user.
  
NOTE: This code assumes that the .lcd images found in the images subfolder
  in the sudoku (main) folder are stored on the SD card.
  
NOTE: If you are having issues with the buttons, it may be useful to debounce
  with a capacitor.
  
NOTE: The LCD image of the blank board was adapted from the .jpg file found at
  https://s-media-cache-ak0.pinimg.com/564x/4b/bf/65/4bbf6534dd4748bc16e301485
  79783f7.jpg
