#pragma once

/*
 * Read and write will take inputs that specify where the action is taking place, like stdin stdout stderr.
 * Those are represented as 0 for stdin, 1 for stdout, 2 for stderr.
 * Read only needs to work with 0 for now. Write only needs to be concerned with 1 and 2.
 */


int64_t read(int64_t fd, char * buffer, int64_t bytes);

int64_t write(int64_t fd, char * buffer, int64_t bytes);

extern int64_t syscall(uint64_t nr, ...);
extern void syscall_entry();

