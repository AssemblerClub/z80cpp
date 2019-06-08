;;
;; TEST: POP rp
;;    
;;
.area _DATA
.area _CODE
LD   SP, #0x0008    ;; 0000 31 08 00
POP  BC             ;; 0003 C1
POP  DE             ;; 0004 D1
POP  HL             ;; 0005 E1
POP  AF             ;; 0006 F1
HALT                ;; 0007 76
.db #0xCC, #0xBB, #0xEE, #0xDD
.db #0xF2, #0xF1, #0x77, #0xAA

;; OUTPUT
;; AF=0xAA77, BC=0xBBCC, DE=0xDDEE, HL=0xF1F2, SP=0x0010

