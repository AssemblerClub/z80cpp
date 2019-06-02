;;
;; TEST LD (bc), a / LD (de), a
;;    
;;
.area _DATA
.area _CODE
LD BC, #0x0010    ; BC = 0x0010
LD DE, #0x0011    ; DE = 0x0011
LD  A, #0xCC      ;  A = 0xCC
LD (BC), A        ; (0x0010) = 0xCC
LD  A, #0xDD      ;  A = 0xDD
LD (DE), A        ; (0x0011) = 0xDD
HALT
