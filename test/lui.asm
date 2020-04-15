# Statement Test: lui
# load upper immediate (load immediate into upper 16 bits of reg)
main:
      lui   $t0, 0xffff
      lui   $t1, 0x1234
      .word 0xfeedfeed
