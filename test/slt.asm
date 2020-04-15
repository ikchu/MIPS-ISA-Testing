# Statement Test: slt
# set-less-than with positive/negative vals and <, >, == cases for each
main:
      li    $t0, 1
      li    $t1, 2
      li    $t2, -1
      li    $t3, -2
      slt   $t4, $t0, $t1
      slt   $t5, $t0, $t0 
      slt   $t6, $t0, $t2 
      slt   $t7, $t3, $t1
      slt   $t8, $t3, $t2
      slt   $t9, $t2, $t2
      .word 0xfeedfeed
