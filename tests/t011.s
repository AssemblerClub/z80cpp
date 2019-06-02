;;
;; TEST: LD a, (**) / LD (**), a
;;    
;;
.area _DATA
.area _CODE
LD  A, (#0x00)    ;  A = 0x3A
LD (#0x01), A     ; (0x01) = 0x3A
HALT
