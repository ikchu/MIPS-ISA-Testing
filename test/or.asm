# Statement Test: or
# perform bitwise or with registers: two positive numbers, two negative numbers, and one positive and one negative number
main: 
      li    $t0, 21393 
      li    $t1, 27327
      li    $t2, -11602
      li    $t3, -16636
      or    $t4, $t0, $t1
      or    $t5, $t0, $t2
      or    $t6, $t2, $t3
      .word 0xfeedfeed
