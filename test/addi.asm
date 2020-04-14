# Statement Test: addi
# perform addition with immediates: two positive numbers, two negative numbers, and one positive and one negative number
main: 
      li $t0, 100
      addi $t2, $t0, 12 
      li $t4, 10
      addi $t4, $t4, -5
      li $t7, -23
      addi $t8, $t7, -23
      .word 0xfeedfeed
