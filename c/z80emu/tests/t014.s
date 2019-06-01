;;
;; TEST: DEC rp
;;    
;;
.area _DATA
.area _CODE
LD BC, #0x0100
LD DE, #0x0112
LD HL, #0x0000
LD SP, #0xAAAB
DEC BC            ; BC = 0x00FF
DEC DE            ; DE = 0x0111
DEC HL            ; HL = 0xFFFF
DEC SP            ; SP = 0xAAAA
HALT
