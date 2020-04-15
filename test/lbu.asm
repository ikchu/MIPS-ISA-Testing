# Statement Test: lbu
# load byte from memory. check mem indexing, sign extend, and byte-slicing
main:
      li    $t0, 4
      lbu   $t1, 12($t0)
      .word 0xfeedfeed
      .word 0x12345678 # currently located at M[16] (after converting to ELF)
