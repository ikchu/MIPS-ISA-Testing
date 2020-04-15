# Statement Test: lhu
# load half-word from memory. check mem indexing, sign extend, and half-slicing
main:
      li    $t0, 26
      lhu   $t1, -6($t0)
      .word 0xfeedfeed
      .word 0x00000000
      .word 0x12345678 # currently located at M[20] (after converting to ELF)
