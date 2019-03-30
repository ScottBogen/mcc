Scott Bogen - Project 2 (McCreight's Algorithm)

- All code is contained within mcc.c

- Example usage:
    $   gcc -o mcc mcc.c 
    $   ./mcc Human-BRCA2-cds.fasta DNA_alphabet.txt

- BWT writes to `output.txt`, and will overwrite it if it exists

- `terminal_output.txt` shows the results from stdout for `colorblind_human_gene.fasta`, to show off other aspects of the API

- All inputs work except for the yeast -- a segfault happens somewhere around insert 5000

- I never used string labels for edges. Instead, I stored the starting and ending indices where the substring occurs in S.
    - ex: S="BANANA$", start=2, end=4, label then = "NAN"


Thank you for the 24h extention. A fatal bug noted on Monday took until Thursday night to resolve.