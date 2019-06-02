;;
;; TEST: INC rp
;;    
;;
.area _DATA
.area _CODE
LD BC, #0x00FF
LD DE, #0x0111
LD HL, #0xFFFF
LD SP, #0xAAAA
INC BC            ; BC = 0x0100
INC DE            ; DE = 0x0112
INC HL            ; HL = 0x0000
INC SP            ; SP = 0xAAAB
HALT
