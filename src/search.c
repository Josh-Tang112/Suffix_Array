#include "search.h"

int naive_binary(int *SA, char *ref, char *query, int l, int r) {
    int c = (r + l) / 2;
    int cmp = strncmp(ref + SA[c], query, strlen(query));
    // printf("%3d %8d %8d %.15s %.15s\n",cmp,l,r,query,ref+SA[c]);
    if(!cmp){
        return c;
    }
    else if(l == r) {
        return -1;
    }
    else if(cmp < 0) { // SA[c] < query
        l = c + 1;
        return naive_binary(SA, ref, query, l, r);
    }
    else { // SA[c] > query
        r = c - 1;
        return naive_binary(SA, ref, query, l, r);
    }
}

struct tuple naive_query(int *SA, int SA_len, char *ref, char *query, int l, int r){
    struct tuple ret;
    int start = naive_binary(SA, ref, query, l, r);
    if(start < 0){ret.start = 0; ret.end = 0; return ret;}
    int end = start + 1, len = strlen(query);
    while(start > 0 && !strncmp(ref + SA[start - 1], query, len)){
        start--;
    }
    while(end < SA_len && !strncmp(ref + SA[end], query, len)) {
        end++;
    }
    ret.start = start;
    ret.end = end;
    return ret;
}

int LCP(char *p, char *c, int *i){
    *i = 0;
    while(p[*i] != '\0' && c[*i] != '\0'){
        if(p[*i] == c[*i]){(*i)++;}
        else if(p[*i] < c[*i]){
            return -1;
        }
        else if (p[*i] > c[*i]){
            return 1;
        }
    }
    if(p[*i] == '\0'){return 0;}
    else {return 1;}
}
unsigned int min(int a, int b) {
    if(a <= b) {return a;}
    else {return b;}
}
int simpleaccel_binary(int *SA, char *ref, char *query, int l, int r, int LCP_pl, int LCP_pr){
    int c = (r + l) / 2;
    int deduced_min = min(LCP_pl, LCP_pr), LCP_pc;
    int cmp = LCP(query + deduced_min, ref + SA[c] + deduced_min, &LCP_pc);
    LCP_pc += deduced_min;
    // printf("%8d %8d %8d\n",LCP_pl,LCP_pc,LCP_pr);
    // printf("%3d %8d %8d %8d %.15s %.15s\n",cmp,l,r,deduced_min,query,ref+SA[c]);
    if(!cmp) { // query == SA[c]
        return c;
    }
    else if(l == r) {
        return -1;
    }
    else if(cmp < 0) { // query < SA[c]
        return simpleaccel_binary(SA, ref, query, l, c - 1, LCP_pl, LCP_pc);
    }
    else { // query > SA[c]
        return simpleaccel_binary(SA, ref, query, c + 1, r, LCP_pc, LCP_pr);
    }
}

struct tuple simpleaccel_query(int *SA, int SA_len, char *ref, char *query, int l, int r) {
    struct tuple ret;
    int LCP_pl, LCP_pr;
    LCP(ref + SA[l], query, &LCP_pl);
    LCP(ref + SA[r - 1], query, &LCP_pr);
    int start = simpleaccel_binary(SA, ref, query, l, r, LCP_pl, LCP_pr);
    if(start < 0){ret.start = 0; ret.end = 0; return ret;}
    int end = start + 1, len = strlen(query);
    while(start > 0 && !strncmp(ref + SA[start - 1], query, len)){
        start--;
    }
    while(end < SA_len && !strncmp(ref + SA[end], query, len)) {
        end++;
    }
    ret.start = start;
    ret.end = end;
    return ret;
}