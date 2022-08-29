#include "pa7.h"

int fd, buf_size;
long file_size;
int numThreads = 2;
int mFlag = 0;
char* filename;
int mem_size = 0;

typedef struct{					//added 2 new variables, og_buf and real_buf
	char* left_buf;
	char* right_buf;
	off_t left_pos;
	int len;
	int fig_buf;
	int real_buf;

} thread_info;
pthread_mutex_t file_mutex;
pthread_mutex_t buf_mutex;


void reverse_str(char* str){
	int len = strlen(str);
	int half = len >> 1;
	for(int i = 0; i < half;i++){
		char c = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = c;
	}
}
void* reverse(void* args){
    thread_info info = *((thread_info*)args);

    pthread_mutex_lock(&file_mutex);
    lseek(fd, info.left_pos,SEEK_SET);
    read(fd, info.left_buf, info.len);
    pthread_mutex_unlock(&file_mutex);

    info.left_buf[info.len] = '\0';

    pthread_mutex_lock(&file_mutex);
    lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);
    read(fd, info.right_buf, info.len);
    pthread_mutex_unlock(&file_mutex);

    info.right_buf[info.len] = '\0';
    reverse_str(info.left_buf);
    reverse_str(info.right_buf);

    pthread_mutex_lock(&file_mutex);
    lseek(fd, info.left_pos,SEEK_SET);
    write(fd, info.right_buf, info.len);
    lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);
    write(fd, info.left_buf, info.len);
    pthread_mutex_unlock(&file_mutex);
}

void* memReverse(void* args){// need to remeber to update cursor to new location
	char loop = 1;
	thread_info info = *((thread_info*)args);

	// printf("TPS: %d\n", info.fig_buf);
	// printf("POS: %d\n", info.left_pos);
	// printf("FLN: %d\n", info.len);

	pthread_mutex_lock(&file_mutex);
	lseek(fd, info.left_pos,SEEK_SET);//buf_size to traverse past//partiotion of file is left_pos
	read(fd, info.left_buf, info.len);
	pthread_mutex_unlock(&file_mutex);
	
	info.left_buf[info.len] = '\0';
	
	pthread_mutex_lock(&file_mutex);
	lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);//seeking right buffer bytes
	read(fd, info.right_buf, info.len);//reading into right buf
	pthread_mutex_unlock(&file_mutex);
	

	info.right_buf[info.len] = '\0';
	// printf(".len = %d\n", info.len);
	// printf("MALLOC: %d\n", malloc_size(info.left_buf));
	//reversing chars in the buf
	reverse_str(info.left_buf);
	reverse_str(info.right_buf);
	
	pthread_mutex_lock(&file_mutex);
	lseek(fd, info.left_pos,SEEK_SET);
	write(fd, info.right_buf, info.len);
	lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);
	write(fd, info.left_buf, info.len);
	pthread_mutex_unlock(&file_mutex);

	info.fig_buf -= (info.real_buf-1);		//removed -1 : real_buf-1
	info.left_pos += info.len;
	//need a starting lseek and read foir left and right, then SEEK_CUR inside the while loop
	// use the buffer size  to associate with the new section of the file that will be worked on
	// also needs to find a way to reverse the remainder of the partitioned file

	while(loop) {

		// printf("POS: %d\n", info.left_pos);
		// printf("LEN: %d\n", info.len);
		//Do I need to change or update info.len?

		pthread_mutex_lock(&file_mutex);
		lseek(fd, info.left_pos,SEEK_SET);//buf_size to traverse past. //left_pos += len after each run to move to new space 
		read(fd, info.left_buf, info.len);
		pthread_mutex_unlock(&file_mutex);

		info.left_buf[info.len] = '\0';
		
		pthread_mutex_lock(&file_mutex);
		lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);//seeking right buffer bytes//because the system reads from left to right you have to use + .len to get curs infront of string
		read(fd, info.right_buf, info.len);//reading into right buf
		pthread_mutex_unlock(&file_mutex);
		
		info.right_buf[info.len] = '\0';
		//reversing chars in the buf
		reverse_str(info.left_buf);
		reverse_str(info.right_buf);
		
		pthread_mutex_lock(&file_mutex);
		lseek(fd, info.left_pos,SEEK_SET);
		write(fd, info.right_buf, info.len);
		lseek(fd, (off_t)(-(info.left_pos + info.len)),SEEK_END);//need to better understand, typecast off_t for offset, then move cursor infront of string to copy by adding .len
		write(fd, info.left_buf, info.len);
		pthread_mutex_unlock(&file_mutex);
		info.left_pos+=info.len;
		if (!info.fig_buf){
			//printf("Breaking Thread Function\n");
			loop = 0;
		}else if (info.fig_buf <= info.len){
			info.len = info.fig_buf;
			info.fig_buf-=info.fig_buf;
		}else {
			info.fig_buf-=info.len;
		}
		



		// if (info.fig_buf >= info.real_buf)
		// 	info.fig_buf -= (info.real_buf-1);			//removed -1 : real_buf-1
		// printf("fig_buf: %d\n", info.fig_buf);
		
		// if (info.fig_buf == 0){
		// 	printf("Breaking Thread Function\n");
		// 	break;
		// 	//exit(1);
		// }
		// if (info.fig_buf < info.real_buf){
		// 	info.len = info.fig_buf;//same as remainder idea
		// 	info.fig_buf-=info.fig_buf;// zero out fig_buf
		// }
	}
}


