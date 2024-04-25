gcc -o clock clk.c 
gcc -o process process.c 
gcc -o process_generator process_generator.c 
gcc -o sched scheduler.c -lm
gcc -o test_generator test_generator.c 


./process_generator
