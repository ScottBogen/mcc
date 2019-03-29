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

// two globals for ID:

int lower_id = 1;
int upper_id = 100;       // set to n in main, subtract for each internal node
int alphabet_length = 5;

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
void enumerate(node* u);
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

    r->parent = r;
    r->SL = r;

    t->root = r;
    t->u = r;
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

node* findPath(tree* t, node* v, char* S, int offset) {

    /* 
        If a node has no child, simply create one. 
        - Make the ID the lower_id.
        - Make the start index the offset.
        - Make the label length the string length minus the offset
    */
    if (!v->children) {
        node* temp = createNode();
        temp->id = lower_id++;
        temp->children = NULL;
        temp->depth = v->depth + temp->label_length;
        temp->SL = NULL;
        temp->parent = v;

        temp->start_index = offset;
        temp->end_index = strlen(S) - offset - 1;

        v->children = (node**) malloc(alphabet_length * sizeof(struct Node));
        v->children[0] = temp;
    }

    else {
        for (int i = 0; i < alphabet_length; i++) {     // for each child 
            if (v->children[i] != NULL) {                   // if such a child does exist
                node* child = v->children[i];
                printf("findPath: testing child #%d, id = %d\n", i, child->id);
                if (S[child->start_index] == S[offset]) {           // if the first index of the label matches with s[offset]
                    printf("TRUE: label = %c, s[offset] = %c\n", S[child->start_index], S[offset]);
                    printf("child start index: %d\t string offset: %d\n", child->start_index, offset);

                    int length = child->end_index - child->start_index + 1;

                    //printf("child basic info:\n");
                    //printNodeInfo(child, S);

                    if(strncmp(S+child->start_index, S+offset, length) == 0) {        // if they are one in the same, we will have to jump to it 
                        printf("the input string exhausts the label\n");
                        printf("time to jump to this kid: id=%d, start index = %d, len = %d\n", child->id, child->start_index, length);
                        node* baby = findPath(t,child, S, offset+length);
                        return baby;
                    }
                    else {                                  // if they are not one in the same, we must break here along the edge and create two children.
                        printf("path does not exhaust label -- we must create new nodes\n");
                        int matches = 0;
                        int j = offset;
                        int k = child->start_index;

                        while (S[j++] == S[k++]) {
                            matches++;
                        }

                        //printf("matches = %d\n", matches);

                        // if this child already has 1 or 2 children, we don't need to create even more.
                        // make 

                        node* new_parent = createNode();
                        new_parent->id = upper_id--; // should be upper id
                        new_parent->start_index = child->start_index;
                        new_parent->end_index = child->start_index + matches - 1;
                        new_parent->parent = v;
                        new_parent->SL = NULL;

                        node* new_child = createNode();
                        new_child->id = lower_id++;
                        new_child->start_index = offset+matches;
                        new_child->end_index = offset + matches + strlen(S+offset+matches) - 1;
                        new_child->parent = new_parent;
                        new_child->SL = NULL;


                        child->parent = new_parent;
                        child->start_index = child->parent->end_index + 1;

                        new_parent->children = (node**) malloc(alphabet_length * sizeof(struct Node));
                        new_parent->children[0] = new_child;
                        new_parent->children[1] = child;
                
                        printf("CREATED NODE with id %d\n", new_parent->id);
                        printf("CREATED NODE with id %d\n", new_child->id);

                        sortChildren(new_parent, S);


                        t->u = new_parent; 
                        printf("\tnew u: %d\n", t->u->id);

                        v->children[i] = new_parent;
                        sortChildren(v, S);
                        return new_parent;
                    }
                } 
                else {
                    printf(" child %d is not the right child: input = %c, label[0] = %c\n", child->id, S[offset], S[child->start_index]);
                } 
            }
        }

        // if we get down here, it means we have children in v, but were not able to insert due to different 0th string indices
        // make a new node to be a new child for v.

        printf("Node had no adequate children... let's make one\n");

        // this node will be a LEAF
        node* temp = createNode();
        temp->id = lower_id++;
        temp->children = NULL;
        temp->start_index = offset;
        temp->end_index = strlen(S) - 1;
        temp->SL = NULL;
        temp->parent = v;
        temp->children = NULL;

        // insert temp 

        int i = 0;
        while (v->children[i]) { i++; };

        int position = i;

        v->children[i] = temp;
        sortChildren(v, S);       

        printf("CREATED NODE with id %d\n", temp->id);
        return v->children[i];    // unlikely we will have to do anything further in this recursive window
    }
}

