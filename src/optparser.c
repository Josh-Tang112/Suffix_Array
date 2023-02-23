/* This is an example program that parses the options provided on the
 * command line that are needed for assignment 0. You may copy all or
 * parts of this code in the assignment */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <argp.h>

#include "../includes/optparser.h"

error_t build_parser(int key, char *arg, struct argp_state *state) {
	struct build_arguments *args = state->input;
	error_t ret = 0;
	switch(key) {
	case 'p':
		args->preftab = atoi(arg);
		break;
	case 'r':
		args->ref = (char *)malloc(strlen(arg) + 1);
		strncpy(args->ref, arg, strlen(arg));
		break;
	case 'o':
		args->output = (char *)malloc(strlen(arg) + 1);
		strncpy(args->output, arg, strlen(arg));
		break;
	default:
		ret = ARGP_ERR_UNKNOWN;
		break;
	}
	return ret;
}

struct build_arguments build_parseopt(int argc, char *argv[]) {
	struct build_arguments args;

	bzero(&args, sizeof(args));



	struct argp_option options[] = {
		{ "preftab", 'p', "pref", 0, "Length of each entry in the prefix table",0},
		{ "reference", 'r', "ref", 0,"File path to file containing reference",0},
		{"output",'o',"out",0,"File path to the output destination",0},
		{0}
	};
	struct argp argp_settings = { options, build_parser, 0, 0, 0, 0, 0 };
	if (argp_parse(&argp_settings, argc, argv, 0, NULL, &args) != 0) {
		printf("Got an error condition when parsing\n");
	}

	return args;
}

error_t query_parser(int key, char *arg, struct argp_state *state) {
	struct query_arguments *args = state->input;
	error_t ret = 0;
	switch(key) {
	case 'i':
		args->index = (char *)malloc(strlen(arg) + 1);
		strncpy(args->index, arg, strlen(arg));
		break;
	case 'q':
		args->query = (char *)malloc(strlen(arg) + 1);
		strncpy(args->query, arg, strlen(arg));
		break;
	case 'm':
		args->mode = atoi(arg);
		break;
	case 'o':
		args->output = (char *)malloc(strlen(arg) + 1);
		strncpy(args->output, arg, strlen(arg));
		break;
	default:
		ret = ARGP_ERR_UNKNOWN;
		break;
	}
	return ret;
}


struct query_arguments query_parseopt(int argc, char *argv[]) {
	struct argp_option options[] = {
		{ "index", 'i', "index", 0, "File path to the built suffix array", 0},
		{ "queries", 'q', "query", 0, "File path to the list of queries", 0},
		{ "query_mode", 'm', "mode", 0, "Mode used for querying", 0},
		{ "output", 'o', "out", 0, "File path to output file", 0},
		{0}
	};

	struct argp argp_settings = { options, query_parser, 0, 0, 0, 0, 0 };

	struct query_arguments args;
	bzero(&args, sizeof(args));

	if (argp_parse(&argp_settings, argc, argv, 0, NULL, &args) != 0) {
		printf("Got error in parse\n");
	}


	return args;
}

