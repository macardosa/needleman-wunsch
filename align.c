#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SEQ_MAX_LEN 10000
char am[] =  { 'A',  'R',  'N',  'D',  'C',  'Q',  'E',  'G',  'H',  'I',  'L',  'K', \
             'M',  'F',  'P',  'S',  'T',  'W',  'Y',  'V' }; 
int BLOSUM62[21][21];
int **score; 
char **trace;
int similarity = 0;

void loadBlosum62() {
    FILE *b;
    if( (b = fopen("blosum62","r" )) != NULL ) {
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
    char c;
    int i;
    int bufsize = 1000;
    char *buffer = (char *) malloc(bufsize*sizeof(char));
    if(buffer == NULL) {
        perror("Unable to allocate buffer");
        exit(1);
    }

    if((f = fopen(fname,"r")) != NULL) {
        fgets(buffer, bufsize, f);
        i=0;
        while( fscanf(f,"%c", &c) != EOF ) {
            if(isupper(c))
                s[i++] = c;
        }
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

int main(int argc, char **argv) {
    if(argc < 3) {
        perror("Missing sequence!!");
        exit(1);
    }
    int i,j,k;
    char *s1 = malloc(sizeof(char[SEQ_MAX_LEN]));
    char *s2 = malloc(sizeof(char[SEQ_MAX_LEN]));
    char *a1 = malloc(sizeof(char[SEQ_MAX_LEN]));
    char *a2 = malloc(sizeof(char[SEQ_MAX_LEN]));
    int l1, l2;
    int qd, qu, ql;
    int g = -10;

    readSequence(argv[1], s1);
    readSequence(argv[2], s2);
    
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
            similarity++;
        }
        else if( trace[i][j] == 'l' ) {
            a1[k] = s1[j-1];
            a2[k] = '-';
            j--;
            k++;
        }
        else if( trace[i][j] == 'u' ) {
            a2[k] = s2[i-1];
            a1[k] = '-';
            i--;
            k++;
        }
    }
    a1[k] = '\0';
    a2[k] = '\0';

    /* Printing aligned strings */
    printf("Needleman Wunsch Similarity ==> %.1f %%\n\n", \
            (double) ((int) (((double)similarity/k )*1000 + 0.5 ) )/10); 
    printf("=> %s aligned\n", argv[1]);
    for(i=k-1, j=0; i>=0; i--) {
        printf("%c", a1[i]);
        j++;
        if(j == 60) {
            printf("\n");
            j = 0;
        }
    }
    printf("\n\n");

    printf("=> %s aligned\n", argv[2]);
    for(i=k-1, j=0; i>=0; i--) {
        printf("%c", a2[i]);
        j++;
        if(j == 60) {
            printf("\n");
            j = 0;
        }
    }
    printf("\n");

    return 0;
}
