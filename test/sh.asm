# Statement Test: sw
# store word to memory. check mem indexing, sign extend, and byte-slicing
main:
      li    $t0, 32
      li    $t1, 0x12345678
      sh    $t1, -8($t0)
      .word 0xfeedfeed
      .word 0x00000000
      .word 0x00000000 # currently located at M[24] (after converting to ELF)