int compareLabel(char* s1, char* s2, int i, int j) {
    int matches = 0;

    while(s1[i] == s2[j]) {
        matches++;
    }

    return matches;
}

void printNodeInfo(node* temp, char* S) {
    printf("Node Info:\n");
    printf("\tid:\t\t%d\n", temp->id);
    //printf("\tparent id:\t%d\n", temp->parent->id);

    printf("\tparent id\t");
    if (temp->parent) { printf("%d\n", temp->parent->id); } else { printf("no parent\n"); }

    printf("\tchildren?\t");
    if (temp->children) { printf("yes\n"); } else { printf("no\n"); }

    printf("\tstart index:\t%d\n", temp->start_index);
    //printf("\tend index:\t%d\n", temp->end_index);

    char output[64];

    for (int i = 0; i < 64; i++) { output[i] = 0; }

    strncpy(output, S+temp->start_index, (temp->end_index - temp->start_index + 1));

    printf("\tlabel:\t\t%s\n", output);
    printf("\tSL?\t\t");
    if (temp->SL) { printf("%d\n", temp->SL->id); } else {printf("no SL\n");} 
    printf("\n");
}

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

    //printNodeInfo(n,S);


    // when it has returned, print the value of n
     
    if (n->children == NULL) {
        if (n->id > 1) {
            printf("%d\t%c\n", n->id, S[n->id-2]);
        }
        else { 
            printf("%d\t$\n", n->id);
        }
    }
     
}

char* readFile(char* file_name) {
    FILE* fp;
    char* line = NULL;
    size_t len;

    fp = fopen(file_name, "r");

    if (!fp) { printf("FASTA file not opened\n"); exit(0); }
    printf("FASTA file opened\n");
    
    int string_length = 0;
    int i = 0;

    while(getline(&line, &len, fp) != -1) {
        if (line[0] != '>') {
            printf("%s\n", line);
            while (line[i] != '\0') {
                string_length++;
                i++;
            }
            i = 0;
        }
    }

    printf("string length: %d\n\n", string_length);
    
    char* S = (char*) malloc(string_length+1);

    rewind(fp);
    
    line = NULL;

    i = 0;
    string_length = 0;

    while(getline(&line, &len, fp) != -1) {
        if (line[0] != '>') {
            while (line[i] != '\0') {
                S[string_length++] = line[i++];
            }
            i = 0;
        }
    } 
    S[string_length] = '$';

    close(fp);
    return S;

}

int main(int argc, char** argv) {

    char* S = readFile("s3.fasta");

    // call pre-process (get input string, get alphabet, set variables)
    //char* S = "BANANA$";
    int n = strlen(S);
    char* alphabet = "$ACGT";

    // create tree struct
    tree* t = init();

    //t->root->parent = t->root;

    // populate tree
     
    for (int i = 0; i < n-1; i++) {
        //findPath(t, t->root, S, i);
    }
    
    //GTGGCGCGAG$
    for (int i = 0; i < n; i++) {
        printf("--- ITERATION #%d, c=%c ---\n", i, S[i]);
        //findPath(t, t->root, S, i);
        insert(t, S, i);
    }

    //insert(t, S, 0);
    //insert(t, S, 1);
    //insert(t, S, 2);
    //insert(t, S, 3);

    // search tree
    printf("---SEARCH TREE---\n");
    searchTree(t->root, S);

    printf("u = %d\n", t->u->id);
    
    //      banana$
    //      0123456

    //for (int i = 1; i < n; i++) {
        // insert (String, start index)
        //insert(t, S, i-1);
    //}

    // run API

    return 0;
}

