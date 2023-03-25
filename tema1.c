#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "tema1.h"

int N, N_map, N_red;
int files_done;
pthread_mutex_t m1, map_finished;
pthread_barrier_t b1;

//Verify if n can be written as r^E
//returns n if true else returns 0
int PerfPower(int n, int E){
    if(n <= 0 )
        return 0;

    //Binary search
    int st = 1, dr = n;
    while(st <= dr){
        int mid = (dr-st)/2 + st;
        int p = pow(mid, E);
        if(p == n)
            return n;
        
        if(p < n && p > 0)
            st = mid + 1;
        else
            dr = mid - 1;
    }
    return 0;
}

char* get_args(int argc, char **argv){
	if(argc < 4) {
		printf("Numar insuficient de parametri: ./tema1 N_map N_red nume_fisier\n");
		exit(1);
	}

	N_map = atoi(argv[1]);
	N_red = atoi(argv[2]);
    return argv[3];
}

//Debug function: print the matrix
void print(Node ***l)
{
    printf("N = %d\n", N);
    printf("N_red = %d\n", N_red);
    printf("N_map = %d\n", N_map);

    for(int i = 0; i < N_map; i++){
        printf("%d:\n", i);
        for(int j = 0; j < N_red; j++){
            printf("%d: ", j);
            printList(l[i][j]);
            printf("\n");
        }
        printf("\n");
    }
}

void* reduce_function(void* argument){
    struct red_argument* reds = (struct red_argument*)argument;

    //I will wait for the mapping threads to finish
    pthread_mutex_lock(&map_finished);
    pthread_mutex_unlock(&map_finished);

    Node ***result = reds->l;
    int e = reds->e;

    for(int i = 1; i < N_map; i++){

        mergeLists(result[0][e], result[i][e]);
    }

    int res = NList(result[0][e]);

    //Write the result in output file
    char *file_n = (char*) calloc(7 + e, sizeof(char));
    strcat(file_n, "out");
    char* auxS = toArray(e + 2);
    strcat(file_n, auxS);
    free(auxS);
    strcat(file_n, ".txt");

    FILE *fptr;
    fptr = fopen(file_n,"w");

    if(fptr == NULL)
    {
       printf("Error!");   
       exit(1);
    }

    fprintf(fptr,"%d", res);
    fclose(fptr);
    free(file_n);

    return NULL;
}

void* map_function(void* argument){
    
    char * line = NULL;
    int len = 0;

    struct map_argument* maps = (struct map_argument*)argument;
    
    //The case when we have more mappers than files to process
    if(maps->i >= N){
        pthread_barrier_wait(&b1);
        pthread_mutex_unlock(&map_finished);
        return NULL;

    }

    //The thread is active until there are no files to procces;
    while(1){

        FILE* f1 = fopen(maps->file_names[maps->i], "rt");
        if (f1 == NULL)
	    {
	        printf("nu exista fisierul %s\n", maps->file_names[maps->i]);
	        return NULL;
	    }
        getline(&line, &len, f1);
        int M = atoi(line);

        for(int j = 0; j < M; j++){
           getline(&line, &len, f1);
           int el = atoi(line);

           // verify if the element is a perfect power for any k
           for(int k = 0; k < N_red; k++){
               int res = PerfPower(el, k + 2);
               if(res > 0){
                    AddToList(maps->l[k], res);
               }
            }
        }
        fclose(f1);

        pthread_mutex_lock(&m1);

        if(files_done == N){

            pthread_mutex_unlock(&m1);
            free(maps);
            pthread_barrier_wait(&b1);
            pthread_mutex_unlock(&map_finished);
            return NULL;
        }

        //if there are files left I take the next one which is not proccesed or in processing
        maps->i = files_done;
        files_done++;
        pthread_mutex_unlock(&m1);
    }
}

int main(int argc, char *argv[])
{
    char * line = NULL;
	int len, r;
    void *status, *status2;

    FILE* f = fopen(get_args(argc, argv), "rt");

    if (f == NULL)
	{
		printf("nu exista fisierul %s\n", get_args(argc, argv));
		return -1;
	}

    getline(&line, &len, f);
    N = atoi( line);

    char **arr = calloc(N, sizeof(char*));
    Node ***HashMaps = MatrixList(N_red, N_map);

    pthread_mutex_init(&m1, NULL);
    pthread_mutex_init(&map_finished, NULL);
    pthread_barrier_init(&b1, NULL, N_map);

    pthread_t threads[N_map];
    pthread_t threads_red[N_red];

    //read test file
    for(int i = 0; i < N; i++) {

        getline(&line, &len, f);
        if(line[strlen(line) - 1] == '\n'){
            line[strlen(line) - 1] = '\0';
        }
        arr[i] = calloc(strlen(line), sizeof(char));
        strcpy(arr[i], line);
    }

    fclose(f);

    //start the threads for mapping
    files_done = N_map;
    pthread_mutex_lock(&map_finished); //block the reducers to start before mappers are finished
    for(int i = 0; i < N_map; i++){
        struct map_argument* aux1 = (struct map_argument*)calloc(1, sizeof(struct map_argument));
        aux1->file_names = arr;

        aux1->l = HashMaps[i];
        aux1->i = i;

        r = pthread_create(&threads[i], NULL, map_function, aux1);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
    }

    //start the threads for reducing
    //at the same time with the mapping threads
    for(int i = 0; i < N_red; i++){
        struct red_argument* aux2 = (struct red_argument*)calloc(1, sizeof(struct red_argument));
        aux2->l = HashMaps;
        aux2->e = i;

        r = pthread_create(&threads_red[i], NULL, reduce_function, aux2);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}

    }

    //Join all threads to join in main function
    for(int i = 0; i < N_map; i++){

        r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
    }
    for(int i = 0; i < N_red; i++){

        r = pthread_join(threads_red[i], &status2);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
    }

    //print(HashMaps);
    pthread_mutex_destroy(&m1);
    pthread_mutex_destroy(&map_finished);
    pthread_barrier_destroy(&b1);

	return 0;
}
