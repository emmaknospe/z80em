;; GOAL: get this to run (insertion sort)
    ld hl, array
    push hl
    pop de
    ld a, (arrayLength)
    ld b, a
    inc hl ; start hl pointing at array + 1
elementLoop:
    ld a, (hl)
    push hl
compareLoop:
    dec hl
    sbc hl, de
    jp z, insertAtEnd
    adc hl, de
    cp (hl)
    jp nc, insert
    ld c, a
    ld a, (hl)
    inc hl
    ld (hl), a
    dec hl 
    ld a, c
    jp compareLoop
insertAtEnd:
    adc hl, de
insert:
    ld (hl), a
    pop hl
    inc hl
    dec b
    jp nz, elementLoop
    ld hl, arrayLength
    ld a, (hl)
    ld b, a
    inc hl
printLoop:
    ld a, (hl)
    out (0), a
    dec b
    jp nz, printLoop 
    halt
arrayLength:
    .BYTE 8
array:
    .BYTE 5
    .BYTE 9
    .BYTE 7
    .BYTE 3
    .BYTE 13
    .BYTE 11
    .BYTE 8
    .BYTE 4

