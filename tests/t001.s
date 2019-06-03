;;
;; TEST: BASIC 8-bit REGISTER LOADING
;;    It loads 8-bit registers with data
;;
.area _DATA
.area _CODE
LD A, #0x11
LD B, A
LD A, #0x22
LD C, A
LD H, #0x33
LD L, #0x44
LD A, #0x00
LD H, A
LD L, A
LD D, #0x55
LD E, #0x66
LD A, D      ; A=0x55, BC=0x1122, DE=0x5566, HL=0x0000
HALT
