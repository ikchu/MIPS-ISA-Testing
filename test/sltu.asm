# Statement Test: sltu
# set-less-than-unsigned with positive/negative vals and <, >, == cases for each
main:
      li    $t0, 1
      li    $t1, 2
      li    $t2, -1 # FFFF
      li    $t3, -2 # FFFE
      sltu  $t4, $t0, $t1
      sltu  $t5, $t0, $t0 
      sltu  $t6, $t0, $t2
      sltu  $t7, $t3, $t1 
      sltu  $t8, $t3, $t2
      sltu  $t9, $t2, $t2  
      .word 0xfeedfeed
