#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

char ** allocate_matrix(int width,int height);

// Struct variables to measure execution time
struct timeval t_start, t_end;
void print_matrix1(char **matrix, int width, int height)
{

    int i,j;
     
    for(i=1;i<height+1;i++){
	for(j=1;j<width+1;j++){//make seg fault
  
	    printf("%c", matrix[i][j] ? 'x' : ' ');
	}

	printf("\n");

    }
    printf("----------------------------");
    printf("Board's state in steps-old");
    printf("---------------------------\n");

}
void update_matrix(char *** in_matrix,int width, int height, int steps)
{
    int i,j,k;

    char ** new_matrix=allocate_matrix(width,height);
    char ** pmatrix=*in_matrix;

    for (k=0;k<steps;k++){
	for(i=1;i<height+1;i++){
	    for(j=1;j<width+1;j++){
		int sum=pmatrix[i-1][j-1]+pmatrix[i-1][j]+pmatrix[i-1][j+1]+pmatrix[i][j-1]+ 
		    pmatrix[i][j+1]+ pmatrix[i+1][j-1]+ pmatrix[i+1][j]+pmatrix[i+1][j+1];
	        if(pmatrix[i][j]==0 && sum==3){
		    new_matrix[i][j]=1;
		}else{
		    if(sum<2 ||sum>3)
			new_matrix[i][j]=0;
		    else new_matrix[i][j]=pmatrix[i][j];
		}
	    }
	}
    char ** tmp=pmatrix;
    pmatrix=new_matrix;
    new_matrix=tmp;
    //print_matrix1(pmatrix,width,height);
    }
    (*in_matrix)=pmatrix;
}

char ** allocate_matrix(int width,int height)
{
    char ** matrix;
    int i;
    matrix=(char**)calloc(height+2,sizeof(char*));
    if(matrix== NULL){
	perror("allocation error:");
	exit(EXIT_FAILURE);
    }
    for(i=0;i<height+2;i++){;
	matrix[i]=(char*)calloc(width+2,sizeof(char));
	if(matrix[i]==NULL){
	    perror("allocate error:");
	    exit(EXIT_FAILURE);
	}
    }
   return matrix;
}


void read_matrix(const char* filename, char ***matrix, int *width, int *height)
{
    FILE * input=NULL;
    int i,j;

    input=fopen(filename,"r+");
    if(input==NULL){
	perror("fopen error:");
	exit(EXIT_FAILURE);
    }
    fscanf(input,"width: %d\n",width);
    fscanf(input,"height: %d\n",height);


    (*matrix)=allocate_matrix(*width,*height);
    for(i=1;i<*height+1;i++){
	for(j=1;j<*width+1;j++){
	    int val;
	    int test=fscanf(input,"%d",&val);
	    if(test==0 || test==EOF){
		perror("scanf failure,why?");
		exit(EXIT_FAILURE);
	    }
	    (*matrix)[i][j]=val;
	   
	}
    }


    fclose(input);

}

void print_matrix(const char *filename,char **matrix, int width, int height)
{
    FILE *output =NULL;
    int i,j;
    output=fopen(filename,"w");
    if(output==NULL){
	perror("fopen failed:");
	exit(EXIT_FAILURE);
    }
    fprintf(output,"width:%d\n",width);
    fprintf(output,"height:%d\n",height);
    for(i=1;i<height+1;i++){
	for(j=1;j<width+1;j++){
	    int test=fprintf(output,"%d",matrix[i][j]);
	    if(test==0 ||test==EOF){
		perror("scanf failed,corrupte file?");
		exit(EXIT_FAILURE);
	    }
	}
	fprintf(output,"\n");
    }
    fclose(output);
}


int main(int argc, char *argv[])
{
    int width =0,height=0;
    char ** matrix;
    int steps;
   
    if(argc!=4){
	fprintf(stderr,"USAGE:%S<INPUT_FILE> <NUM STEPS> <OUTPUT FILE>/n",argv[0]);
	exit(EXIT_FAILURE);
    }
    sscanf(argv[2],"%d", &steps);
  
    read_matrix(argv[1],&matrix,&width,&height);
   
    printf("serial\n");
    gettimeofday(&t_start, NULL);
    
    update_matrix(&matrix,width,height,steps);
  
    gettimeofday(&t_end, NULL);
    printf("%ld\n\n", ((t_end.tv_sec * 1000000 + t_end.tv_usec) - (t_start.tv_sec * 1000000 + t_start.tv_usec)));
    print_matrix(argv[3],matrix,width,height);
 
    return EXIT_SUCCESS;
}

