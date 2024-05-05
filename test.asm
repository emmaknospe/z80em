    LD A, 56 
    CP 55 
    JP Z, equal
    LD A, 0
    OUT (1), A
    HALT
equal:
    LD A, 1
    OUT (1), A
    HALT

