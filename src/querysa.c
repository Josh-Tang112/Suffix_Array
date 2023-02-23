#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "tuple.h"
#include "search.h"
#include "../includes/optparser.h"

int main(int argc, char *argv[]) {

    if (argc > 9 || argc < 5){
        printf("Wrong number of argumants. Refer to README for more details.\n");
        return 1;
    }

    struct query_arguments args = query_parseopt(argc, argv);
    if(!args.index || !args.query || !args.output){
        printf("Wrong command line input is given. Refer to README for more details.\n");
        return 1;
    }

    /* Read in the index file */
    FILE *in = fopen(args.index, "rb");
    if(!in){perror(NULL); return 1;}

    /* Read in the preftab */
    int preftab_k = 0, preftab_len = 0;
    char zero_byte;
    struct tuple *preftab = NULL;
    fread(&preftab_k, sizeof(int), 1, in); // read preftab
    fread(&zero_byte, 1, 1, in); // read zero byte 
    if(preftab_k > 0){
        fread(&preftab_len, sizeof(int), 1, in); // number of entries
        fread(&zero_byte, 1, 1, in); // read zero byte 
        preftab = (struct tuple *)malloc(sizeof(struct tuple) * (preftab_len));
        for(int i = 0; i < preftab_len; i++){ // read in [start, end)
            fread(&preftab[i].start, sizeof(int), 1, in);
            fread(&preftab[i].end, sizeof(int), 1, in);
        }
        fread(&zero_byte, 1, 1, in); // read zero byte 
    }

    /* Read in SA*/
    unsigned int real_len = 0;
    fread(&real_len, sizeof(int), 1, in); // number of entries
    fread(&zero_byte, 1, 1, in); // read zero byte
    int *SA = (int *)malloc(sizeof(int) * real_len);
    size_t num_read = fread(SA, sizeof(int), real_len, in); // SA entries
    if(num_read < real_len){ 
        perror("There is discrepency between expected number of entries and actual entries.");
        return 1;
    }
    fread(&zero_byte, 1, 1, in); // read zero byte

    /* Read in reference text */
    char *txt = (char *)malloc(real_len + 1);
    num_read = fread(txt, 1, real_len, in);
    if(num_read < real_len){
        perror("Expected reference string length is different from actual string length.");
        return 1;
    }
    txt[real_len] = '\0';

    /* Read in query file */
    FILE *query_file = fopen(args.query, "r");
    size_t start = ftell(query_file); // get file size
    fseek(query_file, 0, SEEK_END);
    size_t end = ftell(query_file);
    fseek(query_file,0,SEEK_SET);
    unsigned int query_file_len = end - start;
    char *queries = (char *)malloc(query_file_len), ch; // allocate mem for query file
    int num_query = 0, name_start = 0, name_reading = 0, query_start = 0, query_reading = 0;
    int n = 0; num_read = 0;
    do {ch = fgetc(query_file); if(ch == '>'){num_query++;} } while(ch != EOF); // get number of queries
    fseek(query_file,0,SEEK_SET);
    char **query_names = (char **)malloc(num_query * sizeof(char *)); // allocate mem for pointers to q name
    char **query_str = (char **)malloc(num_query * sizeof(char *)); // allocate mem for pointers to q str
    do {
        ch = fgetc(query_file);
        if(ch == '>') {
            if(query_reading == 1){queries[n] = '\0'; n++;}
            name_start = 1;
        }
        else if(name_start == 1 && ch != '\n'){
            queries[n] = ch; 
            query_names[num_read] = queries + n;
            n++; name_start = 0; name_reading = 1;
        }
        else if(name_reading == 1 && ch != '\n'){
            queries[n] = ch; n++;
        }
        else if(name_reading == 1 && ch == '\n'){
            queries[n] = '\0';
            n++; name_reading = 0; query_start = 1;
        }
        else if(query_start == 1 && isalpha(ch)){
            queries[n] = toupper(ch);
            query_str[num_read] = queries + n;
            n++; num_read++; query_start = 0; query_reading = 1;
        }
        else if(query_reading == 1 && isalpha(ch)){
            queries[n] = toupper(ch); n++;
        }
    } while(ch != EOF);

    time_t start_timer, end_timer;
    FILE *out = fopen(args.output, "w+");
    for(int i = 0; i < num_query; i++){
        struct tuple ret;
        time(&start_timer);
        if(preftab_k > 0){
            int j = 0;
            while(j < preftab_len &&
                strncmp(txt + SA[preftab[j].start], query_str[i], 
                min(preftab_k, strlen(query_str[i])))){
                j++;
            }
            if(j == preftab_len) {
                ret.start = 0; ret.end = 0;
            }
            else if(!args.mode){ // naive
                ret = naive_query(SA,real_len,txt,query_str[i],preftab[j].start,preftab[j].end);
            }
            else{
                ret = simpleaccel_query(SA,real_len,txt,query_str[i],preftab[j].start,preftab[j].end);
            }
        }
        else if(args.mode) { // accel no preftab
            ret = simpleaccel_query(SA,real_len,txt,query_str[i],0,real_len);
        }
        else { // simple no preftab
            ret = naive_query(SA,real_len,txt,query_str[i],0,real_len);
        }
        time(&end_timer);
        printf("For query of length %ld, it takes %ld sec to complete.\n", strlen(query_str[i]),end_timer - start_timer);
        fprintf(out, "%s",query_names[i]);
        fprintf(out, "\t%d", ret.end - ret.start);
        for(unsigned int j = ret.start; j < ret.end; j++){
            fprintf(out, "\t%d",SA[j]);
            // printf("\n%d %.12s",SA[j], txt + SA[j]);
        }
        fprintf(out, "\n");
    }

    fclose(in);
    fclose(query_file);
    fclose(out);
    free(SA);
    free(txt);
    free(args.index);
    free(args.query);
    free(args.output);
    return 0;
}