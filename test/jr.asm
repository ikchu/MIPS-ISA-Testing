# Statement Test: jr
# perform jump register
main: 
      li    $t0, 16
      jr    $t0
      li    $t1, 1            # shouldn't happen
end:
      li    $t2, 2
      .word 0xfeedfeed
