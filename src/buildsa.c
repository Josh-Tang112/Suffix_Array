#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <divsufsort.h>

#include "tuple.h"
#include "../includes/optparser.h"

unsigned int create_preftab(struct tuple *preftab, char *ref, int *SA, int SA_len, int k){
    unsigned int count = 0;
    preftab[0].start = 0;
    preftab[0].end = 1;
    for(int i = 1; i < SA_len; i++){
        if(SA_len - SA[i] < k)
            continue;
        if(!strncmp(ref + SA[i - 1], ref + SA[i], k)){
            preftab[count].end++;
        }
        else {
            // printf("%.10s %.10s\n",ref + SA[i - 1], ref + SA[i]);
            count++;
            preftab[count].start = i;
            preftab[count].end = i + 1;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 7){
        printf("Wrong number of argumants. Refer to README for more details.\n");
        return 1;
    }

    /* get commandline args */
    struct build_arguments args = build_parseopt(argc, argv);
    if(!args.ref || !args.output){
        printf("You must specify path to reference and output file. ");
        printf("Read README for more detail\n");
        return 1;
    }
    else if(args.preftab < 0){
        printf("The length of the minimum interested string should be >= 0.\n");
        return 1;
    }

    /* read in files */
    FILE *in = fopen(args.ref, "r");
    if(!in){
        perror(NULL);
        return 1;
    }

    /* get file size */
    size_t start = ftell(in);
    fseek(in, 0, SEEK_END);
    size_t end = ftell(in);
    fseek(in,0,SEEK_SET);
    printf("The reference file is %lu bytes long.\n", end - start);

    /* read in files without newline characters */
    char *txt = (char *)malloc(end - start + 1), ch;
    int flag = 0;
    unsigned int real_len = 0;
    do {
        ch = fgetc(in);
        if(ch == EOF)
            break;
        if(ch == '>')
            flag = 0;
        else if(ch == '\n' && flag == 0)
            flag = 1;
        else if(flag && isalpha(ch)){
            txt[real_len] = toupper(ch);
            real_len++;
        }
    } while (ch != EOF);

    /* declaring timer variable */
    time_t start_timer, end_timer;

    /* allocate memory for suffix array */
    int *SA = (int *)malloc(sizeof(int) * real_len);
    /* create the suffix array */
    time(&start_timer);
    if(divsufsort((unsigned char *)txt, SA, real_len) < 0){
        printf("Failed to create suffix array. Exting.\n");
        free(SA); free(txt); fclose(in); free(args.ref); free(args.output);
        return 1;
    }
    time(&end_timer);
    printf("Suffix Array Construction Takes %ld seconds.\n", end_timer - start_timer);
    
    /* if preftab is provided, create preftab */
    struct tuple *preftab = NULL;
    int count = 0;
    if(args.preftab > 0){
        preftab = (struct tuple *)calloc(pow(10, args.preftab), sizeof(struct tuple));
        if(!preftab){perror(NULL);}
        count = create_preftab(preftab, txt, SA, real_len, args.preftab);
        count++;
    }

    /* Writing to binary file */
    FILE *out = fopen(args.output, "wb+");
    int zero_byte = 0;
    /* Format of the Write: Preftab Zero_Byte Suffix_Array Zero_Byte Reference_String */
    /* If preftab is present */
    /* K(4 bytes) Zero_Byte Number_of_Entries(4 bytes) Zero_Byte pairs of 4-byte integers*/
    /* If preftab is not present */
    /* just a 4 byte zero */
    fwrite(&args.preftab, sizeof(int), 1, out); // k
    fwrite(&zero_byte,1,1,out); // zero byte
    if(count > 0){
        fwrite(&count, sizeof(int), 1, out); // Number_of_Entries
        fwrite(&zero_byte,1,1,out); // zero byte
        for(int i = 0; i < count; i++){
            fwrite(&preftab[i].start, sizeof(int), 1, out); // [start part
            fwrite(&preftab[i].end, sizeof(int), 1, out); // end) part
        }
        fwrite(&zero_byte,1,1,out); // zero byte
    }
    /* Format of Suffix Array: Number_of_Entries(4 bytes) Zero_Byte Entries */
    fwrite(&real_len, sizeof(real_len), 1, out); // Number_of_Entries(4 bytes) 
    fwrite(&zero_byte,1,1,out); // zero byte
    size_t num_written = fwrite(SA, sizeof(int), real_len, out); // entries
    if(num_written < real_len) {
        perror("Number of written entries is different from the number of entries in the suffix entries.");
        return 1;
    }
    fwrite(&zero_byte,1,1,out); // zero byte
    /* Since we can infer the length of reference str by using the length of SA, just write the str to the file */
    num_written = fwrite(txt, 1, real_len, out); // ref
    if(num_written < real_len) {
        perror("Number of written entries is different from the length of reference in the suffix entries.");
        return 1;
    }

    free(SA);
    free(txt);
    fclose(in);
    fclose(out);
    free(args.ref);
    free(args.output);
    return 0;
}