/* 

    Layout:

    tree: 
        - root
        - diplay(node* u): display u's children from left to right
        - constructor: sets root node
        - enumerate(node* u): display u's children in DFS traversal (l to r after visiting parent)
        - BWT(char* s): enumerate leaf node's id's from left to right (lexicographically smallest to largest)
            - BWT index is an array B of size n, given by B[i] = s[leaf(i)-1],
              where leaf(i) is the suffix id of the ith leaf in the lexicographical order

    node: 
        - int id 
        - int depth             parent->depth + edge label length 
        - node** children
        - node* parent 
        - node* SL
        - char* label;

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NULL 0

// two globals for ID:

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


tree* init();    // create tree
node* createNode();

void insert(tree* t, char* S, int i);       // insert new node into tree from string S at offset i
                                            // ex: t, "banana$", "3" = insert("nana$")
void display(node* u);
void enumerate(node* u);
void BWT(char* s);

void findPath(node* v, char* S, int offset);

void nodeHops();


// creates tree, sets root to null, sets SL to self
tree* init() {
    tree* t = (tree*) malloc(sizeof(struct Tree));

    node* r = createNode();
    r->children = NULL;
    r->parent = NULL;
    r->depth = 0;
    r->id = 10;
    r->label = NULL;
    r->SL = r;
    r->parent = r;

    t->root = r;
    return t;
}

node* createNode() {
    node* n = (node*) malloc(sizeof(struct Node));
    n->children = NULL;
    n->depth = 0;
    n->id = 0;
    n->label = "";
    n->label_length = 0;
    n->parent = NULL;
    n->SL = NULL;
    return n;
}

void insert(tree* t, char* S, int i) {

    // make local pointers
    node* root = t->root;
    node* u = t->u;

    // check cases

    // cases 1-2: SL(u) is known
    if (u->SL != NULL) {
        // case 1: u is not the root
        if (u != root) {
            // take SL(u) to v
            node* v = u->SL;
            // findPath(v, s[i+alpha...])
            findPath(v, S, i);

        }
        // case 2: u is the root
        else {

        }
    }

    // cases 3-4: SL(u) is known
    else {
        // case 3: u' is not the root
        if (u->parent != root) {

        }

        // case 4: u' is the root
        else {
            node* u_prime = u->parent;  // should be root
            char* beta = u->label;
            node* v_prime = u_prime->label;
        }
    }
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

        v->children = (node**) malloc(alphabet_length * sizeof(struct Node));
        v->children[0] = temp;
    }
}

void printNodeInfo(node* temp) {
    printf("Node Info:\n");
    printf("\tid:\t\t%d\n", temp->id);
    //printf("\tparent id:\t%d\n", temp->parent->id);

    printf("\tparent?\t\t");
    if (temp->parent->id) { printf("id:%d\n", temp->parent->id); } else { printf("no\n"); }

    printf("\tchildren?\t");
    if (temp->children) { printf("yes\n"); } else { printf("no\n"); }

    printf("\tlabel length:\t%d\n", temp->label_length);
    printf("\tdepth:\t\t%d\n\n", temp->depth);
}

char* getLabel(node* n, char* S) {


}

int main(int argc, char** argv) {

    // call pre-process (get input string, get alphabet, set variables)
    char* S = "aaa$";
    int n = 7;
    char* alphabet = "$abn";

    // create tree struct
    tree* t = init();

    t->root->parent = t->root;

    // populate tree

    // test: insert T1: "banana$";
    findPath(t->root, S, 0);

    printNodeInfo(t->root);
    printNodeInfo(t->root->children[0]);


    //      banana$
    //      0123456

    //for (int i = 1; i < n; i++) {
        // insert (String, start index)
        //insert(t, S, i-1);
    //}


    // run API


    return 0;
}