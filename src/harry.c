/*
 * Harry - A Tool for Measuring String Similarity
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

#include "config.h"
#include "common.h"
#include "harry.h"
#include "hconfig.h"
#include "util.h"
#include "input.h"
#include "measures.h"
#include "output.h"
#include "vcache.h"
#include "hmatrix.h"

/* Global variables */
int verbose = 0;
int log_line = 0;
config_t cfg;

static int print_conf = 0;
static char *measure = NULL;

/* Option string */
#define OPTSTRING       "n:ga:m:c:i:o:d:zulvqVhMCD"

/**
 * Array of options of getopt_long()
 */
static struct option longopts[] = {
    {"input_format", 1, NULL, 'i'},
    {"decode_str", 0, NULL, 1000},
    {"reverse_str", 0, NULL, 1001},
    {"stopword_file", 1, NULL, 1002},
    {"output_format", 1, NULL, 'o'},
    {"compress", 0, NULL, 'z'},
    {"triangular", 0, NULL, 'u'},
    {"measure", 1, NULL, 'm'},
    {"word_delim", 1, NULL, 'd'},
    {"num_threads", 1, NULL, 'n'},
    {"cache_size", 1, NULL, 'a'},
    {"global_cache", 0, NULL, 'g'},
    {"config_file", 1, NULL, 'c'},
    {"verbose", 0, NULL, 'v'},
    {"log_line", 0, NULL, 'l'},
    {"print_measures", 0, NULL, 'M'},
    {"print_config", 0, NULL, 'C'},
    {"print_defaults", 0, NULL, 'D'},
    {"quiet", 0, NULL, 'q'},
    {"version", 0, NULL, 'V'},
    {"help", 0, NULL, 'h'},
    {NULL, 0, NULL, 0}
};

/**
 * Prints version and copyright information to a file stream
 * @param f File pointer
 * @param p Prefix character
 * @param m Message
 * @return number of written characters
 */
int harry_version(FILE *f, char *p, char *m)
{
    return fprintf(f, "%sHarry %s - %s\n", p, PACKAGE_VERSION, m);
}

/**
 * Prints version and copyright information to a file stream
 * @param z File pointer
 * @param p Prefix character
 * @param m Message
 * @return number of written characters
 */
int harry_zversion(gzFile * z, char *p, char *m)
{
    return gzprintf(z, "%sHarry %s - %s\n", p, PACKAGE_VERSION, m);
}

/**
 * Print configuration
 * @param msg Text to add to output
 */
static void print_config(char *msg)
{
    harry_version(stdout, "# ", msg);
    config_print(&cfg);
}

/**
 * Print usage of command line tool
 */
static void print_usage(void)
{
    printf("Usage: harry [options] <input> <output>\n"
           "\nI/O options\n"
           "  -i,  --input_format <format>   Set input format for strings.\n"
           "       --decode_str              Enable URI-decoding of strings.\n"
           "       --reverse_str             Reverse (flip) all strings.\n"
           "       --stopword_file <file>    Provide a file with stop words.\n"
           "  -o,  --output_format <format>  Set output format for vectors.\n"
           "  -z,  --compress                Enable zlib compression of output.\n"
           "  -u,  --triangular              Save triangular matrix only.\n"
           "\nModule options:\n"
           "  -m,  --measure <name>          Set similarity measure.\n"
           "  -d,  --word_delim <delim>      Set delimiters for words.\n"
           "  -n,  --num_threads <num>       Set number of threads.\n"
           "  -a,  --cache_size <size>       Set size of cache in megabytes.\n"
           "  -g,  --global_cache            Enable global cache for similarity values.\n"
           "\nGeneric options:\n"
           "  -c,  --config_file <file>      Set configuration file.\n"
           "  -v,  --verbose                 Increase verbosity.\n"
           "  -l,  --log_line                Print a log line every minute\n"
           "  -q,  --quiet                   Be quiet during processing.\n"
           "  -M,  --print_measures          Print list of similarity measures\n"
           "  -C,  --print_config            Print the current configuration.\n"
           "  -D,  --print_defaults          Print the default configuration.\n"
           "  -V,  --version                 Print version and copyright.\n"
           "  -h,  --help                    Print this help screen.\n" "\n");
}

/**
 * Print version of Harry tool
 */
