;;
;; TEST: EX (SP), HL
;;    
;;
.area _DATA
.area _CODE
LD HL, #0xAABB    
LD SP, #0x0008    
EX (SP),HL        
HALT              ; SP=0x08, HL=0x1122, (0x08)=0xBBAA
.dw #0x1122