gcc -o clock clk.c 
gcc -o process process.c 
gcc -o process_generator process_generator.c `pkg-config --cflags --libs gtk+-3.0`
gcc -o sched scheduler.c `pkg-config --cflags --libs gtk+-3.0` -lm
gcc -o test_generator test_generator.c 


./process_generator