int main(int argc, char** argv){
		
	pthread_mutex_init(&file_mutex, NULL);

	//Identifying CLAs
	for (int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-input")){
			filename = (char*)malloc(strlen(argv[++i])+1);
			strcpy(filename, argv[i]);
			printf("%s\n", filename);
		}if (!strcmp(argv[i],"-nthreads")){
			numThreads = atoi(argv[++i]);
			if (numThreads < 1 || numThreads > 90){
				printf(stderr, "FATAL: THREAD COUNT IS BETWEEN 1 AND 90\n");
				return -1;//change🚨
			}
		}if(!strcmp(argv[i], "-mem")){
			mFlag = 1;
			mem_size = atoi(argv[++i]);		
		}
	}
	//reverseMe(filename);
	///Users/miguel/Desktop/input.txt

	fd = open(filename, O_RDWR);
	char c;
	int n_read;
	thread_info args[numThreads];
	file_size = lseek(fd, 0, SEEK_END);//find the file size
	// printf("FILE SIZE: %d\n", file_size);
	// file_size = ;
	//printf("FileBoy: %d\nFile_Size:%d\n", fileBoy(filename), file_size);
	
	
//	if(file_size%2){
//		fprintf(stderr, "Error: file size is odd\n");
//		exit(1);
//	}

	// for (int i = 0; i < THREADS; i++)
	// {
	// 	pthread_create
	// }
	
	long remainder = file_size >> 1;						//setting remainder earlier in code...makes remainder equal to half of total file size
														//this is what we need to work on if the size is bigger
														//then the mem max
														
														//iff buf size is greater than memory limit we have to 
														//-mem 20000 ✅
														//-mem 2000,  file size = 5000, threadCount = 10
														//Equation for adjusted buf_size:
														//(int)(ceil(mem_size/(2.0*THREADS)) + 1)
	if (mem_size == 0) {
		buf_size = (int)(ceil(file_size/(2.0*numThreads)) + 1);//regular buf_size AKA og_Buf
		// printf("FBuff Size is : %d\n", buf_size);
	} else {
		buf_size = (int)(ceil(mem_size/(2.0*numThreads)) + 1);//memory optimized buf_size Ex:2000/20 = 100 bytes
		// printf("MBuff Size is : %d\n", buf_size);
	}

	// sleep(4);
	char* left_buf[numThreads];
	char* right_buf[numThreads];
	pthread_t thread[numThreads];
	
	if (mFlag && (mem_size < file_size)) {//runs if mem_size is set and is smaller than file_size and if mFlag is set
		long remainder = file_size >> 1;
		for(int i = 0; i < numThreads;i++){
			//makes space for left and right buffer
			left_buf[i] = (char*) malloc(buf_size);
			right_buf[i] = (char*) malloc(buf_size);
			//fill data structure with attr
			args[i].fig_buf = (int)(ceil(file_size/(2.0*numThreads)) + 1);
			args[i].real_buf = buf_size;

			args[i].left_buf = left_buf[i];
			args[i].right_buf = right_buf[i];
			args[i].left_pos = i*(args[i].fig_buf-1);//should this be buf_size or fig_buf
			args[i].len = (i != numThreads-1)?(buf_size-1):remainder;//len is the length of string in data bytes that are to be worked on with the thread.
																	 //updating outside of function prevents the loop inside from running correctly
															
			remainder -= (buf_size-1);						//adjusts remainder
			//sleep(1);
			pthread_create(thread + i, NULL, memReverse, (void *)(args+i));
			printf("Thread %d created: \n", i);
		}
	}else {//og method
		
		for(int i = 0; i < numThreads;i++){
			//makes space for left and right buffer
			left_buf[i] = (char*) malloc(buf_size);			
			right_buf[i] = (char*) malloc(buf_size);		//its a pointer so we pass along the address we're working with
			
			//fill data structure with attr
			args[i].left_buf = left_buf[i];
			args[i].right_buf = right_buf[i];
			args[i].left_pos = i*(buf_size-1);
															//why * i? size * 2 = double size, --this is position of cursor, 
															//this needs to be spaced out by n amount per thread to not overlap
															//basically moving address spot over by i and multiply it by bufsize to
															//seperate it enough from the last address by a valid amount
															//because this is working with the memory size directly
															
			args[i].len = (i != numThreads-1)?	(buf_size-1):remainder;
															//buf_size is half of the memory used by the thread,
															//when i == THREADS-1 we SHOULD only have the endings strings left in remainder
															
			remainder -= (buf_size-1);//empties remainder
			pthread_create(thread + i, NULL, reverse, (void *) (args+i));
			printf("Thread %d created: \n", i);
		}
	}

	
	for(int i = 0; i < numThreads;i++){
		pthread_join(thread[i], NULL);
		printf("Thread %d Joined!\n", i);
	}

}
