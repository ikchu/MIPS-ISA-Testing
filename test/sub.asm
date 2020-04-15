# Statement Test: sub/subu
# perform subtraction with registers: p-p=p, p-p=n, n-n=p, n-n=n, p-n, n-p
main: 
      li    $t0, 1
      li    $t1, 2
      li    $t2, -1
      li    $t3, -2
      sub   $t4, $t0, $t1
      sub   $t5, $t1, $t0
      sub   $t6, $t2, $t3
      sub   $t7, $t3, $t2
      sub   $t8, $t0, $t3
      sub   $t9, $t3, $t0
      .word 0xfeedfeed
