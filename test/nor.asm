# Statement Test: nor
# perform bitwise nor with registers: two positive numbers, two negative numbers, and one positive and one negative number
main: 
      li    $t0, 4629 
      li    $t1, 24638
      li    $t2, -6496
      li    $t3, -13429
      nor   $t4, $t0, $t1
      nor   $t5, $t0, $t2
      nor   $t6, $t2, $t3
      .word 0xfeedfeed
