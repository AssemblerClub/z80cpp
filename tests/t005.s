;;
;; HALT TEST
;;    Should stop at HALT and start issuing NOPS, not reaching LD A, #0x33
;;
.area _DATA
.area _CODE
LD A, #0x77
HALT
LD A, #0x33
