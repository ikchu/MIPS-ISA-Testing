# Statement Test: sll
# perform shift-left-logical 
main: 
      li    $t0, 1
      sll   $t1, $t0, 1 # 0x02 (2)
      sll   $t2, $t0, 5 # 0x20 (-32)
      sll   $t3, $t0, 31 # 0x80000000 (2147483648)
      sll   $t4, $t3, 1 # 0x0 (0)
      li    $t5, 0xFFFFFFFF # (-1)
      sll   $t6, $t5, 1 # 0xFFFFFFFE (-2)
      sll   $t7, $t5, 5 # 0xFFFFFFE0 (-32)
      sll   $t8, $t5, 31 # 0x80000000 (2147483648)
      sll   $t9, $t8, 1 # 0x0 (0)
      .word 0xfeedfeed
