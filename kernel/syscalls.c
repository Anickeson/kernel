#include "general.h"


// syscall invokes an interupt code of 0x80 so this will be all held in an interupt handler
// So the interupt handler will call the functions from this file

int64_t read(int64_t fd, char * buffer, int64_t bytes){
	char next_ch;
	int ind = 0; 
	//ind is separate from i in the for loop 
	//because if we recieve a backspace character we need to move the ind back
	//while not affecting the quantity of iterations within the loop
	
	if (fd == 0){

		//loop bytes times
		for (int i = 0; i < (bytes - 1); i++){
			next_ch = kgetc();

			if (next_ch == '\b' && ind != 0){
			//handling the backspace case, moving ind back one to overwrite the space as long as there is space
					ind--;
			} else {
			//base case is that we write the character to the buffer
				buffer[ind] = next_ch;
			}

			ind++;
		}
	} else {
		//we aren't handling anything other than reading from stdin right now
		return -1;
	}

	//adding null terminator at the end
	buffer[bytes-1] = '\0';
	
	//read characters from the fd into the buffer
	//I think if the fd is 0 we just use kgetc
	
	return ind;
}

int64_t write(int64_t fd, char * buffer, int64_t bytes){
	int ind = 0;
	//loop amount of times specified by bytes
	//take char from buffer[i] and write them to either stdout or stderr
	if (fd == 1 || fd == 2){
		for (; ind <= bytes; ind++) {
			write_to_buff(buffer[ind]);
		}
	} else {
		return -1;
	}
	return ind;
}
