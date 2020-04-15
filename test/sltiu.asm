# Statement Test: sltiu
# set-less-than-imm-unsigned with positive/negative imms and <, >, == cases for each
main:
      li    $t0, 2
      sltiu $t1, $t0, -1
      sltiu $t2, $t0, 1
      sltiu $t3, $t0, 2
      sltiu $t4, $t0, 3
      li    $t5, -2
      sltiu $t6, $t5, -3
      sltiu $t7, $t5, -2
      sltiu $t8, $t5, -1
      sltiu $t9, $t5, 1
      .word 0xfeedfeed
