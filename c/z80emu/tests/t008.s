;;
;; TEST: LD a, (bc) / LD (bc), a
;;    
;;
.area _DATA
.area _CODE
LD BC, #0x0000    ; BC = 0x0000
LD  A, (BC)       ;  A = 0x01
LD  C, A          ;  C = 0x01
LD (BC), A        ; (0x0001) = 0x01
HALT
