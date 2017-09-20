/*
* sudoku.cpp
* Name: Luke Slevinsky, Gillian Pierce
* Section: EB1
* CCID: lslevins, gmpierce
* U of A ID: 1453199, 1458107
*
* Description: This project implements a client server style sudoku game between
*   an arduino client and python server. A user is able to input their own sudoku
*   puzzle to be solved by the server or attempt to solve a randomly generated puzzle
*   (of varying difficulty levels) and have their solution checked.
*/

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h> // SD library

#include "ArduinoExtras.h"
#include "hashtable.h" // For hashtable
#include "lcd_image.h" // contains lcd_image_draw, used for squares and background

#include "serial_handling.h" // contains needed serial communication functions for client side
#include "dprintf.h"  // useful debug printing

// Standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS    5 // Chip select line for SD card.
#define TFT_CS   6 // Chip select line for TFT display.
#define TFT_DC   7 // Data/command line for TFT display.
#define TFT_RST  8 // Reset line for TFT (or connect to +5V).
#define JOY_SEL  9 // Pin 9 responds to clicking the joystick.
#define JOY_VERT_ANALOG 0 // Pin A0 responds to vertical joystick movement.
#define JOY_HORIZ_ANALOG 1 // Pin A1 responds to horizontal joystick movement.
#define JOY_DEADZONE 64 // Only care if position is JOY_CENTRE +/- JOY_DEADZONE.
#define JOY_CENTRE 512 // The centre value for the joystick.
#define TFT_WIDTH 128 // Width of the LCD.
#define TFT_HEIGHT 160 // Height of the LCD.

#define randomAnalog 7 // Analog pin used to generate random number.
#define potentPin 2 // potentiometer pin used for difficulty
#define selectButton 22 //for selection of numbers
#define solveButton 23 //for solving
#define checkButton 24 //for solving

// Initialize the Adafruit LCD.
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Initialize the SD card.
Sd2Card card; // SD card reads require 512 byte buffer

// Set the DDArduino background as the backG_image
lcd_image_t backG_image = { "sudoku.lcd", 128, 128 }; // sudoku board

// These values are used in displaying/selecting names.
int8_t selection = 1; // The current selection.
int8_t old_selection = 1; // The prviously selected menu option.
int8_t update = 0; // Keeps track of vertical movement in Mode 1;

// These values are used in changing from map mode to list mode.
// the path request, start and stop lat and lon

int8_t mode = 0; // Mode 0 is the menu. Mode 1 is the game mode.
int8_t lastmode = -1; // The previous mode.

// Finite state machine controls
//  ie. cursor and button must be released before another input is taken
bool cursorMode = 0;   // To be used for input control
bool lastCursorMode = 0;
bool buttonMode = 0;
bool lastButtonMode = 0;

// maps the number between 0 and 9
uint8_t mappedEntry = 0;
uint8_t lastEntry = 0;

// corresponds to square selected on board
uint16_t row = 0;
uint16_t col = 0;
int8_t boardSquare = 0;
int8_t lastSquare = 0;
int8_t dif = 0;  // difficulty of game selected

int board[9][9] = {{0}}; // Sudoku board
HashTable doNotDisturb(30);  // HasTable to store which cells shouldn't be touched

bool check = -1;  //boolean used to represent if board is correctly solved

// Game main menu
struct Menu {
    char name[17];   //   max 17-char name
};

// Instruction menu option
struct Instructions {
    char name[26];  // max 26-char name
};



// global structs
Menu sudmenu[4];
Instructions lines[18];
Menu gameMenu[5];

void displayTft(uint16_t textSize, uint16_t fg, uint16_t bg, uint16_t cursorX, uint16_t cursorY, char* text){
    /**
    Helper function to display text to TST display

    @param textSize  Size of text on TFT (0,1,2, ...)
    @param fg  foregorund colour for text to be printed
    @param bg  backgorund colour for text to be printed
    @param cursorX  x coordinate of starting top left coordinate
    @param cursorY  y coordinate of starting top left coordinate
    @param text  text to be printed

    @return Void
    */
    tft.setTextSize(textSize);
    tft.setTextColor(fg, bg);
    tft.setCursor(cursorX, cursorY);
    tft.print(text);
}

