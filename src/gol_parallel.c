#include <stdio.h>   // for printf, fscanf, scanf
#include <stdlib.h>  // for malloc, calloc, EXIT_FAILURE

#include <pthread.h> //

#include "io.h"


unsigned int n_threads;
pthread_barrier_t barr;
int width =0,height=0, steps=0;
char** matrix_from;
char** matrix_to;
int* threads_arg;


//********** allocate (width+2) x (height+2) char matrix
char** allocate_matrix(int width,int height){
    int i;
    char** matrix=(char**)calloc(height+2,sizeof(char*));
    if(matrix== NULL){
	perror("allocation error:");
	exit(EXIT_FAILURE);
    }
    for(i=0;i<height+2;i++){
	matrix[i]=(char*)calloc(width+2,sizeof(char));
	if(matrix[i]==NULL){
	    perror("allocate error:");
	    exit(EXIT_FAILURE);
	}
    }
   return matrix;
}


//********* free memory for matrix
void free_matrix(char** matrix, int width, int height){
    int i=0;
    for(i=0;i<height+2;i++){
	free(matrix[i]);
    }
    free(matrix);
}


//******** read matrix from file and allocate memory to store it
char** dump_matrix_file(const char* filename){
    FILE * input=NULL;
    int i,j;

    input=fopen(filename,"r");
    if(input==NULL){
	perror("fopen error:");
	exit(EXIT_FAILURE);
    }
    fscanf(input,"width: %d\n", &width);
    fscanf(input,"height: %d\n", &height);
   
    char** init_matrix = allocate_matrix(width,height);

    for(i=1;i<height+1;i++){
	for(j=1;j<width+1;j++){	   
	    int val;
	    int test=fscanf(input,"%d",&val);
	    if(test==0 || test==EOF){
		perror("scanf failure,why?");
		exit(EXIT_FAILURE);
	    }
	    init_matrix[i][j]=val;
	   
	}
    }

    fclose(input);

    return init_matrix;
}

void copy_matrix(char** matrix_from, char** matrix_to, int width, int height){
    int i,j;
    for(i=0;i<height;i++){
	for(j=0;j<width;j++){	   
	    matrix_to[i][j] = matrix_from[i][j];
	}
    }
}



//********* update the begin_row to end_row of matrix (including end_row)
//--------- write matrix_to according to matrix_from and the rule of game of life 
void update_matrix(int begin_row, int end_row){
    int i,j;
    for(i=begin_row;i<end_row;i++){
	for(j=1;j<width+1;j++){
	    int sum = matrix_from[i-1][j-1]+matrix_from[i-1][j]+matrix_from[i-1][j+1]+matrix_from[i][j-1]+ 
		matrix_from[i][j+1]+ matrix_from[i+1][j-1]+ matrix_from[i+1][j]+matrix_from[i+1][j+1];
	    if(matrix_from[i][j]==0 && sum==3){
		matrix_to[i][j]=1;
	    }else{
		if(sum<2 ||sum>3)
		    matrix_to[i][j] = 0;	    
		else matrix_to[i][j] = matrix_from[i][j];
	    }
	}
    }
}


//*********** thread entry function **********
void* entry_function(void* p_i_thread){
    int i_thread=*( (int*)p_i_thread );
    char** temp;

    // Calculate the array bounds that each thread will process
    int bound = height / n_threads;
    int begin_row = i_thread * bound;
    int end_row = begin_row + bound;

    // exclude extern cells
    if(i_thread==0) begin_row++;
    if(i_thread==n_threads-1) end_row=height + 1;

    // Play the game for steps
    int i;
    for (i=0; i<steps; i++){	
	update_matrix(begin_row,end_row);

	//thread barrier
	int bn = pthread_barrier_wait(&barr); 
	if(bn == PTHREAD_BARRIER_SERIAL_THREAD){
	    temp=matrix_from;
	    matrix_from = matrix_to;
	    matrix_to = temp;
	  			 			
	}else if(bn != 0){
	    printf("Could not wait on barrier\n");
	    exit(-1);
	}
	 bn = pthread_barrier_wait(&barr); 
	 if(bn != 0 && bn != PTHREAD_BARRIER_SERIAL_THREAD ){
	     printf("Could not wait on barrier\n");
	     exit(-1);
	 }
    }

    return 0;
}


//********** return wall-clock time of updating the matix for 'steps' times by 'n_threads' threads
long walltime_of_threads(int n_threads){
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL); // ------start timer
   
    // Barrier initialization
    if(pthread_barrier_init(&barr, NULL, n_threads)){
	printf("Could not create a barrier\n");
	return -1;
    }
	
    // Create the threads
    pthread_t thr[n_threads];
    int i_thread=0;
    for(i_thread = 0; i_thread < n_threads; i_thread++){
	threads_arg[i_thread]=i_thread;
	if(pthread_create(&thr[i_thread], NULL, &entry_function, (void*)&threads_arg[i_thread])){
	    printf("Could not create thread %d\n", i_thread);
	    return -1;
	}
    }
    
    // Proccess's master thread waits for the execution of all threads
    //begin = clock();
    for(i_thread = 0; i_thread < n_threads; i_thread++){
	if(pthread_join(thr[i_thread], NULL))
	{
	    printf("Could not join thread %d\n", i_thread);
	    return -1;
	}
    }

    gettimeofday(&t_end, NULL);  // ---------stop timer
    
    return (t_end.tv_sec * 1e6 + t_end.tv_usec) - (t_start.tv_sec * 1e6 + t_start.tv_usec);   // in microseconds, only work for intervals shorter than ~2000 secs
}


int main(int argc, char* argv[])
{
    if(argc!=5){
	fprintf(stderr,"USAGE:\n\t%s <input file> <num steps> <output file> <max number of threads> \n", argv[0]);
	fprintf(stderr, "EXAMPLE:\n\t%s random.txt 500 result.txt 8\n", argv[0]);
	exit(EXIT_FAILURE);
    }

    char* init_fname = argv[1];
    sscanf(argv[2],"%d", &steps);
    char* out_fname = argv[3];
    int max_n_threads = atoi(argv[4]);

    long time_vs_threads[max_n_threads+1];
    int tmp[max_n_threads];
    threads_arg=tmp;

    char** init_matrix = dump_matrix_file(init_fname);
    char** zero_matrix = allocate_matrix(width, height);
    matrix_from = allocate_matrix(width, height);
    matrix_to = allocate_matrix(width, height);

    // run by different number of threads, and measure the time
    for(n_threads=1; n_threads <= max_n_threads; n_threads++){
	copy_matrix(init_matrix, matrix_from, width+2, height+2);
	copy_matrix(zero_matrix, matrix_to, width+2, height+2);
	
	time_vs_threads[n_threads] = walltime_of_threads(n_threads);
	printf("walltime when run by %d threads: %d microseconds\n", n_threads, time_vs_threads[n_threads]);
    }

    save_matrix_file(out_fname, matrix_from,width,height);

    char time_fname[20];
    sprintf(time_fname,"time-%d-%d.txt",width,height);
    save_vector(time_fname, time_vs_threads+1, max_n_threads);
   
    free_matrix(zero_matrix,width,height);
    free_matrix(init_matrix,width,height);
    free_matrix(matrix_from,width,height);
    free_matrix(matrix_to,width,height);
    return 0;
}
