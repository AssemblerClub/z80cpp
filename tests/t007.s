;;
;; TEST: LD (**), hl / LD (*), a
;;    
;;
.area _DATA
.area _CODE
LD HL, #0xAABB    ; HL = 0xAABB
LD  A, #0xCC      ;  A = 0xCC
LD (#0x10), HL    ; (0x0010) = 0xBBAA
LD (#0x12), A     ; (0x0012) = 0xCC
HALT