// len = num children
void sortChildren(node* n, char* S) {
    int i,j;

    int len = 0;
    while(n->children[len] != NULL) { len++; }


    if (len < 2) { return; }

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

    printf("entered insert...\n");

    printf("u id = %d\n", u->id);
    // cases 1-2: SL(u) is known
    if (u->SL != NULL) {
        printf("not root\n");
        // case 1: u is not the root
        if (u != root) {
            printf("Case 1A entered\n");
            // take SL(u) to v
            node* v = u->SL;
            // findPath(v, s[i+alpha...])
            findPath(t, v, S, i + v->start_index); // +1 to remove 
        }
        // case 2: u is the root
        else {
            printf("Case 1B entered\n");
            // go to parent u 
            // take SL(u) to root (v)
            node* v = root;
            findPath(t, v, S, i);
        }
    }

    // cases 3-4: SL(u) is known
    else {
        // case 3: u' is not the root
        printf("root\n");
        if (u->parent != root) {
            printf("Case 2A entered\n");
            node* u_prime = u->parent;

            printf("UPRIME = %d\n", u_prime->id);

            if (!u_prime->SL) { 
                printf("OH SHIT OF UCK OOH FUCK\n");
            }

            node* v_prime = u_prime->SL;

            node* ancestor = u_prime;

            while (ancestor->parent != root) { ancestor = ancestor->parent; }

            printf("case3: my ancestor's id is %d w start index %d and end index %d\n", ancestor->id, ancestor->start_index, ancestor->end_index);

            int beta_start = u->start_index; 
            int beta_end = u->end_index;

            int alpha_prime_start = v_prime->start_index;
            int alpha_prime_end = v_prime->end_index;

            //printf("VPRIME ID = %d\n\n\n\n", v_prime->id);

            int str_length = beta_end - beta_start + 1;

            if (str_length <= 1 || alpha_prime_start - alpha_prime_end == 0) {
                printf("BETA is too short, going to findpath right away\n");
                node* v = findPath(t, root, S, i);
                u->SL = v;
                t->u = v;
                printf("setting suffix links to %d\n", v->id);
                
            }
            else { 
                char* beta = (char*) malloc(str_length);
                strncpy(beta, S+beta_start, str_length);
                printf("2a: beta start = %d, beta end = %d\n", beta_start, beta_end);

                node* v_parent = nodeHops(v_prime, S, beta, 0);
                printf("exited nodehops with vparent = id:%d\n", v_parent->id);
                printf("calling FINDPATH on node id=%d, offset=%d\n", v_parent->id, i + (ancestor->end_index - ancestor->start_index + 1));
                node* v = findPath(t, v_parent, S, i + (ancestor->end_index - ancestor->start_index + 1));  //-1 because it's alpha, not alpha prime
                // note: above line was +1 before I touched it, that's what worked with mississippi
                u->SL = v;
                t->u = v;
                printf("shitting suffix links to %d\n", v_prime->id);

                free(beta); 
            }

            //node* v = nodeHops(v_prime, S, beta, 0);
            // make a beta for the above line to use 
            //u->SL = v;
            //findPath(t, v, S, i + alpha);
        }

        // case 4: u' is the root
        else {
            printf("Case 2B entered\n");
            node* u_prime = u->parent;  // should be root
            printf("uprime id: %d (should be 0)\n", u_prime->id);
            int beta_prime_start = u->start_index + 1;        // +1 to remove 1st char
            int beta_prime_end = u->end_index; 

            int str_length = beta_prime_end - beta_prime_start + 1;

            if (str_length <= 1) { 
                node* v = findPath(t, u_prime, S, i);
                u->SL = v;
                t->u = v;
            }
            else {
                char* beta = (char*) malloc(str_length);
                strncpy(beta, S+i, str_length);

                node* v_prime = nodeHops(u_prime, S, beta, 0);
                printf("exited nodehops with v = id:%d\n", v_prime->id);

                printf("calling FINDPATH on node id %d, offset=%d\n", v_prime->id, i+beta_prime_start - 1);
                node* v = findPath(t, v_prime, S, i);
                
                u->SL = v;
                t->u = v;

                printf("case 2b: %d->SL = %d\n", u->id, v->id);
                free(beta);
            }
        }
    }
}

/*                                  beta_offset will be 0 on first call, and will track where in Beta we are as we move across strings 
    nodeHops(node* n, char* S, char* beta, int offset) {  
        for (int i = 0; i < alphabet_length; i++) {
            if (n->children[i]) {
                node* child = n->children[i];
                if (S[child->start_index] == B[offset]) {
                    printf("hopping to child[%d], id = %d\n", i, child->id);
                    return nodeHops(child, S, beta, offset+(child->end_index - child->start_index + 1);
                } 
            }
        }
        printf("no matching children were found in nodehops\n");
        return n;   // base case: no matching children are found, this is our node.
    }

*/

node* nodeHops(node* n, char* S, char* beta, int offset) {  
    
    if (!n->children) { return n->parent;}      // leaf nodes do us no good

    for (int i = 0; i < alphabet_length; i++) {
        if (n->children[i]) {
            node* child = n->children[i];
            if (S[child->start_index] == beta[offset]) {
                printf("hopping to child[%d], id = %d\n", i, child->id);
                return nodeHops(child, S, beta, offset+(child->end_index - child->start_index + 1));
            } 
        }
    }
    printf("no matching children were found in nodehops\n");
    return n->parent;   // base case: no matching children are found, the parent is our node.
}
