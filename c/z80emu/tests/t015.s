;;
;; TEST: EX AF, AF'
;;    
;;
.area _DATA
.area _CODE
LD A, #0xFA  ; A= 0xFA
EX AF, AF'   ; A'=0xFA
LD A, #0x33  ; A= 0x33
EX AF, AF'   ; A=0xFA, A'=0x33
HALT
