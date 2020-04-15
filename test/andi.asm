# Statement Test: andi
# perform bitwise and with an immediate: two positive numbers, two negative numbers, positive/negative, negative/positive
main:
      li    $t0, 32768
      andi  $t1, $t0, 28940
      li    $t2, -127
      andi  $t3, $t2, 60196 #(-5340)
      li    $t4, 28661
      andi  $t5, $t4, 44029 #(-21507)
      li    $t6, -9314
      andi  $t7, $t6, 27011
      .word 0xfeedfeed
