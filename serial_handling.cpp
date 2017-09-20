#include "serial_handling.h"

#include <Arduino.h>
#include <errno.h>
//#include <assert13.h>
#include "dprintf.h"

/*
    Gets an acknowledgment whether the sudoku board is solved or not.

    The function sends a request to the server listening on the serial
    port and waits for the server responding with a boolean representing
    whether the board was correctly solved or incorrectly solved.

    Inputs:

    board(*)[9] - the current sudoku board waiting to be verified

    Returns:

    1 if the board is correct, 0 if the board is incorrect

*/
bool check_board(int board[9][9]) {
    size_t buf_size = 32;
    size_t buf_len = 0;
    char buf[buf_size];
    int32_t t = 0;
    int16_t path_len = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    int check;

    // start the server communication with a check request
    delay(1000);
    dprintf("Requesting board check");
    Serial.println("C");  // Send server check request
    while (1) {
        buf_len = serial_readline_timed(buf, buf_size, 1000);
        if ((int) buf_len < 0){
            return -1;  // If timeout return error message
        }
        sscanf(buf,"%c", buf[0]);
        dprintf("%d %d", j, k);
        dprintf("first loop %c",buf[0]);

        if (k == 9){
            k = 0;
            if (j==8){
                break;
            }
            else{
                j += 1;
            }
        }
        if (buf[0] == 'A') {  // if acknowledgment was recieved, send the next cell to server
            dprintf("sending %d",board[j][k]);
            Serial.println(board[j][k]);
            k+=1;
        }
    }



    //delay(1000);

    while (true){
        ///buf_len = serial_readline_timed(buf, 2, -1);
        ///dprintf("%s",buf[0]);
        dprintf("waiting for D"); // Waiting for completion of solve
        //buf_len = serial_readline_timed(buf, buf_size, -1); // I guess I don't need this?
        sscanf(buf,"%c", buf[0]);
        dprintf("%c",buf[0]);

        if (buf_len < 0){
            return buf_len;
        }

        if (buf[0] == 'D') {  // if check is complete receive the result
            dprintf("inside check");
            buf_len = serial_readline_timed(buf, buf_size, 1000);
            sscanf(buf,"%d", &check);
            dprintf("%d",check);
            break;
        }
    }
    dprintf("Outside Check");
    return check;
}

/*
    Requests for a current sudoku board to be solved.

    The function sends a request to the server listening on the serial
    port and waits for the completed board.

    Inputs:

    board(*)[9] - the current sudoku board waiting to be verified

    Returns:

    -1 if an error occurred
    >= 0 if ok.

*/
int8_t solve_board(int board[9][9]) {
    dprintf("in solve board");
    size_t buf_size = 32;
    size_t buf_len = 0;
    char buf[buf_size];
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t jj = 0;
    uint8_t kk = 0;
    int16_t path_len = 0;
    int value = 0;
    char c[buf_size];

    // start the server communication with a solve request
    delay(1000);
    dprintf("Requesting board solve");
    Serial.println("F");  // send server solve request
    while (1) {
        buf_len = serial_readline_timed(buf, buf_size, 1000);
        if ((int) buf_len < 0){
            dprintf("error");
            return -1;  // If timeout return error message
        }
        if (k == 9){
            k = 0;
            j += 1;
        }
        sscanf(buf,"%c", buf[0]);
        dprintf("%d %d", j, k);
        dprintf("first loop %c",buf[0]);

        if (buf[0] == 'A') {  // if acknowledgment was recieved, send the next board cell
            dprintf("sending %d",board[j][k]);
            Serial.println(board[j][k]);
            k+=1;
        }
        if (k == 9 && j==8){
            break;
        }
    }dprintf("done 1");

    while (true){
        dprintf("waiting for D"); // waiting for solve to be completed
        buf_len = serial_readline_timed(buf, buf_size, -1);
        sscanf(buf,"%c", buf[0]);
        dprintf("In second loop %c",buf[0]);

        if (buf_len < 0){
            return buf_len;
        }

        if (buf[0] == 'D') {  // if solve is complete, begin receiving the board
            dprintf("inside");
            while (1) {
                Serial.println('A');
                if (kk == 9 && jj==8){
                    break;
                }

                buf_len = serial_readline_timed(buf, buf_size, 1000);
                if ((int) buf_len < 0){
                    return -1;  // If timeout return error message
                }
                if (kk == 9){
                    kk = 0;
                    jj += 1;
                }

                value =  buf[0] - '0';  // Conversion of char into int representation
                dprintf("%d %d", jj, kk);
                dprintf("second loop %d", value);


                board[jj][kk] = value;  // update the board value
                kk+=1;
            }
            buf_len = serial_readline_timed(buf, buf_size, 1000);
            Serial.read();
            sscanf(buf,"%c", &c);
            dprintf("Wait for E: %c", *c);
            if (*c == 'E') {  // end of protocol

                break;
            }
            else {
                dprintf("error-solve"); // error in transmission
                Serial.println(-1);
                return -1;
            }
        }
        else if (buf[0] == '-'){  // error in solve
            dprintf("Unsolvable");
            return -1;
        }
    }dprintf("Outside?");
    return 0;
}