static void print_version(void)
{
    printf("Harry %s - A Tool for Measuring String Similarity\n"
           "Copyright (c) 2013 Konrad Rieck (konrad@mlsec.org)\n",
           PACKAGE_VERSION);
}

/**
 * Parse command line options
 * @param argc Number of arguments
 * @param argv Argument values
 * @param in Return pointer to input filename
 * @param out Return pointer to output filename
 */
static void harry_parse_options(int argc, char **argv, char **in, char **out)
{
    int ch;
    optind = 0;

    while ((ch = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (ch) {
        case 'c':
            /* Skip. See harry_load_config(). */
            break;
        case 'i':
            config_set_string(&cfg, "input.input_format", optarg);
            break;
        case 1000:
            config_set_bool(&cfg, "input.decode_str", CONFIG_TRUE);
            break;
        case 1001:
            config_set_bool(&cfg, "input.reverse_str", CONFIG_TRUE);
            break;
        case 1002:
            config_set_string(&cfg, "input.stopword_file", optarg);
            break;
        case 'o':
            config_set_string(&cfg, "output.output_format", optarg);
            break;
        case 'm':
            config_set_string(&cfg, "measures.measure", optarg);
            break;
        case 'd':
            config_set_string(&cfg, "measures.word_delim", optarg);
            break;
        case 'n':
            config_set_int(&cfg, "measures.num_threads", atoi(optarg));
            break;
        case 'a':
            config_set_int(&cfg, "measures.cache_size", atoi(optarg));
            break;
        case 'g':
            config_set_bool(&cfg, "measures.global_cache", CONFIG_TRUE);
            break;
        case 'z':
            config_set_bool(&cfg, "output.compress", CONFIG_TRUE);
            break;
        case 'u':
            config_set_bool(&cfg, "output.triangular", CONFIG_TRUE);
            break;
        case 'q':
            verbose = 0;
            log_line = 0;
            break;
        case 'l':
            log_line = 1;
            break;
        case 'v':
            verbose++;
            break;
        case 'M':
            measure_print();
            exit(EXIT_SUCCESS);
            break;
        case 'D':
            print_config("Default configuration");
            exit(EXIT_SUCCESS);
            break;
        case 'C':
            print_conf = 1;
            break;
        case 'V':
            print_version();
            exit(EXIT_SUCCESS);
            break;
        case 'h':
        case '?':
            print_usage();
            exit(EXIT_SUCCESS);
            break;
        }
    }

    /* Check configuration */
    if (!config_check(&cfg)) {
        exit(EXIT_FAILURE);
    }

    /* We are through with parsing. Print the config if requested */
    if (print_conf) {
        print_config("Current configuration");
        exit(EXIT_SUCCESS);
    }

    argc -= optind;
    argv += optind;

    /* Check for input and output arguments */
    if (argc != 2) {
        print_usage();
        exit(EXIT_FAILURE);
    } else {
        *in = argv[0];
        *out = argv[1];
    }

    /* Check for stdin and stdout "filenames" */
    if (!strcmp(*in, "-"))
        config_set_string(&cfg, "input.input_format", "stdin");
    if (!strcmp(*out, "-"))
        config_set_string(&cfg, "output.output_format", "stdout");
}


/**
 * Load the configuration of Harry
 * @param argc number of arguments
 * @param argv arguments
 */
static void harry_load_config(int argc, char **argv)
{
    char *cfg_file = NULL;
    int ch;

    /* Check for config file in command line */
    while ((ch = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (ch) {
        case 'c':
            cfg_file = optarg;
            break;
        case '?':
            print_usage();
            exit(EXIT_SUCCESS);
            break;
        default:
            /* empty */
            break;
        }
    }

    /* Init and load configuration */
    config_init(&cfg);

    if (cfg_file != NULL) {
        if (config_read_file(&cfg, cfg_file) != CONFIG_TRUE)
            fatal("Could not read configuration (%s in line %d)",
                  config_error_text(&cfg), config_error_line(&cfg));
    }

    /* Check configuration and set defaults */
    if (!config_check(&cfg)) {
        exit(EXIT_FAILURE);
    }

}

/**
 * Init the Harry tool
 * @param argc number of arguments
 * @param argv arguments
 */
static void harry_init()
{
    const char *cfg_str;
    int nthreads;

    if (verbose > 1)
        config_print(&cfg);

    /* Init value cache */
    vcache_init();

    /* Configure module (init as first) */
    config_lookup_string(&cfg, "measures.measure", &cfg_str);
    measure = measure_config(cfg_str);

    config_lookup_int(&cfg, "measures.num_threads", &nthreads);
    if (nthreads > 0)
        omp_set_num_threads(nthreads);

    /* Load stop words */
    config_lookup_string(&cfg, "input.stopword_file", &cfg_str);
    if (strlen(cfg_str) > 0)
        stopwords_load(cfg_str);
}

/**
 * Read a set of strings to memory from input
 * @param input Input filename 
 * @param num Pointer to number of strings
 * @return array of string objects
 */
static hstring_t *harry_read(char *input, int *num)
{
    const char *cfg_str;
    int i, chunk, read;
    hstring_t *strs = NULL;

    /* Get chunk size */
    config_lookup_int(&cfg, "input.chunk_size", &chunk);

    /* Open input */
    config_lookup_string(&cfg, "input.input_format", &cfg_str);
    info_msg(1, "Opening input '%0.40s' [%s].", input, cfg_str);
    input_config(cfg_str);
    if (!input_open(input))
        fatal("Could not open input source");

    info_msg(1, "Reading strings in chunks of %d", chunk);
    for (*num = 0, read = chunk; read == chunk; *num += read) {
        /* Allocate memory for strings */
        strs = realloc(strs, (*num + chunk) * sizeof(hstring_t));
        if (!strs)
            fatal("Could not allocate memory for strings");

        /* Read chunk */
        read = input_read(strs + *num, chunk);
    }

    /* Close input */
    input_close();

    /* Symbolize strings if requested */
    for (i = 0; i < *num; i++)
        strs[i] = hstring_preproc(strs[i]);

    return strs;
}

/**
 * Compare a set of string objects
 * @param strs Array of string objects
 * @param num Number of strings
 * @return Matrix of similarity valurs
 */
static hmatrix_t *harry_compute(hstring_t *strs, int num)
{
    hmatrix_t *mat = hmatrix_init(strs, num);

    if (!hmatrix_alloc(mat))
        fatal("Could not allocate matrix for similarity measure");

    /* Compute matrix */
    info_msg(1, "Computing similarity measure '%s' with %d threads.",
             measure, omp_get_max_threads());
    hmatrix_compute(mat, strs, measure_compare);

    return mat;
}

/**
 * Write similarity values to an output file
 * @param output Output filename
 * @param mat Matrix of similarity values
 */
static void harry_write(char *output, hmatrix_t *mat)
{
    const char *cfg_str;

    /* Open output */
    config_lookup_string(&cfg, "output.output_format", &cfg_str);
    output_config(cfg_str);
    info_msg(1, "Writing %ld similarity values to '%0.40s' [%s].",
             mat->size, output, cfg_str);
    if (!output_open(output))
        fatal("Could not open output destination");

    output_write(mat);
    output_close();
}


/**
 * Exit Harry tool. 
 */
static void harry_exit(hstring_t *strs, hmatrix_t *mat, int num)
{
    const char *cfg_str;

    /* Free memory */
    input_free(strs, num);
    free(strs);

    /* Destroy matrix */
    hmatrix_destroy(mat);

    config_lookup_string(&cfg, "input.stopword_file", &cfg_str);
    if (strlen(cfg_str) > 0)
        stopwords_destroy();

    /* Destroy value cache */
    vcache_destroy();

    /* Destroy configuration */
    config_destroy(&cfg);
}

/**
 * Main function of Harry tool 
 * @param argc Number of arguments
 * @param argv Argument values
 * @return exit code
 */
int main(int argc, char **argv)
{
    hmatrix_t *mat = NULL;
    hstring_t *strs = NULL;
    int num = 0;
    char *input = NULL;
    char *output = NULL;

    harry_load_config(argc, argv);
    harry_parse_options(argc, argv, &input, &output);

    harry_init();
    strs = harry_read(input, &num);
    mat = harry_compute(strs, num);
    harry_write(output, mat);
    harry_exit(strs, mat, num);

    return EXIT_SUCCESS;
}
