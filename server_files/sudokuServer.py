'''
sudokuServer.py
Name: Luke Slevinsky, Gillian Pierce
Section: EB1
CCID: lslevins, gmpierce
U of A ID: 1453199, 1458107

Description: This project implements a client server style sudoku game between
an arduino client and python server. A user is able to input their own sudoku
puzzle to be solved by the server or attempt to solve a randomly generated puzzle
(of varying difficulty levels) and have their solution checked.

'''

from adjacencygraph import UndirectedAdjacencyGraph

import time
import solver
import math # used for sqrt
import sys   # used with stdin
from cs_message import *  # used for serial communication with diagnostic msgs

def server(serial_in, serial_out):
    '''Acts as a sudoku server that accepts various requests ranging from board
    creation to solving a sudoku board. For a solve and check request, the
    entire current board is received via serial monitor from the arduino client
    and then the result, whether it be an entire solved board or just a bool
    representing if the board was represented correctly. If sudokuServer.py
    script is run, this function will run indefinitely.

    Args:
        serial_in,serial_out(textserial objects): Serial channels for
        and recieving data
    '''
    state = 0  # state 0 is waiting for input
    colours = [0]*81  # initialization of list to hold inputted and outputted graph colours
    count = 0  # loop counter
    graph = solver.make_graph()  # sudoku board graph representaton is created

    while True:
        while state == 0:  # state 0 is waiting for input from client
            log_msg("state 0")
            msg = receive_msg_from_client(serial_in)
            log_msg("state0 got {}".format(msg))
            if msg[0] == 'C':  # if C received, jump to check protocol
                log_msg("server C")
                while count < 81:  # recieve all 81 board cells
                    send_msg_to_client(serial_out, "A")
                    line = receive_msg_from_client(serial_in).split()
                    log_msg("server send {}".format(line))

                    if line == -1:
                        log_msg("Error - Reset State")
                        break

                    colours[count] = int(line[0])  # add received colours to board
                    count += 1
                count = 0
                log_msg(colours)
                state = 1
            elif msg[0] == 'F':  # if F is received, jump to solve protocol
                log_msg("I got F")
                while count < 81:  # recieve board cells
                    send_msg_to_client(serial_out, "A")
                    line = receive_msg_from_client(serial_in).split()

                    if line == -1:
                        log_msg("Error - Reset State")
                        break

                    colours[count] = int(line[0])
                    log_msg("{}".format(count))
                    count += 1
                count = 0
                log_msg(colours)
                state = 2
            elif msg[0] == 'G':  # generate board mode
                log_msg("I got G")
                send_msg_to_client(serial_out, "A")
                difficulty = receive_msg_from_client(serial_in)
                if difficulty == -1:
                    log_msg("Error - Reset State")
                    break
                else:
                    # changes difficulty to represent number of hints given:
                    #   easy - 60 hints
                    #   medium - 40 hints
                    #   hard - 20 hints
                    difficulty = 20*(int(difficulty))
                log_msg("should be difficulty {}".format(difficulty))

                state = 3 # go to board generation state

        while state == 1:  # check board state
            log_msg("state 1")
            # add graph colours to graph, via client input
            solver.add_colours(graph, colours)
            # run sudoku check algorithm
            isSolved = solver.solve_check(graph)
            log_msg("sent?")
            send_msg_to_client(serial_out, "D")
            send_msg_to_client(serial_out, str(isSolved)) # output resulting bool
            state = 0  # reset to initial state


        while state == 2:  # solve board state
            log_msg("state 2")
            # add graph colours to graph, via client input
            solver.add_colours(graph, colours)
            # run sudoku solve algorithm
            t = time.time()  # timeout timer for solver function
            solver.t = t
            # solve the board - error is 0 if error occured
            error = solver.solve(graph)
            log_msg(graph.colours())
            if not error:  # if error code was received, send error message to reset
                state = 0
                send_msg_to_client(serial_out, "-")  # send reset message to client
                break
            print(graph.colours())  #debug line for solved graph

            log_msg("sending?")
            send_msg_to_client(serial_out, "D")  # done task
            while state == 2:
                msg = receive_msg_from_client(serial_in)
                log_msg("in server")
                log_msg("server {}".format(msg))
                if msg[0] == 'A':  # if acknowledgment received, continue sending
                    if count == 81:  # all vertices sent
                        log_msg("should be done")
                        count = 0
                        send_msg_to_client(serial_out, "E")  # end of list
                        state = 0
                        break
                    # if acknowledgment has been received, send a vertex
                    send_msg_to_client(serial_out, str(graph.colour(count+1)))
                    count += 1
                else:
                    log_msg("Error - Reset State")
                    send_msg_to_client(serial_out, "-")  # send reset message to client
                    count = 0
                    state = 0
                    break

        while state == 3:

            t = time.time()  # timeout timer for gen function
            solver.t = t
            # create sudoku board with required difficulty
            new_colour_list = solver.generate_board(difficulty)

            log_msg("sending colours")
            send_msg_to_client(serial_out, "D")  # done task
            count = 0
            while state == 3:
                msg = receive_msg_from_client(serial_in)
                log_msg("in server")
                log_msg("server {}".format(msg))
                if msg[0] == 'A': # if acknowledgment received, continue sending
                    if count == 81:  # all vertices sent
                        log_msg("should be done")
                        count = 0
                        send_msg_to_client(serial_out, "E")  # end of list
                        state = 0
                        break
                    # if acknowledgment has been received, send a vertex
                    send_msg_to_client(serial_out, str(new_colour_list[count]))
                    count += 1
                else:
                    log_msg("Error - Reset State")
                    send_msg_to_client(serial_out, "-")  # send reset message to client
                    count = 0
                    state = 0
                    break
            count = 0




if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(
        description='Route finder server.',
        formatter_class=argparse.RawTextHelpFormatter,
        )

    parser.add_argument("-d",
        help="Debug on",
        action="store_true",
        dest="debug")

    parser.add_argument("-d0",
        help="Debug off",
        action="store_false",
        dest="debugOff")

    parser.add_argument("-s",
        help="Set serial port for protocol",
        nargs="?",
        type=str,
        dest="serial_port_name",
        default="/dev/ttyACM0")

    args = parser.parse_args()
    debug = args.debug
    d0 = args.debugOff

    set_logging(d0)

    import textserial
    serial_port_name = args.serial_port_name

    if serial_port_name != "0":
        log_msg("Opening serial port: {}".format(serial_port_name))
        # Open up the connection
        baudrate = 9600  # [bit/seconds] 115200 also works

        # Run the server protocol forever

        # The with statment ensures that if things go bad, then ser
        # will still be closed properly.
        # errors='ignore' allows any 1 byte character, not just the usual
        # ascii range [0,127]

        # creates graph and dictionaries with ancilliary data

        with textserial.TextSerial(
            serial_port_name, baudrate, errors='ignore', newline=None) as ser:
            server(ser, ser) # runs the server

    else:  # if no serial port use stdin and stdout
        print("No serial port. Using stdin and stdout.")
        server(sys.stdin, sys.stdout)
