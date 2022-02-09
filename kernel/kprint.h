#pragma once

//Kernel Implimentation Task #1, a bunch of printing

//needed strlen for kprint_s, might come in handy elsewhere
int strlen(const char * string);

//prints character to the terminal
void kprint_c(char c);

//prints a string to the terminal without altering string in anyway
void kprint_s (const char * str);

//takes in an unsigned 64-bit integer and prints it to the terminal
void kprint_d (uint64_t value);

void kprint_x (uint64_t value);

//prints the address stored in a pointer
void kprint_p(void * ptr);

//imp of printf done in class
void kprintf(const char* format, ...); 