void displayTftVal(uint16_t textSize, uint16_t fg, uint16_t bg, uint16_t cursorX, uint16_t cursorY, uint16_t val){
    /**
    Helper function to display text to TST display

    @param textSize  Size of text on TFT (0,1,2, ...)
    @param fg  foregorund colour for text to be printed
    @param bg  backgorund colour for text to be printed
    @param cursorX  x coordinate of starting top left coordinate
    @param cursorY  y coordinate of starting top left coordinate
    @param val  value to be printed

    @return Void
    */
    tft.setTextSize(textSize);
    tft.setTextColor(fg, bg);
    tft.setCursor(cursorX, cursorY);
    tft.print(val);
}

void print_names() {
    /**
    Prints the names from the sudmenu struct for menu screen

    @return Void
    */
    tft.fillScreen(0);
    tft.setCursor(0, 0); // where the characters will be displayed
    tft.setTextWrap(false);

    for (uint8_t i = 0; i < 4; i++) {
        if (i != selection) { // Not highlighted
            tft.setTextColor(0xFFFF, 0x0000);
        } else { // Highlighted
            tft.setTextColor(0x0000, 0xFFFF);
        }

        // Hard coded positions for menu options
        switch (i) {
            case 0:
            tft.setCursor(10, 0);
            tft.setTextSize(3);
            tft.setTextColor(0x780F, 0x0000);
            break;

            case 1:
            tft.setCursor(30, i*16+8);
            tft.setTextSize(2);
            tft.setTextColor(0x0FF0, 0x0000);
            break;

            case 2:
            tft.setCursor(45, i*32+16);
            tft.setTextSize(1);
            break;

            case 3:
            tft.setCursor(33, i*32+16);
            tft.setTextSize(1);
            break;
        }
        tft.print(sudmenu[i].name);
        tft.print("\n");
    }
    tft.print("\n");
    update = 0;
}

void update_names() {
    /**
    Used in updating of cursor selection on main menu

    @return Void
    */
    switch (selection) {

        case 2: // START
        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

        if (old_selection == 3) {
            tft.setCursor(33, old_selection*32+16);
        }

        tft.print(sudmenu[old_selection].name);

        delay(100);

        // Highlight new selection
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
        tft.setCursor(45, selection*32+16);
        tft.print(sudmenu[selection].name);

        update = 0; // Resets the update variable
        break;

        case 3: // Instructions

        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
        if (old_selection == 2) {
            tft.setCursor(45, old_selection*32+16);
        }


        tft.print(sudmenu[old_selection].name);

        delay(100);

        // Highlight new selection
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
        tft.setCursor(33, selection*32+16);
        tft.print(sudmenu[selection].name);
        update = 0; // Resets the update variable
        break;
    }
}

void print_instruct() {
    /**
    Prints data off of instruction struct

    @return Void
    */
    tft.fillScreen(0);
    tft.setCursor(0, 0); // where the characters will be displayed
    tft.setTextColor(0xFFFF, 0x0000);
    tft.setTextWrap(false);

    for (uint8_t i = 0; i < 18; i++) {
        tft.print(lines[i].name);
        tft.print("\n");
    }
}

void scanJoystick() {
    /**
    Scans joystick fornew inputs. Used in navigating the menu

    @return Void
    */

    // Get the vertical/horizontal/clicked state of the joystick.
    int vert = analogRead(JOY_VERT_ANALOG);
    int horiz = analogRead(JOY_HORIZ_ANALOG);
    int select = digitalRead(JOY_SEL);

    // If button is presed, change mode
    if (select == 0) {
        lastmode = mode;
        mode = selection-1;
    }
    // checks if joystick is up or down
    if (mode == 0){
        if (abs(vert - JOY_CENTRE) > JOY_DEADZONE) {
            // If the joystick is up, the reading is smaller
            if (vert - JOY_CENTRE < 0) {
                // Subtract from selection variable.
                update = -1;
            }
            else {
                // Add to selection variable.
                update = 1;
            }
        }
    }
}


