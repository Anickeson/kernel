#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "util.h"
#include "kprint.h"
#include "circ_buff.h"

#define BUFF_LENGTH 1000

typedef struct circ_buff{
	uint64_t read;
	uint64_t write;
	char characters[BUFF_LENGTH];
} circ_buff_t;

circ_buff_t buff;

void buffer_setup(){
	buff.read = 0;
	buff.write = 0;
}

void write_to_buff(uint64_t ch){
	//writes at write index

	buff.characters[buff.write] = ch;
	if((buff.write + 1)	% BUFF_LENGTH == 0) {
    if (buff.write == buff.read) { 
      //if true then we're over writing unread buffer items and will move read index with us
			buff.write = 0;
      buff.read = 0;
    } else {
      buff.write = 0;
    }
	} else {
    if (buff.write == buff.read) { 
      //if true then we're over writing unread buffer items and will move read index with us
			buff.write++;
      buff.read++;
    } else {
			buff.write++;
    }
	}

}

char read_from_buff(){
	//reads character at read index
  
  //if nothing new to read then return -1
  if (buff.write == buff.read) {
    return -1;
  }

	uint64_t output = buff.characters[buff.read];
	if((buff.read + 1) % BUFF_LENGTH == 0) {
		buff.read = 0;
	} else {
		buff.read++;
	}
	return output;
}

char kgetc(){
  //call read_from_buff until it returns anything but -1

  char output;
  while ((output = read_from_buff()) == -1);
  return output;
}

size_t kgets(char* output, size_t capacity){
	//call kgetc until either a '\n' is found or we've reached capacity
	//ensure that a '\0' is at the end of every array
	
	char new_input;
	size_t char_count;
	
	//if we aren't at capacity and haven't reached an '\n' char, keep calling kgetc() and storing the output
	for (char_count = 0; char_count < (capacity - 1) && new_input != '\n'; char_count++){
		output[char_count] = (new_input = kgetc());
	}//this block might be to hard to read

	//adding new line char at the end of string
	output[char_count+1] = '\0';

	//return amount of characters read
	return char_count;
}
