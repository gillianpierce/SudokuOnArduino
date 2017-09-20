#ifndef SERIAL_HANDLING_H
#define SERIAL_HANDLING_H

#include <stdint.h>

int16_t serial_readline_timed(char *line, uint16_t line_size, long timeout);

int16_t serial_readline(char *line, uint16_t line_size);

bool check_board(int board[9][9]);

int8_t solve_board(int board[9][9]);

int8_t gen_board(uint8_t difficulty, int board[9][9]);

#endif
