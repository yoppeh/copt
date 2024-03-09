/*
 * copt.c
 *
 * copt implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "copt.h"

extern copt_t *__start_copt;
extern copt_t *__stop_copt;

copt_error_t copt_parse(int argc, char **argv, void *user_data) {
    copt_error_t err = {COPT_OK, NULL};
    copt_t **options = &__start_copt;
    for (int i = 1; i < argc; i++) {
        char *equal = strchr(argv[i], '=');
        if (equal != NULL) {
            *equal = '\0';
        }
        int len = strlen(argv[i]);
        int j;
        for (j = 0; options[j] != NULL; j++) {
            if (options[j]->option != NULL) {
                int len2 = strlen(options[j]->option);
                if (len2 > len) {
                    len = len2;
                }
                if (strncmp(options[j]->option, argv[i], len) == 0) {
                    options[j]->present = true;
                    if (options[j]->type == COPT_ARG_REQUIRED) {
                        if (equal != NULL) {
                            options[j]->value = equal + 1;
                        } else {
                            if (i + 1 < argc) {
                                options[j]->value = argv[i + 1];
                            } else {
                                err.code = COPT_MISSING_ARGUMENT;
                                err.option = options[j]->option;
                                return err;
                            }
                        }
                    } else if (options[j]->type == COPT_ARG_OPTIONAL) {
                        if (equal != NULL) {
                            options[j]->value = equal + 1;
                        } else {
                            options[j]->value = NULL;
                        }
                    } else if (options[j]->type == COPT_ARG_NONE) {
                        if (equal != NULL) {
                            err.code = COPT_UNEXPECTED_ASSIGNMENT;
                            err.option = options[j]->option;
                            return err;
                        }
                    }
                    if (options[j]->validate != NULL) {
                        int ret = options[j]->validate(options[j], user_data);
                        if (ret != 0) {
                            err.code = COPT_OK;
                            err.option = NULL;
                            return err;
                        }
                    }
                    break;
                }
            }
        }
        if (options[j] == NULL) {
            err.code = COPT_UNRECOGNIZED_OPTION;
            err.option = argv[i];
            return err;
        }
    }
    for (int i = 0; &options[i] < &__stop_copt; i++) {
        if (!options[i]->present) {
            if (options[i]->missing != NULL) {
                int ret = options[i]->missing(options[i], user_data);
                if (ret != 0) {
                    err.code = ret;
                    err.option = options[i]->option;
                    return err;
                }
            }
        }
    }
    return err;
}

void copt_show_syntax(void) {
    copt_t **options = &__start_copt;
    int opt_field_width = 0;
    int arg_field_width = 0;
    char *opt = NULL;
    for (int i = 0; &options[i] < &__stop_copt; i++) {
        if (options[i]->option != NULL) {
            int l = strlen(options[i]->option);
            if (options[i]->type != COPT_ARG_NONE) {
                l += 7;
            }
            if (l > opt_field_width) {
                opt_field_width = l;
            }
        }
    }
    opt_field_width += 5;
    opt = malloc(opt_field_width + 1);
    if (opt == NULL) {
        return;
    }
    for (int i = 0; &options[i] < &__stop_copt; i++) {
        if (options[i]->option != NULL) {
            int l = strlen(options[i]->option);
            memset(opt, '.', opt_field_width);
            memcpy(opt, options[i]->option, l);
            if (options[i]->type != COPT_ARG_NONE) {
                opt[l] = '=';
                if (options[i]->type == COPT_ARG_OPTIONAL) {
                    opt[l + 1] = '[';
                } else if (options[i]->type == COPT_ARG_REQUIRED) {
                    opt[l + 1] = '<';
                }
                opt[l + 2] = 'a';
                opt[l + 3] = 'r';
                opt[l + 4] = 'g';
                if (options[i]->type == COPT_ARG_OPTIONAL) {
                    opt[l + 5] = ']';
                } else if (options[i]->type == COPT_ARG_REQUIRED) {
                    opt[l + 5] = '>';
                }
                l += 6;
                opt[l++] = ' ';
            } else {
                opt[l] = ' ';
            }
            opt[opt_field_width - 2] = ' ';
            opt[opt_field_width - 1] = '\0';
            printf("%s%s\n", opt, options[i]->description);
        } else if (options[i]->description != NULL) {
            printf("%s\n", options[i]->description);
        }
    }
    free(opt);
}
