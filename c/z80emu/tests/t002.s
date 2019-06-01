;;
;; TEST: INDIRECT LOADING THROUGH HL
;;    It tests how to load some data and modify memory
;; using HL as indirect pointer.
;;
.area _DATA
.area _CODE
LD H, #0x00
LD L, #0x10
LD B, #0x3E
LD C, #0x11
LD D, #0x06
LD (HL), B
LD L, C
LD (HL), C
LD L, #0x12
LD (HL), D
HALT
HALT
HALT