#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char am[] =  { 'A',  'R',  'N',  'D',  'C',  'Q',  'E',  'G',  'H',  'I',  'L',  'K', \
             'M',  'F',  'P',  'S',  'T',  'W',  'Y',  'V' }; 
int BLOSUM62[21][21];
int **score; 
char **trace;

void loadBlosum62() {
    FILE *b;
    if( b = fopen("blosum62","r" ) ) {
       int i,j;
       for(i=0; i<20; i++) {
           for(j=0;j<20; j++) {
               fscanf(b, "%d", &BLOSUM62[i][j]);
           }
       }
    }
    else {
        printf("Error: file blosum62 not found!\n");
        exit(1);
    }
}

void readSequence(char *fname, char *s) {
    FILE *f;
    if(f = fopen(fname,"r")) {
        fscanf(f,"%s",s);
    }
    else {
        printf("Error: file %s not found!\n", fname);
        exit(1);
    }
}

void allocate(int l1, int l2) {
    int i;
    score = (int **)malloc(l2*sizeof(int *));
    trace = (char **)malloc(l2*sizeof(char *));
    for(i=0; i<l2; i++) {
        score[i] = (int *)malloc(l1*sizeof(int));
        trace[i] = (char *)malloc(l1*sizeof(char));
    }
}

void init(int l1, int l2) {
    int i;
    loadBlosum62();
    allocate(l1+1, l2+1);

    score[0][0] = 0;
    for(i=1; i<=l1; i++) {
        score[0][i] = score[0][i-1] - 10;
        trace[0][i] = 'l';
    }

    trace[0][0] = '0';
    for(i=1; i<=l2; i++) {
        score[i][0] = score[i-1][0] - 10;
        trace[i][0] = 'u';
    }
}

int amPos(char c) {
    int i = 0;
    while(am[i] != c)
        i++;
    return i;
}

char label;
int max(int a, int b, int c) {
    int m = a;
    label = 'd';

    if( m < b ) {
        m = b;
        label = 'u';
    }
    if(m < c) {
        m = c;
        label = 'l';
    }
    return m;
}

#define SEQ_MAX_LEN 500

int main() {
    int i,j,k;
    char *s1 = malloc(sizeof(char[SEQ_MAX_LEN]));
    char *s2 = malloc(sizeof(char[SEQ_MAX_LEN]));
    char *a1 = malloc(sizeof(char[SEQ_MAX_LEN]));
    char *a2 = malloc(sizeof(char[SEQ_MAX_LEN]));
    int l1, l2;
    int qd, qu, ql;
    int g = -10;

    readSequence("s1",s1);
    readSequence("s2",s2);
    
    l1 = strlen(s1);
    l2 = strlen(s2);

    init(l1,l2);

    /* Scoring */
    for(i=1; i<=l2; i++) {
        for(j=1; j<=l1; j++) {
            qd = score[i-1][j-1] + \
                 BLOSUM62[amPos(s2[i-1])][amPos(s1[j-1])];
            qu = score[i-1][j] + g;
            ql = score[i][j-1] + g;
            score[i][j] = max(qd, qu, ql);
            trace[i][j] = label;
        }
    }

    /* Trace back */
    k = 0;
    for(i=l2, j=l1; trace[i][j] != '0';){
        if( trace[i][j] == 'd' ) {
            a1[k] = s1[j-1];
            a2[k] = s2[i-1];
            i--;
            j--;
            k++;
        }
        else if( trace[i][j] = 'l' ) {
            a1[k] = s1[j-1];
            a2[k] = '-';
            j--;
            k++;
        }
        else if( trace[i][j] = 'u' ) {
            a2[k] = s2[i-1];
            a1[k] = '-';
            i--;
            k++;
        }
    }
    a1[k] = '\0';
    a2[k] = '\0';

    for(i=k-1; i>=0; i--)
        printf("%c", a1[i]);
    printf("\n");

    for(i=k-1; i>=0; i--)
        printf("%c", a2[i]);
    printf("\n");

    return 0;
}
