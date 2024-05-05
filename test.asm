    LD A, 55
    CP 56 
    JP C, equal
    LD A, 0
    OUT (1), A
    HALT
equal:
    LD A, 1
    OUT (1), A
    HALT
    

