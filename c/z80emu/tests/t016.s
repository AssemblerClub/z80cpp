;;
;; TEST: EX DE, HL
;;    
;;
.area _DATA
.area _CODE
LD HL, #0x1122 
LD DE, #0xCCDD
EX DE, HL         ; HL=0xCCDD, DE=0x1122
HALT
