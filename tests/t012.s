;;
;; TEST: LD sp, hl
;;    
;;
.area _DATA
.area _CODE
LD HL, #0xAABB    ; HL = 0xAABB
LD SP, HL         ; SP = 0xAABB
HALT
