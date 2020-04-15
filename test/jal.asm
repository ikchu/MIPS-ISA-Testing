# Statement Test: jal
# perform a jump instruction in a loop
main:
      li    $t0, 0
      li    $t5, 2
      li    $t7, 16
loop: 
      beq   $t0, $t5, end
      addi  $t0, $t0, 1
      jal   loop               # this is instruction 0x14. r[31] should be 0x1c
end: 
      .word 0xfeedfeed
