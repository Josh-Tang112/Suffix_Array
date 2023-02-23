#ifndef optparser
#define optparser

#include <argp.h>

struct build_arguments {
	int preftab;
	char *ref;
	char *output;
};

error_t build_parser(int key, char *arg, struct argp_state *state);

struct build_arguments build_parseopt(int argc, char *argv[]);

struct query_arguments {
	char *index;
	char *query;
	int mode;
	char *output;
};

error_t query_parser(int key, char *arg, struct argp_state *state);

struct query_arguments query_parseopt(int argc, char *argv[]);

#endif