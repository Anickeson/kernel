//header file for Printing Function Kernel Implimentation

//Print a single character to the terminal
void kprint_c(char c);

//Print a string to the terminal
void kprint_s(const char * str);

//Print an unsigned 64-bit integer value to the terminal in decimal notation
//no leading zeroes
void kprint_d(uint64_t value);

//Print an unsigned 64-bit integer value to the terminal in lowercase hexadecimal notation
//no leading zeros or "0x"
void kprint_x(uint64_t value);

//Print the value of a pointer to the terminal in lowercase hexadecimal with the prefix "0x"
void kprint_p(void * prt);
