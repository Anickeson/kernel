#include "general.h"


char buffer[BUFF_LENGTH]; 
volatile uint64_t write_count = 0; //index will only ever be incremented
volatile uint64_t read_ind = 0; //indicies will be modded on array access to make buff circular


void write_to_buff(char ch){

	int mod_write = write_count % BUFF_LENGTH;
	int mod_read = read_ind % BUFF_LENGTH;
	//write at modded write count	
	
	buffer[mod_write] = ch;
	
	//will need to increment read_ind when we're over writing 
	//increment will not be necessary when buffer is empty
	if (mod_write == mod_read && (write_count - read_ind) != 0) {
		read_ind++;
	}

	//increment write count after writing
	write_count++;

}

char read_from_buff(){


  //if nothing new to read then return -1
  if ((write_count - read_ind) <= 0){
    return -1;
  }

	char output = buffer[read_ind % BUFF_LENGTH]; 
	read_ind++;
	return output;
}

char kgetc(){
  //call read_from_buff until it returns anything but -1

  char output;
  while (1){
		output = read_from_buff();
		if (output != -1){
			break;
		}
	}
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
