# Statement Test: add/addu
# perform addition with registers: two positive numbers, two negative numbers, and one positive and one negative number
main: 
      li    $t0, 1
      li    $t1, -2
      add   $t2, $t0, $t0
      add   $t3, $t0, $t1
      add   $t4, $t1, $t1
      .word 0xfeedfeed
