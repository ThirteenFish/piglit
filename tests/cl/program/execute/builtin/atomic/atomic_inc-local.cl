/*!
[config]
name: atomic_inc local
clc_version_min: 11

[test]
name: simple int
kernel_name: simple_int
dimensions: 1
global_size: 1 0 0
local_size: 1 0 0
arg_out: 0 buffer int[2] -2 -1
arg_in:  1 buffer int[1] NULL
arg_in:  2 int           -2

[test]
name: simple uint
kernel_name: simple_uint
dimensions: 1
global_size: 1 0 0
local_size:  1 0 0
arg_out: 0 buffer uint[2] 2 3
arg_in:  1 buffer uint[1] NULL
arg_in:  2 uint           2

[test]
name: threads
kernel_name: threads_int
dimensions: 1
global_size: 8 0 0
local_size:  8 0 0
arg_out: 0 buffer int[1] 8
arg_in:  1 buffer int[1] NULL

[test]
name: threads
kernel_name: threads_uint
dimensions: 1
global_size: 8 0 0
local_size:  8 0 0
arg_out: 0 buffer uint[1] 8
arg_in:  1 buffer uint[1] NULL

!*/

#define SIMPLE_TEST(TYPE) \
kernel void simple_##TYPE(global TYPE *out, local TYPE *mem, TYPE initial) { \
	*mem = initial; \
	TYPE a = atomic_inc(mem); \
	out[0] = a; \
	out[1] = *mem; \
}

#define THREADS_TEST(TYPE) \
kernel void threads_##TYPE(global TYPE *out, local TYPE *mem) { \
	*mem = 0; \
	barrier(CLK_LOCAL_MEM_FENCE); \
	atomic_inc(mem); \
	barrier(CLK_LOCAL_MEM_FENCE); \
	*out = *mem; \
}

SIMPLE_TEST(int)
SIMPLE_TEST(uint)

THREADS_TEST(int)
THREADS_TEST(uint)
