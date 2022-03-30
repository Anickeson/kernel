#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

#include "stivale2.h"
#include "kprint.h"
#include "interupt_handling.h"
#include "handlers.h"
#include "pic.h"
#include "port.h"
#include "circ_buff.h"
#include "paging.h"
#include "syscalls.h"

#define SYS_read 0
#define SYS_write 1
