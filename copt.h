/**
 * copt.h
 *
 * copt global declarations.
 */

#ifndef _COPT_H
#define _COPT_H

#include <stdbool.h>

/**
 * @brief Define an array of options in a special segment so that multiple 
 * option arrays will be merged into one by the linker.
 */
#define copt_options_t __attribute__((section(".copt"), used)) copt_t *

/**
 * @brief Error codes.
 */
typedef enum copt_error_code_t {
    /** @brief No error. */
    COPT_OK,
    /** @brief An unrecognized option was passed on the command line. */
    COPT_UNRECOGNIZED_OPTION,
    /** @brief An assignment was found when no argument was expected. */
    COPT_UNEXPECTED_ASSIGNMENT,
    /** @brief A required argument is missing. */
    COPT_MISSING_ARGUMENT,
    /** @brief An option is missing. */
    COPT_MISSING_OPTION,
    /** @brief An application error. This value and values greater than this 
     * cab be returned by the validate and missing callback functions.
    */
    COPT_APPLICATION_ERROR
} copt_error_code_t;

/**
 * @brief An error return object. code is the copt_error_code_t and option is
 * the option the argv string caused the error.
 */
typedef struct copt_error_t {
    /** @brief copt_error_code_t describing the error. */
    copt_error_code_t code;
    /** @brief the command line option that caused the error. */
    const char *option;
} copt_error_t;

/**
 * @brief Argument types.
 */
typedef enum copt_arg_t {
    /** @brief An argument is not expected. */
    COPT_ARG_NONE,
    /** @brief An argument is required. */
    COPT_ARG_REQUIRED,
    /** @brief An argument may be passed, but isn't required. */
    COPT_ARG_OPTIONAL
} copt_arg_t;

/**
 * @brief The definition of a single option item.
 */
typedef struct copt_t {
    /** @brief The option. may be NULL to use the description in 
     * copt_show_syntax() */
    const char *option;
    /** @brief Option description text. Used by copt_show_syntax(). */
    const char *description;
    /** @brief The argument passed to the option. Initialize this to NULL. */
    const char *value; 
    /** @brief Callback function to validate an option and argument. May be
     * NULL. */
    copt_error_code_t (*validate)(struct copt_t *option, void *user_data);
    /** @brief Callback function that is called if an option is not given on 
     * the command line. May be NULL. */
    copt_error_code_t (*missing)(struct copt_t *option, void *user_data);
    /** @brief The argument type: COPT_ARG_NONE, COPT_ARG_REQUIRED, 
     * COPT_ARG_OPTIONAL. */
    copt_arg_t type;
    /** @brief Initialize to false. Will be set to true if the option is given
     * on the command line. */
    bool present;
} copt_t;

/**
 * @brief parse the command line using the coalesced option description arrays.
 * 
 * @param argc command line argument count, as passed to main().
 * @param argv array of command line argument strings, as passed to main().
 * @param user_data user data passed to callback functions, unused by copt.
 * @return copt_error_t error code and option string.
 */
extern copt_error_t copt_parse(int argc, char **argv, void *user_data);

/**
 * @brief shows the command line option syntax.
 */
extern void copt_show_syntax(void);

#endif // _COPT_H
