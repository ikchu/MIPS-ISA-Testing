# Statement Test: ori
# perform bitwise or with an immediate: two positive numbers, two negative numbers, positive/negative, negative/positive
main:
      li    $t0, 24904
      ori   $t1, $t0, 20184
      li    $t2, 49584 # (-15952)
      ori   $t3, $t2, 46355 # (-19181)
      li    $t4, 2809
      ori   $t5, $t4, 53360 # (-12176)
      li    $t6, 57370 # (-8166)
      ori   $t7, $t6, 5908
      .word 0xfeedfeed
