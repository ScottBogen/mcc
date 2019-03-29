#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Tree {
    struct Node* root;
    struct Node* u;
} tree;

int n;

typedef struct Node {
    int id;
    int depth; // 1-indexed. for "banana", 1 = "b", 2 = "a", etc.  

    struct Node** children;
    struct Node* parent;
    struct Node* SL;

    char* label;

} node;

tree* init() {
    tree* t = (tree*) malloc(sizeof(struct Tree));

    node* r = createNode();
    r->children = NULL;
    r->parent = r;
    r->depth = 0;
    r->id = 0;
    r->label = NULL;
    r->SL = r;
    r->parent = r;
    r->SL = r;

    t->root = r;
    t->u = r;
    return t;
}

node* createNode() {
    node* n = (node*) malloc(sizeof(struct Node));
    return n;
}

/* 
    Assumptions:
        - Findpath will only be called on the parent on the node(s) to be created.
        - Findpath has two cases:
            - Case 1: It breaks halfway on the edge. In this case, we create 2 nodes. The parent may become u.
            - Case 2: It breaks on the first index (no children can fit it). In this case, we create 1 leaf node. The parent may become u.


    for all children "child" of n,
        if S[offset] matches with child->label[0];
            if yes, this is the path we walk.

            

            matches = 0;
            while (S[offset] == child->label[matches]) {

            }

        if no, move onto the next child

    if no children match, then we have to create a new child.

    }

*/

int main() {

    char* S = "banana$";
    n = 7;

    tree* t = init();


    return 0;
}