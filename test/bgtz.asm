# Statement Test: bgtz
# perform branch if greater than zero instruction
main: 
      li $t0, 10
      bgtz $t0, end3
end2:
      li $t8, 123
end3:
      li $t8, 1
      .word 0xfeedfeed
