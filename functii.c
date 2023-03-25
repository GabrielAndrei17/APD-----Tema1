#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "tema1.h"

void AddToList(Node *l, int n){
    
    if(l->val == -1){
        l->val = n;
        return;
    }
    
    Node* aux = l;
    while(aux->next != NULL){
        //add only unic elements
        if(aux->val == n)
            return;
        aux = aux->next;
    }
    if(aux->val == n)
        return;

    aux->next = (Node*)calloc(1, sizeof(Node));
    aux->next->val = n;
    aux->next->next = NULL;
}

void printList(Node* n)
{
    while (n != NULL) {

        printf(" %d ", n->val);
        n = n->next;
    }
}

void mergeLists(Node *l1, Node *l2){
    
    while (l2 != NULL) {

        if(l2->val != -1)
            AddToList(l1, l2->val);
        l2 = l2->next;
    }
}

int NList(Node *n){

    if(n->val == -1)
        return 0;

    int nr = 0;
    while (n != NULL) {

        nr++;
        n = n->next;
    }
    return nr;
}

char * toArray(int number){
    int n = log10(number) + 1;
    if(number == 0)
        n = 1;
    int i;
    char *numberArray = calloc(n, sizeof(char));
     for (i = n-1; i >= 0; --i, number /= 10)
     {
        numberArray[i] = (number % 10) + '0';
     }
    return numberArray;
}

Node ***MatrixList(int N_red, int N_map){
    
    Node ***list = (Node***) calloc(N_map + 1, sizeof(Node**));
    for(int i = 0; i < N_map; i++){
    
        list[i] = (Node**) calloc(N_red + 1, sizeof(Node*));
        for(int j = 0; j< N_red; j++){
    
            list[i][j] = (Node*)calloc(1, sizeof(Node));
            list[i][j]->val = -1; //empty
            list[i][j]->next = NULL;
        }
    }
    return list;
}