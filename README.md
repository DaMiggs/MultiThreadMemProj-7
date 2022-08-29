# MultiThreadMemProj-7
A multi-threaded program that uses low-level I/O to reverse a given ﬁle (presumably w/ very large size) and, to accomodate customizable memory size.
## Program CLA's
- ***-input yyy.txt:*** where yyy is the path and name of the txt ﬁle that we want to reverse
 its content. **(DONE)**

- ***-nthreads xx:*** where xx is represents the number of threads and can be a positive
 integer from 1 to 90. The number of threads cannot be greater than half of the input
 ﬁle size (in bytes). The default value is 2. **(DONE)**

- ***-mem zz:*** where zz is the maximum total number of bytes that the program is allowed
 to allocate from heap (using malloc function). This argument is optional and if it does
 not exist, it means that there is no memory limitation.
  - **(This CLA IS implemented, there are still has some occurance's where the file pointer overlaps or does not cover the entire file, 
				leaving parts untouched or writing a NULL char to file.	The memory usage however is limited to specified size and all threads run concurrently/parallelly with limit.
				I belive this mainly has to do with the 'remainder' variable in the for loop in main and needs to be changed to accomodate the mem limit.)**