void print_options() {
    /**
    Print off the difficulty options menu

    @return Void
    */

    tft.fillScreen(0);
    tft.setCursor(0, 0); // where the characters will be displayed

    for (uint8_t i = 0; i < 5; i++) {
        if (i != selection) { // Not highlighted
            tft.setTextColor(0xFFFF, 0x0000);
        } else { // Highlighted
            tft.setTextColor(0x0000, 0xFFFF);
        }

        // Hard coded positions for menu options
        switch (i) {
            case 0:  // Difficulty title
            tft.setCursor(0, 0);
            tft.setTextSize(2);
            tft.setTextColor(0x0FF0, 0x0000);
            break;

            case 1:  // Hard
            tft.setCursor(50, i*16+16);
            tft.setTextSize(1);
            break;

            case 2:  // Medium
            tft.setCursor(45, i*16+16);
            tft.setTextSize(1);
            break;

            case 3:  // Easy
            tft.setCursor(50, i*16+16);
            tft.setTextSize(1);
            break;

            case 4:  // Custom board - input your own board
            tft.setCursor(45, i*16+16);
            tft.setTextSize(1);
            break;

        }
        delay(100);
        tft.print(gameMenu[i].name);
        tft.print("\n");
    }
    tft.print("\n");
    update = 0;
}


void update_options() {
    /**
    Used in updating of cursor selection on game menu

    @return Void
    */

    switch (selection) {
        case 1: // Hard
        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

        if (old_selection == 4) {
            tft.setCursor(45, old_selection*16+16);
        }
        else if (old_selection == 2) {
            tft.setCursor(45, old_selection*16+16);
        }

        tft.print(gameMenu[old_selection].name);

        delay(500);

        // Highlight new selection
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
        tft.setCursor(50, selection*16+16);
        tft.print(gameMenu[selection].name);

        update = 0; // Resets the update variable
        break;

        case 2: // Medium
        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);

        if (old_selection == 3) {
            tft.setCursor(50, old_selection*16+16);
        }
        else if (old_selection == 1) {
            tft.setCursor(50, old_selection*16+16);
        }

        tft.print(gameMenu[old_selection].name);

        delay(500);

        // Highlight new selection
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
        tft.setCursor(45, selection*16+16);
        tft.print(gameMenu[selection].name);

        update = 0; // Resets the update variable
        break;

        case 3: // Easy

        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
        if (old_selection == 2) {
            tft.setCursor(45, old_selection*16+16);
        }
        else if (old_selection == 4) {
            tft.setCursor(45, old_selection*16+16);
        }
        tft.print(gameMenu[old_selection].name);

        delay(500);

        // Highlight new selection
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
        tft.setCursor(50, selection*16+16);
        tft.print(gameMenu[selection].name);
        update = 0; // Resets the update variable
        break;

        case 4: // Custom

        tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
        if (old_selection == 1) {
            tft.setCursor(50, old_selection*16+16);
        }
        else if (old_selection == 3) {
            tft.setCursor(50, old_selection*16+16);
        }
        tft.print(gameMenu[old_selection].name);

        delay(500);

        // Highlight new selection
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
        tft.setCursor(45, selection*16+16);
        tft.print(gameMenu[selection].name);
        update = 0; // Resets the update variable
        break;
    }
}


void scanDifficulty() {
    /**
    Scans joystick fornew inputs. Used in selecting a difficulty

    @return Void
    */

    // Get the vertical/horizontal/clicked state of the joystick.
    int vert = analogRead(JOY_VERT_ANALOG);
    int horiz = analogRead(JOY_HORIZ_ANALOG);
    int select = digitalRead(JOY_SEL);

    // If button is presed, change mode
    if (select == 0) {
        dif = selection;  // return chosen difficulty
    }
    // checks if joystick is up or down
    if (mode == 1){
        if (abs(vert - JOY_CENTRE) > JOY_DEADZONE) {
            // If the joystick is up, the reading is smaller
            if (vert - JOY_CENTRE < 0) {
                // Subtract from selection variable.
                update = -1;
            }
            else {
                // Add to selection variable.
                update = 1;
            }
        }
    }
}

