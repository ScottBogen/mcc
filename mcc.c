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
#include <sys/time.h>

// two globals for ID:

int lower_id = 1;   // stores ID of leaves (1 to strlen(S))
int upper_id;       // stores ID of internal nodes

int alphabet_length;
int line_iterator = 1;  // used to ensure 10 li

typedef struct Node {
    int id;
    int depth;          // how deep we are into the string "banana$". 2 = the first 'n'.
    int label_length;   // how much further we go, >= 1. 
                        // 2 = the second 'a'. Thus label = "na";
    struct Node** children;
    struct Node* parent;
    struct Node* SL;

    // for labels
    int start_index;
    int end_index;

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
void enumerate(node* n, char* S);
void BWT(tree* t);
void sortChildren(node* n, char* S);
node* findPath(tree* t, node* v, char* S, int offset);
void printNodeInfo(node* temp, char* S);
node* nodeHops(node* n, char* S, char* beta, int offset); 

// creates tree, sets root to null, sets SL to self
tree* init() {
    tree* t = (tree*) malloc(sizeof(struct Tree));

    node* r = createNode();
    r->children = NULL;
    r->parent = NULL;
    r->depth = 0;
    r->id = 0;
    r->label = NULL;
    r->SL = r;
    r->parent = r;
    r->start_index = -1;
    r->end_index = -1;

    t->root = r;
    t->u = r;

    return t;
}

// creates a node
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

// FindPath algorithm. Offset measures where in S we are. 
node* findPath(tree* t, node* v, char* S, int offset) {

    if(v->children != NULL) {
        for (int i = 0; i < alphabet_length; i++) {     // for each child 
            if (v->children[i] != NULL) {                   // if such a child does exist
                node* child = v->children[i];
                if (S[child->start_index] == S[offset]) {           // if the first index of the label matches with s[offset]

                    int length = child->end_index - child->start_index + 1;

                    if(strncmp(S+child->start_index, S+offset, length) == 0) {        // if they are one in the same, we will have to jump to it 
                        node* baby = findPath(t,child, S, offset+length);
                        return baby;
                    }
                    else {                                  // if they are not one in the same, we must break here along the edge and create two children.

                        // path does not exhaust label -- we must create new nodes
                        int matches = 0;
                        int j = offset;
                        int k = child->start_index;

                        while (S[j++] == S[k++]) {
                            matches++;      // count # matches on the current label
                        }


                        /*              
                                        What we're doing:
                                              parent
                                                | 
                                            new_parent
                                            /       \
                                        child      new_child
                        */

                        node* new_parent = createNode();
                        new_parent->id = upper_id++; // should be upper id
                        new_parent->start_index = child->start_index;
                        new_parent->end_index = child->start_index + matches - 1;
                        new_parent->parent = v;
                        new_parent->SL = NULL;
                        new_parent->depth = v->depth + (new_parent->end_index - new_parent->start_index + 1);

                        node* new_child = createNode();
                        new_child->id = lower_id++;
                        new_child->start_index = offset+matches;
                        new_child->end_index = offset + matches + strlen(S+offset+matches) - 1;
                        new_child->parent = new_parent;
                        new_child->SL = NULL;
                        new_child->depth = new_parent->depth + (new_child->end_index - new_child->start_index + 1);

                        child->parent = new_parent;
                        child->start_index = child->parent->end_index + 1;
                        child->depth = new_parent->depth + (child->end_index - child->start_index + 1);

                        new_parent->children = (node**) malloc(alphabet_length * sizeof(struct Node));
                        new_parent->children[0] = new_child;
                        new_parent->children[1] = child;

                        sortChildren(new_parent, S);

                        t->u = new_parent;

                        v->children[i] = new_parent;
                        sortChildren(v, S);
                        return new_parent;
                    }
                } 
            }
        }
    }

    // if we reach here, node has no adequate children, so we need to make one
    // this node will be a LEAF
    node* temp = createNode();
    temp->id = lower_id++;
    temp->children = NULL;
    temp->start_index = offset;
    temp->end_index = strlen(S) - 1;    // this end index is correct because the node
                                        // is a LEAF, thus label goes to the end of S
    temp->SL = NULL;
    temp->parent = v;
    temp->children = NULL;
    temp->depth = v->depth + (temp->end_index - temp->start_index + 1);

    // insert temp 
    int i = 0;

    if (v->children != NULL) {
        while (v->children[i]) { 
            i++; 
        };
    } 
    else {
        v->children = (node**) malloc(alphabet_length * sizeof(struct Node));
    }

    v->children[i] = temp;
    sortChildren(v, S);       
    return temp;   
}

// test node info (for me)
void printNodeInfo(node* temp, char* S) {
    printf("Node Info:\n");
    printf("\tid:\t\t%d\n", temp->id);
    //printf("\tparent id:\t%d\n", temp->parent->id);

    printf("\tparent id\t");
    if (temp->parent) { printf("%d\n", temp->parent->id); } else { printf("no parent\n"); }

    printf("\tchildren?\t");
    if (temp->children) { printf("yes\n"); } else { printf("no\n"); }

    printf("\tstart index:\t%d\n", temp->start_index);
    printf("\tend index:\t%d\n", temp->end_index);
    printf("\tdepth:\t\t%d\n", temp->depth);

    char output[64];

    for (int i = 0; i < 64; i++) { output[i] = 0; }

    strncpy(output, S+temp->start_index, (temp->end_index - temp->start_index + 1));

    printf("\tlabel:\t\t%s\n", output);
    printf("\tSL?\t\t");
    if (temp->SL) { printf("%d\n", temp->SL->id); } else {printf("no SL\n");} 
    printf("\n");
}

// test function (for me) to print children of node n
void searchTree(node* n, char* S) {
    
    //printf("in node id:%d\n", n->id);

    // search children, l thru r
    for (int i = 0; i < alphabet_length; i++) {
        if (n->children != NULL) {
            if (n->children[i] != NULL) {
                searchTree(n->children[i], S);
            }
        }
    }

    printNodeInfo(n,S);
}

void writeToFile(node* n, char* S) {
    FILE* fp;
    fp = fopen("output.txt", "w");
    searchTreeBST(n, S, fp);
    fclose(fp);
}

void searchTreeBST(node* n, char* S, FILE* fp) {
    
    //printf("in node id:%d\n", n->id);

    // search children, l thru r
    if (n->children != NULL) {
        for (int i = 0; i < alphabet_length; i++) {
            if (n->children[i] != NULL) {
                searchTreeBST(n->children[i], S, fp);
            }
        }
    }

    // when it has returned, print the value of n
    if (n->children == NULL) {
        if (n->id > 1) {
            fprintf(fp, "%c\n", S[n->id-2]);
        }
        else { 
            fprintf(fp, "$\n", n->id);
        }
    } 
}

void display(node* n) {
    printf("%-8d ", n->id);

    if (line_iterator % 10 == 0) { 
        printf("\n"); 
    }
    line_iterator++;

    if (n->children != NULL) {
        for (int j = 0; j < alphabet_length; j++) {
            if (n->children[j] != NULL) {
                display(n->children[j]);
            }
        }
    }
}

// DFS style of printing node depths
void enumerate(node* n, char* S) {
    printf("%-8d ", n->depth);

    if (line_iterator % 10 == 0) { 
        printf("\n"); 
    }
    line_iterator++;

    if (n->children != NULL) {
        for (int j = 0; j < alphabet_length; j++) {
            if (n->children[j] != NULL) {
                enumerate(n->children[j], S);
            }
        }
    }
}


char* readFile(char* file_name) {
    FILE* fp;
    char* line = NULL;
    size_t len;

    fp = fopen(file_name, "r");

    if (!fp) { printf("FASTA file %s not opened\n", file_name); exit(0); }
    printf("FASTA file %s opened\n", file_name);
    
    int string_length = 0;
    int i = 0;

    while(getline(&line, &len, fp) != -1) {
        if (line[0] != '>') {
            while (line[i] != '\0' && line[i] != '\n') {
                string_length++;
                i++;
            }
            i = 0;
        }
    }
    
    char* S = (char*) malloc(string_length+1);

    rewind(fp);
    
    line = NULL;

    i = 0;
    string_length = 0;

    while(getline(&line, &len, fp) != -1) {
        if (line[0] != '>') {
            while (line[i] != '\0' && line[i] != '\n') {
                S[string_length++] = line[i++];
            }
            i = 0;
        }
    } 
    S[string_length] = '$';

    close(fp);
    return S;

}


// to maintain lexicographical structure
void sortChildren(node* n, char* S) {
    int i,j;

    int len = 0;
    while(n->children[len] != NULL) { len++; }

    if (len < 2) { return; }    // nothing to sort

    // lazy bubble sort
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            if (i == j) { continue; }
            int a = n->children[i]->start_index;
            int b = n->children[j]->start_index;

            if (S[a] < S[b]) {
                node* temp = n->children[i];
                n->children[i] = n->children[j];
                n->children[j] = temp;
            }
        }
    } 
}


