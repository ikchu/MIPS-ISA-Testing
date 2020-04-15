# Statement Test: and
# perform bitwise and with registers: two positive numbers, two negative numbers, and one positive and one negative number
main: 
      li    $t0, 16241 
      li    $t1, 14346
      li    $t2, -26231
      li    $t3, -30444
      and   $t4, $t0, $t1
      and   $t5, $t0, $t2
      and   $t6, $t2, $t3
      .word 0xfeedfeed
