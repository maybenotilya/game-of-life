#include "io.h"

#include <stdio.h>
#include <stdlib.h>  // for EXIT_FAILURE



void print_matrix(char** matrix, int width, int height)
{
    int i,j;
    for(i=1;i<height+1;i++){
	for(j=1;j<width+1;j++){//make seg fault
	    printf("%c", matrix[i][j] ? 'x' : ' ');
	}
	printf("\n");
    }

}


//************* file matrix io ********************
void save_matrix_file(const char *filename,char **matrix, int width, int height)
{
    int i,j;
    FILE* output=fopen(filename,"w");
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


void save_vector(char* file_name, long vec[], int n){
    FILE* output=fopen(file_name,"w");
    if(output==NULL){
	perror("fopen failed:");
	exit(EXIT_FAILURE);
    }

    int i;
    for(i=0; i<n; i++){
	    int test=fprintf(output,"%d\n", vec[i]);
	    if(test==0 ||test==EOF){
		perror("scanf failed,corrupte file?");
		exit(EXIT_FAILURE);
	    }	
    }
    fclose(output);
}