void boardInp() {
    /**
    Takes input from joystick in game

    @return Void
    */

    // Get the vertical/horizontal/clicked state of the joystick.
    int vert = analogRead(JOY_VERT_ANALOG);
    int horiz = analogRead(JOY_HORIZ_ANALOG);
    int select = digitalRead(JOY_SEL);

    // if direction is detected, stateMachine state is turned on.
    // Determines whether joystick input is up or down
    if (abs(vert - JOY_CENTRE) > JOY_DEADZONE) {
        // If the joystick is up, the reading is smaller
        if (vert - JOY_CENTRE < 0) {
            // Subtract from selection variable.
            lastCursorMode = cursorMode;
            if (lastCursorMode == 0) {
                cursorMode = 1;
                boardSquare -= 9;  // go down a col
                update = 1;
                if (boardSquare < 0) {
                    boardSquare += 81;  // if at the bottom of the scree, go to the top
                }
            }
        } else {
            // Add to selection variable.
            lastCursorMode = cursorMode;
            if (lastCursorMode == 0) {
                cursorMode = 1;
                boardSquare += 9;  // go up a col
                update = 1;
                if (boardSquare > 80) {
                    boardSquare -= 81; // if at the bottom of the screen, go to the top
                }
            }
        }
    }
    // determines whether joystick input is left or right

    else if (abs(horiz - JOY_CENTRE) > JOY_DEADZONE) {
        // If the joystick is left, the reading is smaller
        if (horiz - JOY_CENTRE < 0) {
            // Subtract from selection variable.
            lastCursorMode = cursorMode;
            if (lastCursorMode == 0) {
                cursorMode = 1;
                boardSquare -= 1; // go left in a row
                update = 1;
                if (boardSquare < 0) {
                    boardSquare += 81; // if at 0, go to 81
                }
            }
        } else {
            // Add to selection variable.
            lastCursorMode = cursorMode;
            if (lastCursorMode == 0) {
                cursorMode = 1;
                boardSquare += 1;  // go right in a row
                update = 1;
                if (boardSquare > 80) {
                    boardSquare -= 81;  // if at 81, go to 0
                }
            }
        }
    }

    else if ((digitalRead(selectButton) == LOW)) {
        lastButtonMode = buttonMode;
        // allow changes to number on board if button has been pressed and released
        //  and coordinates are not in doNotDisturb table (don't remove hints)
        if (!lastButtonMode && !doNotDisturb.exists(row*9 + col)){
            displayTft(1, 0xF800, ST7735_BLACK, 75, 130, "SETTING:");
            int entry;
            entry = analogRead(potentPin);
            lastEntry = map(entry, 0, 1023, 0, 9);
            while(digitalRead(selectButton) == LOW) {}
            while(digitalRead(selectButton) == HIGH) {
                buttonMode = 1;
                entry = analogRead(potentPin);
                mappedEntry = map(entry, 0, 1023, 1, 10);
                if (mappedEntry != lastEntry){  // if number has changed, display change
                    if (mappedEntry < 10){
                        displayTftVal(1, ST7735_RED, ST7735_WHITE, col*14+5, row*14+5, mappedEntry);
                        update = 1;
                        lastEntry = mappedEntry;
                        board[row][col] = mappedEntry;
                    }
                }
            }
        }
    }

    else {  // reset finite state vars if nothing is happening
        cursorMode = 0;
        buttonMode = 0;
    }
}

void update_square() {
    /**
    Used in updating of cursor selection

    @return Void
    */

    // get row and column from square number
    row = boardSquare / 9;
    col = boardSquare % 9;

    // Highlight new selection
    tft.setTextSize(1);
    if (doNotDisturb.exists(row*9 + col)) {
        tft.setTextColor(ST7735_BLACK, ST7735_WHITE);
    }
    else {
        tft.setTextColor(ST7735_RED, ST7735_WHITE);

    }
    if (board[row][col] != NULL){
        tft.setCursor(col*14+5, row*14+5);
        tft.print(board[row][col]);

    }

    delay(100);
    update = 0; // Resets the update variable
}

void display_board() {
    /**
    displays the current inputted values on the board

    Complexity: O(|V|)
        Where V is the number of verticies in the graph

    @return Void
    */
    for (uint8_t i=0; i<9; i++) {
        for (uint8_t j=0; j<9; j++){
            if (board[i][j] != NULL){
                displayTftVal(1, ST7735_BLACK, ST7735_WHITE, j*14+5, i*14+5, board[i][j]);
            }
        }
    }
}

void clearBoard() {
    /**
    Resets the board with all 0's
    */
    memset(board, 0, sizeof(board[0][0]) * 9 * 9);
}

void setDND(int board[9][9]) {
    /**
    Adds the hints to a HashTable to reference when checking to see if users
        can edit certain cells

    @param board  the 2D array representing the sudoku board

    Complexity: O(|V|)
        where V are the vertices in the graph

    @return Void
    */

    for (uint8_t i=0; i<9; i++) {
        for (uint8_t j=0; j<9; j++){
            if (board[i][j] != NULL){
                char* c;
                sprintf(c, "%d", board[i][j]);
                doNotDisturb.set(i*9 + j, c);
            }
        }
    }
}

