# copt

I recently wrote an [AI program that](https://github.com/yoppeh/chewie) that can submit queries to multiple different AI providers. Each backend is defined in its own c module that conforms to a common interface. I wanted these modules to be able to have their own command line options, since some options might only make sense for a particular backend. It seemed like it would be best if those options were defined in the c module to which they belonged.

For [chewie](https://github.com/yoppeh/chewie), I implemented this such that each module defines an array of NULL-terminated option specifications that are merged together before all options are processed. I wanted a more efficient way of doing this. I realize it doesn't matter so much, the time to do the merge and process the arguments isn't critical, but it still bugged me. copt is my solution. It uses the same option data structures and algorithm, but it allows options to be defined in multiple modules and merged together during link time, not at runtime.

This is accomplished by putting the different option arrays in a special section. The section layout is defined in `copt.ld` and should be included in the linker command with `-Tcopt.ld`. `copt.h` exports a `copt_options_t` macro for declaring the array of options in the appropriate section:

```C
copt_options_t options[] = {
    &opt_a,
    &opt_b,
    &opt_c
};
```

`copt_t`, which is the type for each individual option element, doesn't need to be defined in a special section:

```C
copt_t opt_a = {
    .option = "-a",
    .description = "option a",
    .type = COPT_ARG_NONE,
    .value = NULL,
    .present = false,
    .validate = validate_a
};

copt_t opt_b = {
    .option = "b",
    .description = "option b",
    .type = COPT_ARG_REQUIRED,
    .value = NULL,
    .present = false,
    .validate = validate_b
};

copt_t opt_c = {
    .option = "c",
    .description = "option c",
    .type = COPT_ARG_OPTIONAL,
    .value = NULL,
    .present = false,
    .validate = validate_c,
    .missing = missing_c
};
```

The `copt_options_t` type is defined with a `used` attribute so that the data is not optimized away if `-flto` is used. During link-time, two variables `__start_copt` and `__stop_copt` define the start and stop of the merged options array. This isn't of concern to the application, so much, but it is used internally by `copt.c` to access the merged array of options.

The `copt_t` has the following structure:

```C
typedef struct copt_t {
    const char *option;
    const char *description;
    const char *value;
    copt_error_code_t (*validate)(struct copt_t *option, void *user_data);
    copt_error_code_t (*missing)(struct copt_t *option, void *user_data);
    copt_arg_t type;
    bool present;
} copt_t;
```

The `option` field is the option specification. It can be NULL, in which case the `copt_t` element will not be used for option processing, but its `description` will contribute to the option help output.

`description` is the explanatory text that will be printed for this particular option.

`value` should be initialized to `NULL`. It will be set to the value passed on the command line for the option, if any.

`validate` is an optional function that will be called when this particular option is encountered. It returns a `copt_error_code_t`. If the return code is anything other than `COPT_OK`, option parsing will stop and `copt_parse()` will return the error code.

`missing` is an optional function that will be called if this particular option is not given on the command line. It must return a `copt_error_code_t` that is returned immediately to the caller of `copt_parse()` if it is any code other than `COPT_OK`.

`type` is the argument type for this particular option:

- `COPT_ARG_NONE` for no argument taken.
- `COPT_ARG_REQUIRED` should be used if an argument is required for this option.
- `COPT_ARG_OPTIONAL` should be used if an argument can be present, but isn't required.
- `COPT_APPLICATION_ERROR` the application can return this value or above to stop processing and return the code to the application.

`present` should be initialized to `false`. It will be set to `true` only if the option was given on the command line.

Once all of the data structures are defined, command line parsing is initiated simply by called the `copt_parse()` function:

`copt_error_t copt_parse(int argc, char **argv, void *user_data);`

Simply pass it the argc and argv parameters that are passed to `main()`, along with an optional `user_data` value that is passed along to `validate()` and `missing()`. `user_data` is otherwise unused by `copt` and is present solely for the convenience of the application.

If the application needs to display command line option help, the `copt_show_syntax()` function can be used. It will iterate through all options, in the order the linker places them in the final executable, and displays the `option` value (if not `NULL`) along with the `description` value (if not `NULL`).
