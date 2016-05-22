Pipeline test:
System composed by ARM processor, RISCV Rocketchip and HW Accelerator (data complement).
ARM proc takes an image, writes it into buffer memory (DDR space reserved from OS),
set to 1 a polling variable; polls the variable until it goes to 0.
Rocketchip keeps polling the variable, until it goes to 1: it then starts HW acc
computation and waits until it ends. Sets variable to 0, exit.
ARM now sees variable to 0, takes image and writes into a file.