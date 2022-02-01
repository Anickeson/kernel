#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "util.h"

void kprint_c (char c){
	char * string;
	*string = c;	
	term_write(string, 1);
}

void kprint_s (const char * str){

}

void kprint_d (uint64_t value){

}

void kprint_x (uint64_t value){

}

void kprint_p(void * ptr){

}
