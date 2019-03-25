#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lower_id = 1;
int upper_id;       // set to n in main, subtract for each internal node
int N = 7;              
int alphabet_length = 4;


typedef struct Node {
    int id;
    int depth;          // how deep we are into the string "banana$". 2 = the first 'n'.
    int label_length;   // how much further we go, >= 1. 
                        // 2 = the second 'a'. Thus label = "na";
    struct Node** children;
    struct Node* parent;
    struct Node* SL;
    char* label;
} node;

typedef struct Tree {
    struct Node* root;
    struct Node* u;
} tree;

node* createNode() {
    node* n = (node*) malloc(sizeof(struct Node));
    return n;
}

tree* init() {
    tree* t = (tree*) malloc(sizeof(struct Tree));

    node* r = createNode();
    r->children = NULL;
    r->parent = NULL;
    r->depth = 0;
    r->id = 40;
    r->label = NULL;

    r->parent = r;
    r->SL = r;

    
    t->root = r;
    return t;
}

void findPath(node* v, char* S, int offset) {
    if (!v->children) {
        node* temp = createNode();
        temp->id = lower_id;
        temp->children = NULL;
        temp->label_length = N - offset;        // not a guarantee, or maybe it is
        temp->depth = v->depth + temp->label_length;
        temp->label = "banana$";
        temp->SL = NULL;
        temp->parent = v;

        v->children = (node**) malloc(4 * sizeof(struct Node));
        v->children[0] = temp;
    }
}

int main() {

    tree* t = init();
    printf("t root id: %d\n", t->root->id);

    //t->root->parent = t->root;

    printf("t parent root id: %d\n", t->root->parent->id);
    printf("t parent SL id: %d\n", t->root->SL->id);


    return 0;
}