void clearDND() {
    /**
    Removes all values from HashTable to reset the game

    Complexity: O(|V|)
        where V are the vertices in the graph

    @return Void
    */

    for (uint8_t i=0; i<9; i++) {
        for (uint8_t j=0; j<9; j++){
            if (doNotDisturb.exists(i*9 + j)){
                doNotDisturb.remove(i*9 + j);
            }
        }
    }
}

void debugMode() {
    /**
    Debugging function to determine which mode the finite state machine is in

    @return Void
    */
    switch (mode) {
        case 0:
        dprintf("m0");
        break;

        case 1:
        dprintf("m1");
        break;

        case 2:
        dprintf("m2");
        break;

        case 3:
        dprintf("m3");
        break;

        case 4:
        dprintf("m4");
        break;
    }
}

void setup() {
    init();
    Serial.begin(9600);
    Serial.flush();    // There can be nasty leftover bits.

    // This seems to fix some SD card readblock errors.
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);

    dprintf("Initializing SPI communication for raw reads...");
    if (!card.init(SPI_HALF_SPEED, SD_CS)) {
        dprintf("failed!");
        while (true) {}
    } else {
        dprintf("OK!");
    }

    dprintf("Initializing SD card...");
    if (!SD.begin(SD_CS)) {
        dprintf("failed!");
        return;
    }
    dprintf("OK!");

    // Initialize the joystick button.
    pinMode(JOY_SEL, INPUT);
    digitalWrite(JOY_SEL, HIGH);

    // Initialize buttons
    pinMode(selectButton, INPUT);
    pinMode(solveButton, INPUT);
    pinMode(checkButton, INPUT);
    digitalWrite(selectButton, HIGH);
    digitalWrite(solveButton, HIGH);
    digitalWrite(checkButton, HIGH);

    // Hard coding Menu
    strcpy(sudmenu[0].name, "SUDOKU");
    strcpy(sudmenu[1].name, "solver");
    strcpy(sudmenu[2].name, " START ");
    strcpy(sudmenu[3].name, "Instructions");

    // Hard coding Difficulty menu
    strcpy(gameMenu[0].name, "Difficulty?");
    strcpy(gameMenu[1].name, "HARD");
    strcpy(gameMenu[2].name, "MEDIUM");
    strcpy(gameMenu[3].name, "EASY");
    strcpy(gameMenu[4].name, "Custom");


    // Hard coding Instructions
    strcpy(lines[0].name, " HOW TO PLAY SUDOKU ");
    strcpy(lines[1].name, "The board is made up");
    strcpy(lines[2].name, "of a 9x9 grid with 9");
    strcpy(lines[3].name, "3x3 boxes.");
    strcpy(lines[4].name, " ");
    strcpy(lines[5].name, "Each box, vertical");
    strcpy(lines[6].name, "and horizontal line");
    strcpy(lines[7].name, "must be filled with");
    strcpy(lines[8].name, " numbers 1-9");
    strcpy(lines[9].name, "Use the joystick to ");
    strcpy(lines[10].name, "navigate the board ");
    strcpy(lines[11].name, "And press the right ");
    strcpy(lines[12].name, "button to access the");
    strcpy(lines[13].name, "number selection.");
    strcpy(lines[14].name, "Select a number with");
    strcpy(lines[15].name, "the potentiometer and");
    strcpy(lines[16].name, "hit the button to");
    strcpy(lines[17].name, "resume =)");
}

