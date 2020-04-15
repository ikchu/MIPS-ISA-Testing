# Statement Test: slti
# set-less-than-imm with positive/negative imms and <, >, == cases for each
main:
      li    $t0, 2
      slti  $t1, $t0, -1
      slti  $t2, $t0, 1
      slti  $t3, $t0, 2
      slti  $t4, $t0, 3
      li    $t5, -2
      slti  $t6, $t5, -3
      slti  $t7, $t5, -2
      slti  $t8, $t5, -1
      slti  $t9, $t5, 1
      .word 0xfeedfeed
