;;
;; TEST: LD a, (de) / LD (de), a
;;    
;;
.area _DATA
.area _CODE
LD DE, #0x0000    ; DE = 0x0000
LD  A, (DE)       ;  A = 0x11
LD  E, A          ;  E = 0x11
LD (DE), A        ; (0x0011) = 0x11
HALT
