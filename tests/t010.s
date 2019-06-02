;;
;; TEST: LD hl, (**) / LD (**), hl
;;    
;;
.area _DATA
.area _CODE
LD  A, #0xFF      ;  A = 0xFF
LD HL, (#0x0000)  ; HL = 0xFF3E
LD (#0x10), HL    ; (0x10) = 0x3EFF
HALT
