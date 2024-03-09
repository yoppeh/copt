/**
 * main.c
 * 
 *  test/demo
 */

#include <stdio.h>
#include <stdlib.h>

#include "copt.h"

static copt_error_code_t validate_a(copt_t *option, void *user_data);
static copt_error_code_t validate_b(copt_t *option, void *user_data);
static copt_error_code_t missing_c(copt_t *option, void *user_data);
static copt_error_code_t validate_c(copt_t *option, void *user_data);

copt_t opt_a = {
    .option = "-a",
    .description = "option a",
    .type = COPT_ARG_NONE,
    .value = NULL,
    .present = false,
    .validate = validate_a
};

copt_t opt_b = {
    .option = "-b",
    .description = "option b",
    .type = COPT_ARG_REQUIRED,
    .value = NULL,
    .present = false,
    .validate = validate_b
};

copt_t opt_c = {
    .option = "-c",
    .description = "option c",
    .type = COPT_ARG_OPTIONAL,
    .value = NULL,
    .present = false,
    .validate = validate_c,
    .missing = missing_c
};

copt_t opt_divider = {
    .option = NULL,
    .description = "more options:",
    .type = COPT_ARG_NONE,
    .value = NULL,
    .present = false,
    .validate = NULL
};

copt_t opt_do = {
    .option = "do",
    .description = "option do",
    .type = COPT_ARG_NONE,
    .value = NULL,
    .present = false,
    .validate = NULL
};

copt_t opt_re = {
    .option = "re",
    .description = "option re",
    .type = COPT_ARG_OPTIONAL,
    .value = NULL,
    .present = false,
    .validate = NULL
};

copt_t opt_me = {
    .option = "me",
    .description = "option me",
    .type = COPT_ARG_REQUIRED,
    .value = NULL,
    .present = false,
    .validate = NULL
};

copt_options_t options[] = {
    &opt_a,
    &opt_b,
    &opt_c,
    &opt_divider,
    &opt_do,
    &opt_re,
    &opt_me
};

int main(int ac, char **av) {
    copt_show_syntax();
    copt_error_t ret = copt_parse(ac, av, NULL);
    switch (ret.code) {
        case COPT_UNRECOGNIZED_OPTION:
            fprintf(stderr, "unrecognized option %s\n", ret.option);
            break;
        case COPT_UNEXPECTED_ASSIGNMENT:
            fprintf(stderr, "unexpected assignment for option %s\n", ret.option);
            break;
        case COPT_MISSING_ARGUMENT:
            fprintf(stderr, "missing argument for option %s\n", ret.option);
            break;
        case COPT_MISSING_OPTION:
            fprintf(stderr, "missing required option %s\n", ret.option);
            break;
        default:
            break;
    }
    if (ret.code != COPT_OK) {
        return 1;
    }
    return 0;
}

static copt_error_code_t validate_a(copt_t *option, void *user_data) {
    printf("got option a\n");
    return COPT_OK;
}

static copt_error_code_t validate_b(copt_t *option, void *user_data) {
    printf("got option b with value %s\n", option->value);
    return COPT_OK;
}

static copt_error_code_t missing_c(copt_t *option, void *user_data) {
    return COPT_MISSING_OPTION;
}

static copt_error_code_t validate_c(copt_t *option, void *user_data) {
    if (option->value != NULL) {
        printf("got option c with value %s\n", option->value);
    } else {
        printf("got option c with no value\n");
    }
    return COPT_OK;
}
