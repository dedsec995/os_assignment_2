tlb: tlb.c
	gcc tlb.c -o tlb -lm 
run:
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_small_addr_range 0 20 > test.txt

clean:
	rm -rf tlb && rm -rf test.txt && clear 

demo: demo.c
	gcc demo.c -o demo -lm && ./demo

ddemo:
	rm -rf demo && clear
