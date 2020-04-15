# Statement Test: bgtz
# perform break-greater--zero: ensure that bgtz jumps when it should and doesn't when it shouldn't
main:
      li    $t0, 1
      li    $t1, -1
      bgtz  $t0, zero
      li    $t2, 2            # shouldn't happen (1 > 0)
zero:
      bgtz  $zero, wrong
      li    $t3, 3            
end:
      li    $t4, 4
      bgtz  $t1, wrong
      .word 0xfeedfeed
wrong:
      li    $t5, 5            # shouldn't happen (-1 not > 0)
      .word 0xfeedfeed
