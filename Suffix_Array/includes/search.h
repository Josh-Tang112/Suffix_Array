#ifndef SEARCH
#define SEARCH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "tuple.h"

struct tuple naive_query(int *SA, int SA_len, char *ref, char *query, int l, int r);

unsigned int min(int a, int b);

struct tuple simpleaccel_query(int *SA, int SA_len, char *ref, char *query, int l, int r);

#endif