// ex: insert(t, S, 1) --> insert("anana$");
void insert(tree* t, char* S, int i) {

    // make local pointers
    node* root = t->root;
    node* u = t->u;

    // check cases

    // cases 1-2: SL(u) is known
    if (u->SL != NULL) {
        // case 1: u is not the root
        if (u != root) {
            node* v = u->SL;
            int alpha_length = v->depth;
            findPath(t, v, S, i+alpha_length);
        }
        // case 1a: u is the root
        else {
            node* v = root;
            findPath(t, v, S, i);
        }
    }

    // cases 2A-2B: SL(u) is known
    else {
        // case 2A: u' is not the root
        if (u->parent != root) {
            node* u_prime = u->parent;
            node* v_prime = u_prime->SL;

            int beta_start = u->start_index;
            int beta_end = u->end_index;
            int beta_length = beta_end - beta_start + 1;

            int alpha_prime_length = u_prime->depth - 1;
            int alpha_length = beta_length + alpha_prime_length;

            char* beta = (char*) malloc(beta_length);
            strncpy(beta, S+beta_start, beta_length);

            node* v = nodeHops(v_prime, S, beta, alpha_prime_length);

            t->u = v;
            u->SL = v;

            findPath(t, v, S, i+v->depth);

            free(beta);
        }

        // case 4: u' is the root
        else {
            node* u_prime = u->parent; 

            int beta_start = u->start_index;
            int beta_length = u->depth;
            int beta_prime_length = beta_length - 1;


            if (beta_prime_length <= 0) {   // if B' does not exist, we don't need to do any node hopping
                node* v = findPath(t, u_prime, S, i);
                u->SL = root;
            }
            else {          // let's make Beta and put it to work

                char* beta_prime = (char*) malloc(beta_prime_length);
                strncpy(beta_prime, S+i, beta_prime_length);

                node* v_prime = nodeHops(u_prime, S, beta_prime, 0);
                node* v = findPath(t, v_prime, S, i+v_prime->depth);
                
                u->SL = v_prime;

                free(beta_prime);
            }
        }
    }
}


