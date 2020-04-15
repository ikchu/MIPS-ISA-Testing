# Statement Test: blez
# perform break-less-equal-zero: ensure that blez jumps when it should and doesn't when it shouldn't
main:
      li    $t0, -1
      li    $t1, 4294967295   # same 32-bit as -1
      li    $t2, 1
      blez  $t0, large
      li    $t3, 3            # shouldn't happen (-1 <= 0)
large:
      blez  $t1, end
      li    $t4, 4            # shouldn't happen ((int)4294967295 == -1 <= 0)
zero:
      blez  $zero, end
      li    $t5, 5            # shouldn't happen (0 <= 0)
end:
      li    $t6, 6
      blez  $t2, wrong
      .word 0xfeedfeed
wrong:
      li    $t7, 7            # shouldn't happen (1 not <= 0)
      .word 0xfeedfeed