int main() {
    setup();
    uint8_t select = 1; // intializes variable that stores click state
    while (true) {

        select = digitalRead(JOY_SEL);

        // Mode 0 is menu mode.
        while (mode == 0) {
            // if changing into menu mode, print the menu options
            if (lastmode != mode) {
                debugMode();
                print_names();
                lastmode = mode;
            }

            // Scans for joystick input
            scanJoystick();

            // selection and old_selection are used for highlighting
            // in list mode.
            old_selection = selection;
            selection = selection + update;
            if (selection == 1) {
                selection = 3;
            } else if (selection == 4) {
                selection = 2;
            }
            if (update != 0) {
                update_names();
            }
            //delay(100);
        }
        // Game mode
        if (mode == 5) {
            lcd_image_draw(&backG_image, &tft, 0, 0, 0, 0, TFT_WIDTH, TFT_HEIGHT);
            display_board();
            // Displays small game graphic at bottom of tft
            displayTft(2, ST7735_RED, ST7735_BLACK, 0, 130, "Super");
            displayTft(2, ST7735_GREEN, ST7735_BLACK, 0, 145, "SOLVER");

            tft.setCursor(1,1);  // resets cursor for game start
            while(mode == 5) {
                select = digitalRead(JOY_SEL);
                if (select == 0) {
                    // if click, go back to menu
                    clearBoard();  // reset board state
                    clearDND();
                    lastmode = mode;
                    mode = 0;
                    delay(800);
                }
                boardInp();

                // Displays current selection position
                displayTft(1, 0xFFE0, ST7735_BLACK, 75, 130, "Current:");
                tft.setCursor(95, 140);
                tft.print(row);tft.print(" ");tft.print(col);
                // Serial.print(col*14+3); Serial.print(" "); Serial.println(row*14+3);

                if (update != 0) {  // updates the selection location
                    tft.setCursor(col*14+5, row*14+5);
                    update_square();
                }
                // If checkButton is pressd, go to board check routine
                if (digitalRead(checkButton) == LOW) {
                    dprintf("Should go to mode 3");

                    lastmode = mode;
                    mode = 3;
                    delay(500);
                }
                // If SolveButton is pressed, go to solve check routine
                else if (digitalRead(solveButton) == LOW){
                    dprintf("go to mode 4");
                    lastmode = mode;
                    mode = 4;
                    delay(500);
                }
            }
        }
        if (mode == 2) { // instruciton mode
            print_instruct();
            // prints the instrucitons and waits for joystick input
            while(mode == 2) {
                select = digitalRead(JOY_SEL);
                if (select == 0) {
                    // if click, go back to menu
                    mode = 0; lastmode = 2;
                    delay(800);
                }
            }
        }
        if (mode == 3) {  // board check mode
            dprintf("mode 3");
            check = check_board(board);  // run server client check routine
            dprintf("result: %d",check);
            if (check == -1) {
                dprintf("Error occured in check");
                lastmode = mode;
                mode = 0;
            }
            if (check) {
                displayTft(2, 0xF000, ST7735_WHITE, 25, 60, "CORRECT");
            }
            else{
                displayTft(2, 0xF800, ST7735_WHITE, 15, 60, "INCORRECT");
            }
            delay(3000);
            clearBoard();  // reset the board for the next game
            clearDND();
            lastmode = mode;
            mode = 0;
        }
        if (mode == 4) {  // board solve mode
            select = digitalRead(JOY_SEL);

            dprintf("mode 4");
            check = solve_board(board); // run board solving algorithm
            dprintf("in mode 4 %d", check);
            if (check != 0) {
                Serial.println(-1);
                dprintf("Error occured in solve");
                displayTft(2, 0xF800, ST7735_WHITE, 45, 60, "NOT");
                displayTft(2, 0xF800, ST7735_WHITE, 15, 90, "SOLVABLE");
                delay(1500);
                clearBoard();  // reset board state
                clearDND();
                lastmode = mode;
                mode = 0;
            }
            else {
                display_board();  //if solved, display resultant board
            }
            while (mode == 4) {
                select = digitalRead(JOY_SEL);
                if (select == 0) {
                    // if click, go back to menu
                    clearBoard();  // reset board for next game
                    clearDND();  // reset hint dict
                    lastmode = mode;
                    mode = 0;
                    break;
                }
            }
        }
        if (mode == 1) { // pre game
            print_options();
            dprintf("pre game");
            select = digitalRead(JOY_SEL);
            while (mode == 1){
                // Scans for joystick input
                scanDifficulty();

                if (dif == 4){  //custom board mode lets you input your own board
                    mode = 5;
                    dif = 0;
                }
                else if (dif != 0) {
                    gen_board(dif, board); // runs the board generating routine
                    setDND(board);  // Sets Do Not Disturb cells, ie. hint cells
                    mode = 5;
                    dif = 0;
                }
                // selection and old_selection are used for highlighting
                // in list mode.
                old_selection = selection;
                selection = selection + update;
                if (selection == 0) {
                    selection = 4;
                } else if (selection == 5) {
                    selection = 1;
                }
                if (update != 0) {
                    update_options();
                }
            }

        }
    }
    Serial.end();
    return 0;
}
