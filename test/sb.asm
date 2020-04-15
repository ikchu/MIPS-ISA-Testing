# Statement Test: sb
# store byte to memory. check mem indexing, sign extend, and byte-slicing
main:
      li    $t0, 8
      li    $t1, 0x12345678
      sb    $t1, 12($t0)
      .word 0xfeedfeed
      .word 0x00000000 # currently located at M[20] (after converting to ELF)
