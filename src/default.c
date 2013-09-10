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

/** 
 * @defgroup default Configuration
 * Functions for configuration of the Harry tool. Additionally default
 * values for each configuration parameter are specified in this module.
 * @author Konrad Rieck (konrad@mlsec.org)
 * @{
 */

#include "config.h"
#include "common.h"
#include "util.h"
#include "default.h"

/* External variables */
extern int verbose;

/* Default configuration */
static config_default_t defaults[] = {
    {"input", "input_format", CONFIG_TYPE_STRING, {.str = "lines"}},
    {"input", "decode_str", CONFIG_TYPE_INT, {.num = 0}},
    {"input", "fasta_regex", CONFIG_TYPE_STRING, {.str = " (\\+|-)?[0-9]+"}},
    {"input", "lines_regex", CONFIG_TYPE_STRING, {.str = "^(\\+|-)?[0-9]+"}},
    {"input", "reverse_str", CONFIG_TYPE_INT, {.num = 0}},
    {"input", "stopword_file", CONFIG_TYPE_STRING, {.str = ""}},
    {"measures", "type", CONFIG_TYPE_STRING, {.str = "dist_levenshtein"}},
    {"measures", "delim", CONFIG_TYPE_STRING, {.str = ""}},
    {"measures", "cache_size", CONFIG_TYPE_INT, {.num = 256}},
    {"measures", "global_cache", CONFIG_TYPE_INT, {.num = 0}},    
    {"measures.dist_hamming", "norm", CONFIG_TYPE_STRING, {.str = "none"}},
    {"measures.dist_levenshtein", "norm", CONFIG_TYPE_STRING, {.str = "none"}},
    {"measures.dist_levenshtein", "cost_ins", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_levenshtein", "cost_del", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_levenshtein", "cost_sub", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_damerau", "norm", CONFIG_TYPE_STRING, {.str = "none"}},
    {"measures.dist_damerau", "cost_ins", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_damerau", "cost_del", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_damerau", "cost_sub", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_damerau", "cost_tra", CONFIG_TYPE_FLOAT, {.flt = 1.0}},
    {"measures.dist_jarowinkler", "scaling", CONFIG_TYPE_FLOAT, {.flt = 0.1}},
    {"measures.dist_lee", "alphabet", CONFIG_TYPE_INT, {.num = 256}},
    {"measures.dist_compression", "symmetric", CONFIG_TYPE_INT, {.num = 1}},
    {"measures.dist_compression", "level", CONFIG_TYPE_INT, {.num = 9}},
    {"measures.kern_wdegree", "degree", CONFIG_TYPE_INT, {.num = 3}},
    {"measures.kern_wdegree", "shift", CONFIG_TYPE_INT, {.num = 0}},
    {"measures.kern_subsequence", "length", CONFIG_TYPE_INT, {.num = 3}},
    {"measures.kern_subsequence", "lambda", CONFIG_TYPE_FLOAT, {.flt = 0.1}},
    {"output", "output_format", CONFIG_TYPE_STRING, {.str = "text"}},
    {"output", "compress", CONFIG_TYPE_INT, {.num = 0}},
    {NULL}
};

/**
 * Print a configuration setting. 
 * @param f File stream to print to
 * @param cs Configuration setting
 * @param d Current depth.
 */
static void config_setting_fprint(FILE *f, config_setting_t * cs, int d)
{
    assert(cs && d >= 0);

    int i;
    for (i = 0; i < d - 1; i++)
        fprintf(f, "       ");

    char *n = config_setting_name(cs);

    switch (config_setting_type(cs)) {
    case CONFIG_TYPE_GROUP:
        if (d > 0)
            fprintf(f, "%s = {\n", n);

        for (i = 0; i < config_setting_length(cs); i++)
            config_setting_fprint(f, config_setting_get_elem(cs, i), d + 1);

        if (d > 0) {
            for (i = 0; i < d - 1; i++)
                fprintf(f, "       ");
            fprintf(f, "};\n\n");
        }
        break;
    case CONFIG_TYPE_STRING:
        fprintf(f, "%s\t= \"%s\";\n", n, config_setting_get_string(cs));
        break;
    case CONFIG_TYPE_FLOAT:
        fprintf(f, "%s\t= %7.5f;\n", n, config_setting_get_float(cs));
        break;
    case CONFIG_TYPE_INT:
        fprintf(f, "%s\t= %d;\n", n, config_setting_get_int(cs));
        break;
    default:
        error("Unsupported type for configuration setting '%s'", n);
        break;
    }
}

/**
 * Print the configuration.
 * @param cfg configuration
 */
void config_print(config_t * cfg)
{
    config_setting_fprint(stdout, config_root_setting(cfg), 0);
}

/**
 * Print the configuration to a file. 
 * @param f pointer to file stream
 * @param cfg configuration
 */
void config_fprint(FILE *f, config_t * cfg)
{
    config_setting_fprint(f, config_root_setting(cfg), 0);
}

/**
 * The functions add default values to unspecified parameters.
 * @param cfg configuration
 */
static void config_default(config_t * cfg)
{
    int i, j;
    const char *s;
    double f;
    config_setting_t *cs = NULL, *vs;
    char *token, *string, *tofree;

    for (i = 0; defaults[i].name; i++) {
        /* Lookup and create setting group */
        tofree = string = strdup(defaults[i].group);
        vs = config_root_setting(cfg);
        while ((token = strsep(&string, ".")) != NULL) {
            cs = config_setting_get_member(vs, token);
            if (!cs)
                cs = config_setting_add(vs, token, CONFIG_TYPE_GROUP);
            vs = cs;
        }
        free(tofree);

        switch (defaults[i].type) {
        case CONFIG_TYPE_STRING:
            if (config_setting_lookup_string(cs, defaults[i].name, &s))
                continue;

            /* Add default value */
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_STRING);
            config_setting_set_string(vs, defaults[i].val.str);
            break;
        case CONFIG_TYPE_FLOAT:
            if (config_setting_lookup_float(cs, defaults[i].name, &f))
                continue;

            /* Check for mis-interpreted integer */
            if (config_setting_lookup_int(cs, defaults[i].name, &j)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name,
                                        CONFIG_TYPE_FLOAT);
                config_setting_set_float(vs, (double) j);
                continue;
            }

            /* Add default value */
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_FLOAT);
            config_setting_set_float(vs, defaults[i].val.flt);
            break;
        case CONFIG_TYPE_INT:
            if (config_setting_lookup_int(cs, defaults[i].name, &j))
                continue;

            /* Check for mis-interpreted float */
            if (config_setting_lookup_float(cs, defaults[i].name, &f)) {
                config_setting_remove(cs, defaults[i].name);
                vs = config_setting_add(cs, defaults[i].name,
                                        CONFIG_TYPE_INT);
                config_setting_set_int(vs, (long) round(f));
                continue;
            }

            /* Add default value */
            vs = config_setting_add(cs, defaults[i].name, CONFIG_TYPE_INT);
            config_setting_set_int(vs, defaults[i].val.num);
            break;
        }
    }
}

/**
 * Checks if the configuration is valid and sane. 
 * @return 1 if config is valid, 0 otherwise
 */
int config_check(config_t * cfg)
{
    /* Add default values where missing */
    config_default(cfg);

    /* TODO add sanity checks here */

    return 1;
}

/** @} */
