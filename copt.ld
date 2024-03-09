SECTIONS {
    .copt : {
        __start_copt = .;
        *(.copt);
        __stop_copt = .;
    }
}
INSERT AFTER .data