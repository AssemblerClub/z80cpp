;;
;; TEST: PUSH AF
;;    
;;
.area _DATA
.area _CODE
LD SP, #0x0020    ;; 0000 31 20 00
LD  A, #0x11      ;; 0003 3E 11
PUSH AF           ;; 0005 F5
HALT

;; OUTPUT
;; A=0x11, SP=0x001E, (0x001E)=00 11