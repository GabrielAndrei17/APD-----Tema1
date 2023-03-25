#ifndef TEMA1_H
#define TEMA1_H

//The node of a list
typedef struct node_t{
    int val;
    struct node_t *next;
}Node;
//the list is empty if it has val=-1

//argument of mapping thread
struct map_argument {
    Node** l; //output
    char** file_names; //input
    int i; //file id
};

//argument of reducing thread
struct red_argument {
    Node*** l; //input
    int e; //Exponent - 2
};

//Add an element to list l if it dose't exist
void AddToList(Node *l, int n);

//Returns number of the elements from list
int NList(Node *n);

//Returns the number converted to an char array
char *toArray(int number);

//merge 2 lists
void mergeLists(Node *l1, Node *l2);

void printList(Node* n);

//Returns a matrix of lists with the input dimensions
Node ***MatrixList(int N_red, int N_map);

#endif