## 1. General Outline of a Proposal for this Project
---
The above project is to implement the cache simulator program using C++.
The Cache simulator outputs a hit rate for a given memory trace and cache setting (cache overall size, block size, association).


## 2. Compiling and Execution Environment
---
Ubuntu Linux 16.04 LTS 64-bit version was compiled and tested using g++
The program outputs and terminates error messages when:
1 If the program does not have four input factors
2 If you entered a trace file that does not exist
3 Cache block size is not a factor of 2.
4 If the Associate is entered with a value other than 1, 2, 4, and 8.
5 Cache as a whole is not a multiple of (cache block size)x (sociativity)


## 3. Program Input
---
he Cache simulator shall act as follows:
If the executable file in the Cache simulator is named simple_cache_sym (in Windows, simple_cache_sym.exe), 
it must work for the following input in the Windows command window or in the terminal window on Linux:

$ simple_cache_sim gzip.trace 1024 16 4

As above, the cache simulator requires four inputs. Each input is as follows.
1 memory trace file
2 Total Byte Size of Cache
3 cache block size of one byte
4 The association(i.e. for Direct mapped cache, is 1 and the 4-way set association cache is 4. Enter the N value in the N-way associative cache in general)
//For more than 1 association block placement was implemented using the Least Recently Used method.


## 4. memory trace file
---
The memory trace file is given as the first input of the cache simulator. 
The trace file contains data memory accesses that occurred while performing a program. 
The command fetch is not included. 
Each line in the trace file means one memory access. 
Each line is divided into three fields, the second of which means the 32-bit byte address when accessing memory, expressed in hexadecimal in the trace file.
For example, 0xff32e100 means memory address 4281524480. Each field is divided into one space.


## 5. Program Output
---
The Cache simulator is printed in the following format:
The 1st-3rd outputs represent the length of the field corresponding to tag, index, and offset for a given cache setting as input. 
The fourth output is the result of the cache simulation, which outputs three information: total number of memory access, hit count, and hit rate. 
The total number of memory accesses represents the number of memory accesses in the trace file, which is the same as the number of lines in the trace file. 
The Hit rate can be calculated from the total number of memory accesses and the hit count.
