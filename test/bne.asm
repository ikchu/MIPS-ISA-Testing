# Statement Test: bne
# perform break-not-equals: ensure that bne jumps when it should and doesn't when it shouldn't
main:
      li    $t0, 4294967295   # same 32-bit as -1
      li    $t1, 2
      li    $t2, -1
      bne   $t0, $t1, end
      li    $t3, 3            # shouldn't happen
end:
      li    $t4, 4
      bne   $t0, $t2, wrong
      .word 0xfeedfeed
wrong:
      li    $t5, 5            # shouldn't happen
      .word 0xfeedfeed
