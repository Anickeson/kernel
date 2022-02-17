#include <stdint.h>
#include <stddef.h>

#include "circ_buff.h"

#define BUFF_LENGTH 1000

//making all of the circular buffer global within this file //maybe shouldn't be global??
uint64_t characters[BUFF_LENGTH];
uint64_t buff_write = 0;
uint64_t buff_read = 0;

void write_to_buff(uint64_t ch){
	//writes at write index

	characters[buff_write] = ch;
	if((buff_write + 1)	% BUFF_LENGTH == 0) {
    if (buff_write == buff_read) { 
      //if true then we're over writing unread buffer items and will move read index with us
			buff_write = 0;
      buff_read = 0;
    } else {
      buff_write = 0;
    }
	} else {
    if (buff_write == buff_read) { 
      //if true then we're over writing unread buffer items and will move read index with us
			buff_write++;
      buff_read++;
    } else {
			buff_write++;
    }
	}

}

int read_from_buff(){
	//reads character at read index
  
  //if nothing new to read then return -1
  if (buff_write == buff_read) {
    return -1;
  }

	uint64_t output = characters[buff_read];
	if((buff_read + 1) % BUFF_LENGTH == 0) {
		buff_read = 0;
	} else {
		buff_read++;
	}
	return output;
}

/**
 * Read one character from the keyboard buffer. If the keyboard buffer is empty this function will block until a key is pressed.
 *
 * \returns the next character input from the keyboard
 */
char kgetc(){
  //call read_from_buff until it returns anything but -1

  char output;
  while ((output = read_from_buff()) == -1);
  return output;
}

