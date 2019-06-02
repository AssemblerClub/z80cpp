;;
;; TEST: JR n
;;    
;;
.area _DATA
.area _CODE
LD A, #0x11
JR fw_1
LD A, #0x22
back_1:
LD DE, #0xddee
JR fw_2
LD DE, #0x8765
fw_1:
LD BC, #0xbbcc
JR back_1
LD BC, #0x4567
JR fw_2+3
fw_2:
LD HL, #0xf1f2
JR .              ; A=0x11, BC=0xBBCC, DE=0xDDEE, HL=0xf1f2