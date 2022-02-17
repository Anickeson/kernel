#pragma once

void write_to_buff(uint64_t ch);

int read_from_buff();

/**
 * Read one character from the keyboard buffer. If the keyboard buffer is empty this function will block until a key is pressed.
 *
 * \returns the next character input from the keyboard
 */
char kgetc();

