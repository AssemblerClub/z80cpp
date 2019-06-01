;;
;; TEST: EXX
;;    
;;
.area _DATA
.area _CODE
LD  A, #0xFF
LD BC, #0x1122
LD DE, #0x3344
LD HL, #0x5566 
EXX             
LD BC, #0xAABB
LD DE, #0xCCDD
LD HL, #0xEEFF
EXX               ; A=0xFF, BC =0x1122, DE =0x3344, HL =0x5566
                  ;         BC'=0xAABB, DE'=0xCCDD, HL'=0xEEFF
HALT
