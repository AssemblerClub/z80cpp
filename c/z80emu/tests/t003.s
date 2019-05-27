.area _DATA
.area _CODE
.rept 200
      LD H, #0x0A
      LD L, #0x10
      LD B, #0x3E
      LD C, #0x11
      LD D, #0x06
      LD (HL), B
      LD L, C
      LD (HL), C
      LD L, #0x12
      LD (HL), D
.endm