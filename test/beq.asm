# Statement Test: beq
# perform break-equals: ensure that beq jumps when it should and doesn't when it shouldn't
main:
      li    $t0, 4294967295   # same 32-bit as -1
      li    $t1, -1
      li    $t2, 2
      beq   $t0, $t1, end
      li    $t3, 3            # shouldn't happen
end:
      li    $t4, 4
      beq   $t0, $t2, wrong
      .word 0xfeedfeed
wrong:
      li    $t5, 5            # shouldn't happen
      .word 0xfeedfeed
