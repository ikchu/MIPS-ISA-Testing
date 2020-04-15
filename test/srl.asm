# Statement Test: srl
# perform shift-right-logical. Note: srl isn't meant to preserve sign bit
main: 
      li    $t0, 0x80000000
      srl   $t1, $t0, 1 # 0x40000000
      srl   $t2, $t0, 5 # 0x04000000
      srl   $t3, $t0, 31 # 0x01
      srl   $t4, $t3, 1 # 0x00
      li    $t5, 0xFFFFFFFF
      srl   $t6, $t5, 1 # 0x7FFFFFFF
      srl   $t7, $t5, 5 # 0x07FFFFFF
      srl   $t8, $t5, 31 # 0x01
      srl   $t9, $t8, 1 # 0x00
      .word 0xfeedfeed
