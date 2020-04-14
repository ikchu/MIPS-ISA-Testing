# Statement Test: j
# perform a jump instruction in a loop
      li   $t0, 0
      li   $t5, 2
      li   $t7, 16
loop: 
      beq  $t0, $t5, end
      addi $t0, $t0, 1
      j    loop
end: 
      .word 0xfeedfeed