// nodeHops algorithm.
// beta is the beta string itself, offset is how far into the beta string we are
node* nodeHops(node* n, char* S, char* beta, int offset) {  
    
    if (offset > strlen(beta)) {
        return n->parent;       // offset exceeds length of beta, return from node
    }

    if (!n->children) { return n->parent;}      // leaf nodes do us no good

    for (int i = 0; i < alphabet_length; i++) {
        if (n->children[i]) {
            node* child = n->children[i];
            if (S[child->start_index] == beta[offset]) {        // time to hop to the child node
                return nodeHops(child, S, beta, offset+(child->end_index - child->start_index + 1));
            } 
        }
    }
    return n;   // base case: no matching children are found, the parent is our node.
}


void readAlphabetFile(char* file){
    FILE* fp;
    char* line = NULL;
    size_t len;

    fp = fopen(file, "r");

    if (!fp) { printf("Alphabet file %s not opened\n", file); exit(0); }
    printf("Alphabet file %s opened\n", file);

    alphabet_length = 0;
    int i = 0;

    while(getline(&line, &len, fp) != -1) {
        if (line[0] != '>') {
            while (line[i] != '\0' && line[i] != '\n') {
                if (line[i] != ' ') {
                    alphabet_length++;
                }
                i++;
            }
            i = 0;
        }
    }
    alphabet_length++;  // for $, per the way I set it up

    fclose(fp);
}


//  $ <test executable> <input file containing sequence s> <input alphabet file> 
int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Example functionality: ./a.out <s1.fasta> <alphabet.txt>\n");
        exit(0);
    }

    char* sequence_file = argv[1];
    char* alphabet_file = argv[2];

    char* S = readFile(sequence_file);
    readAlphabetFile(alphabet_file);


    printf("S length = %d\t alphabet length = %d\n", strlen(S), alphabet_length);
    
    upper_id = strlen(S) + 1;       // upper_id is used for internal nodes -- it'll start at 1 + the number of leaves
    
    int n = strlen(S);

    // create tree struct
    tree* t = init();


    struct timeval start, stop;

    printf("Press ENTER to build tree\n");
    getchar();
    printf("\n");
    
    gettimeofday(&start, NULL);
    
    // loop to insert new nodes into tree
    for (int i = 0; i < n; i++) {
        // uncomment the two below lines to see if/when the program breaks
        
        // printf("--- ITERATION #%d, CHARACTER=%c ---\r", i, S[i]);
        // fflush(stdout);
        
        insert(t, S, i);
    }
    
    gettimeofday(&stop, NULL);
    printf("\n");
    printf("Elapsed time: %llu ms\n", stop.tv_usec - start.tv_usec);

    printf("Tree build complete.\n");
    printf("Press ENTER to enumerate from root\n");
    getchar();
    printf("\n");

    // enumerate tree
    line_iterator = 1;
    enumerate(t->root, S);

    printf("\nPress ENTER to write DFS results to output.txt\n");
    getchar();
    printf("\n");
    writeToFile(t->root, S);

    printf("Output now exists in output.txt\n");
    printf("Press ENTER to display children id's of node 0 (root)\n");
    getchar();
    printf("\n");

    node* disp = t->root;
    printf("displaying id and children of node: %d\n\n", disp->id);
    line_iterator = 1;
    display(disp);
    printf("\n");

    printf("McCreight's Algorithm program finished.\n");

    return 0;
}