/*
    Generates a random sudoku board based on inputted difficulty.

    Inputs:

    difficulty - integer representign the difficulty/number of hints

    board(*)[9] - the current sudoku board waiting for random configuration

    Returns:

    -1 if an error occurred
    >= 0 if ok.

*/
int8_t gen_board(uint8_t difficulty, int board[9][9]) {
    dprintf("in gen board");
    size_t buf_size = 32;
    size_t buf_len = 0;
    char buf[buf_size];
    uint8_t j = 0;
    uint8_t k = 0;
    int16_t path_len = 0;
    int value = 0;
    char c[buf_size];

    // start the server communication with a solve request
    delay(1000);
    dprintf("Requesting random board");
    Serial.println("G");  // Send a server request to generate a board
    while (1) {
        buf_len = serial_readline_timed(buf, buf_size, 1000);
        if ((int) buf_len < 0){
            dprintf("error");
            return -1;  // If timeout return error message
        }

        sscanf(buf,"%c", buf[0]);
        dprintf("%d %d", j, k);
        dprintf("first loop %c", buf[0]);

        if (buf[0] == 'A') {  // if acknowledgment was recieved, send the difficulty to server
            dprintf("sending %d", difficulty);
            Serial.println(difficulty);
            break;
        }
    }dprintf("done 1");

    //delay(1000);

    while (true){

        dprintf("waiting for D"); // waiting for generation to be completed
        buf_len = serial_readline_timed(buf, buf_size, -1);
        sscanf(buf,"%c", &c);
        dprintf("In second loop %c", *c);

        if (buf_len < 0){
            return buf_len;
        }

        if (buf[0] == 'D') {  // if completion reciept is recieved start receiving board
            dprintf("inside");
            while (1) {
                Serial.println('A');
                if (k == 9 && j==8){
                    break;
                }

                buf_len = serial_readline_timed(buf, buf_size, 1000);
                if ((int) buf_len < 0){
                    return -1;  // If timeout return error message
                }
                if (k == 9){
                    k = 0;
                    j += 1;
                }

                value =  buf[0] - '0';  // Conversion of char into int representation
                dprintf("%d %d", j, k);
                dprintf("second loop %d", value);


                board[j][k] = value;  // update the board
                k+=1;
            }
            buf_len = serial_readline_timed(buf, buf_size, 1000);
            Serial.read();
            sscanf(buf,"%c", &c);
            dprintf("Wait for E: %c", *c);
            if (*c == 'E') {  // end of protocol

                break;
            }
            else {
                dprintf("error-solve");  // error in transmission
                Serial.println(-1);
                return -1;
            }
        }
        else if (buf[0] == '-'){  // error in generation
            dprintf("Error in Generation");
            return -1;
        }
    }dprintf("Outside?");
    return 0;
}


/*
    Function to read a single line from the serial buffer up to a
    specified length (length includes the null termination character
    that must be appended onto the string). This function is blocking.
    The newline character sequence is given by CRLF, or "\r\n".

    Arguments:

    buffer - Pointer to a buffer of characters where the string will
        be stored.

    length - The maximum length of the string to be read.

    timeout - amount of time allotted before timeout occurs

    Preconditions:  None.

    Postconditions: Function will block until a full newline has been
        read, or the maximum length has been reached. Afterwards the new
        string will be stored in the buffer passed to the function.

    Returns: the number of bytes read, -1 if timeout occurred

*/
int16_t serial_readline_timed(char *line, uint16_t line_size, long timeout) {
    unsigned long deadline = millis() + timeout; //wraparound not a problem
    int bytes_read = 0;    // Number of bytes read from the serial port.

    // Read until we hit the maximum length, or a newline.
    // One less than the maximum length because we want to add a null terminator.
    while (bytes_read < line_size - 1){
        if (timeout>0 && millis()>deadline){
            return -1;
        }
        while (Serial.available() == 0) {
            // There is no data to be read from the serial port.
            // Wait until data is available.
        }

        line[bytes_read] = (char) Serial.read();

        // A newline is given by \r or \n, or some combination of both
        // or the read may have failed and returned 0
        if ( line[bytes_read] == '\r' || line[bytes_read] == '\n' ||
             line[bytes_read] == 0 ) {
                // We ran into a newline character!  Overwrite it with \0
                line[bytes_read] == '\0';
                break;    // Break out of this - we are done reading a line.
        } else {
            bytes_read++;
        }
    }

    // Add null termination to the end of our string.
    line[bytes_read] = '\0';
    return bytes_read;
}

/*
    Function to read a single line from the serial buffer up to a
    specified length (length includes the null termination character
    that must be appended onto the string). This function is blocking.
    The newline character sequence is given by CRLF, or "\r\n".

    Arguments:

    buffer - Pointer to a buffer of characters where the string will
        be stored.

    length - The maximum length of the string to be read.

    Preconditions:  None.

    Postconditions: Function will block until a full newline has been
        read, or the maximum length has been reached. Afterwards the new
        string will be stored in the buffer passed to the function.

    Returns: the number of bytes read

*/
int16_t serial_readline(char *line, uint16_t line_size) {
    int bytes_read = 0;    // Number of bytes read from the serial port.

    // Read until we hit the maximum length, or a newline.
    // One less than the maximum length because we want to add a null terminator.
    while (bytes_read < line_size - 1) {
        while (Serial.available() == 0) {
            // There is no data to be read from the serial port.
            // Wait until data is available.
        }

        line[bytes_read] = (char) Serial.read();

        // A newline is given by \r or \n, or some combination of both
        // or the read may have failed and returned 0
        if ( line[bytes_read] == '\r' || line[bytes_read] == '\n' ||
             line[bytes_read] == 0 ) {
                // We ran into a newline character!  Overwrite it with \0
                break;    // Break out of this - we are done reading a line.
        } else {
            bytes_read++;
        }
    }

    // Add null termination to the end of our string.
    line[bytes_read] = '\0';
    return bytes_read;
    }
