;;
;; TEST: PUSH rp
;;    
;;
.area _DATA
.area _CODE
LD  A, #0xAA      ;; 0000 3E AA
LD BC, #0xBBCC    ;; 0002 01 CC BB
LD DE, #0xDDEE    ;; 0005 11 EE DD
LD HL, #0xFF77    ;; 0008 21 77 FF
LD SP, #0x0030    ;; 000B 31 30 00
PUSH HL           ;; 000E E5
PUSH DE           ;; 000F D5
PUSH BC           ;; 0010 C5
PUSH AF           ;; 0011 F5
HALT              ;; 0012 76

;; OUTPUT
;; A=0xAA, BC=0xBBCC, DE=0xDDEE, HL=0xFF77, SP=0x0028
;; (0x0028)=00 AA CC BB EE DD 77 FF 

