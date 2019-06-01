;;
;; BASIC LD-16 bits test
;;    It test loading main 16 bits registers with data
;;
.area _DATA
.area _CODE
LD BC, #0xBBCC    ; BC = 0xBBCC
LD DE, #0xDDEE    ; DE = 0xDDEE
LD HL, #0x1122    ; HL = 0x1122
LD SP, #0x3344    ; SP = 0x3344
HALT