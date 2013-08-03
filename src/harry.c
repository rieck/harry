/*
 * Harry - Similarity Measures for Strings
 * Copyright (C) 2013 Konrad Rieck (konrad@mlsec.org)
 * --
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.  This program is distributed without any
 * warranty. See the GNU General Public License for more details. 
 */

#error alpha_dont_use_harry

#include "config.h"
#include "common.h"
#include "harry.h"
#include "default.h"
#include "util.h"
#include "measures.h"

/* Global variables */
int verbose = 1;
int print_conf = 0;
config_t cfg;

/* Option string */
#define OPTSTRING       "m:c:vqVhCD"

/**
 * Array of options of getopt_long()
 */
static struct option longopts[] = {
    {"module", 1, NULL, 'm'},
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
    printf("Usage: harry [options] [file ...]\n"
           "\nModule options:\n"
           "  -m,  --measure <name>          Set similarity measure.\n"
           "\nGeneric options:\n"
           "  -c,  --config_file <file>      Set configuration file.\n"
           "  -v,  --verbose                 Increase verbosity.\n"
           "  -q,  --quiet                   Be quiet during processing.\n"
           "  -C,  --print_config            Print the current configuration.\n"
           "  -D,  --print_defaults          Print the default configuration.\n"
           "  -V,  --version                 Print version and copyright.\n"
           "  -h,  --help                    Print this help screen.\n\n");
}

/**
 * Print version of Harry tool
 */
static void print_version(void)
{
    printf("Harry %s - Similarity Measures for Strings\n"
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
    int ch;

    optind = 0;

    while ((ch = getopt_long(argc, argv, OPTSTRING, longopts, NULL)) != -1) {
        switch (ch) {
        case 'c':
            /* Skip. See harry_load_config(). */
            break;
        case 'm':
            config_set_string(&cfg, "measures.default", optarg);
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
    const char *str;

    if (verbose > 1)
        config_print(&cfg);

    /* Configure module */
    config_lookup_string(&cfg, "measures.type", &str);
    measure_config(str);
    measure_init();
}

/**
 * Exit Harry tool. 
 */
static void harry_exit()
{
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

    /* AND... ACTION! */

    harry_exit();
    return EXIT_SUCCESS;
}
