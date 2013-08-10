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
#include "default.h"
#include "util.h"
#include "input.h"
#include "measures.h"
#include "output.h"

/* Global variables */
int verbose = 1;
int print_conf = 0;
config_t cfg;

/* Local variables */
static char *input = NULL;
static char *output = NULL;
static str_t *strs = NULL;
static long num = 0;

/* Option string */
#define OPTSTRING       "t:c:i:o:d:vqVhCD"

/**
 * Array of options of getopt_long()
 */
static struct option longopts[] = {
    {"input_format", 1, NULL, 'i'},
    {"decode_str", 1, NULL, 1000},
    {"reverse_str", 1, NULL, 1001},
    {"stopword_file", 1, NULL, 1002},   /* <- last entry */
    {"output_format", 1, NULL, 'o'},
    {"type", 1, NULL, 't'},
    {"delim", 1, NULL, 'd'},
    {"config_file", 1, NULL, 'c'},
    {"verbose", 0, NULL, 'v'},
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
           "  -i,  --input_format <format>   Set input format for strs.\n"
           "       --decode_str <0|1>        Enable URI-decoding of strs.\n"
           "       --reverse_str <0|1>       Reverse (flip) all strs.\n"
           "       --stopword_file <file>    Provide a file with stop words.\n"
           "  -o,  --output_format <format>  Set output format for vectors.\n"
           "\nModule options:\n"
           "  -t,  --type <name>             Set similarity measure module\n"
           "  -d   --delim <delimiters>      Set delimiters for words\n"
           "\nGeneric options:\n"
           "  -c,  --config_file <file>      Set configuration file.\n"
           "  -v,  --verbose                 Increase verbosity.\n"
           "  -q,  --quiet                   Be quiet during processing.\n"
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
 */
static void harry_parse_options(int argc, char **argv)
{
    int ch, user_conf = FALSE;

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
            config_set_int(&cfg, "input.decode_str", atoi(optarg));
            break;
        case 1001:
            config_set_int(&cfg, "input.reverse_str", atoi(optarg));
            break;
        case 1002:
            config_set_string(&cfg, "input.stopword_file", optarg);
            break;
        case 'o':
            config_set_string(&cfg, "output.output_format", optarg);
            break;
        case 't':
            config_set_string(&cfg, "measures.type", optarg);
            break;
        case 'd':
            config_set_string(&cfg, "measures.delim", optarg);
            break;
        case 'q':
            verbose = 0;
            break;
        case 'v':
            verbose++;
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
        input = argv[0];
        output = argv[1];
    }

    /* Last but not least. Warn about default config */
    if (!user_conf) {
        warning("No config file given. Using defaults (see -D)");
    }
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

    if (verbose > 1)
        config_print(&cfg);

    /* Configure module (init as first) */
    config_lookup_string(&cfg, "measures.type", &cfg_str);
    measure_config(cfg_str);
    info_msg(1, "Configuring similarity measure '%s'.", cfg_str);

    /* Load stop words */
    config_lookup_string(&cfg, "input.stopword_file", &cfg_str);
    if (strlen(cfg_str) > 0)
        stopwords_load(cfg_str);

    /* Open input */
    config_lookup_string(&cfg, "input.input_format", &cfg_str);
    input_config(cfg_str);
    info_msg(1, "Opening '%0.40s' with input module '%s'.", input, cfg_str);
    num = input_open(input);
    if (num < 0)
        fatal("Could not open input source");

    /* Allocate memory for strs */
    strs = calloc(num, sizeof(str_t));
    if (!strs)
        fatal("Could not allocate memory for strs");

    /* Open output */
    config_lookup_string(&cfg, "output.output_format", &cfg_str);
    output_config(cfg_str);
    info_msg(1, "Opening '%0.40s' with output module '%s'.", output, cfg_str);
    if (!output_open(output))
        fatal("Could not open output destination");
}

static void harry_load()
{
    long read;
    int i;

    read = input_read(strs, num);
    if (read <= 0)
        fatal("Failed to read strs from input '%s'", input);
        
    for (i = 0; i < num; i++) {
        strs[i] = str_symbolize(strs[i]);
        str_print(strs[i]);
    }
}

static void harry_save()
{
}

/**
 * Exit Harry tool. 
 */
static void harry_exit()
{
    const char *cfg_str;

    info_msg(1, "Flushing. Closing input and output.");
    input_close();
    output_close();
    
    /* Free memory */
    input_free(strs, num);
    free(strs);
    
    config_lookup_string(&cfg, "input.stopword_file", &cfg_str);
    if (strlen(cfg_str) > 0)
        stopwords_destroy();

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
    harry_load_config(argc, argv);
    harry_parse_options(argc, argv);

    harry_init();
    harry_load();

    harry_save();
    harry_exit();
    return EXIT_SUCCESS;
}
