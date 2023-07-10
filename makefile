tlb: tlb.c
	gcc tlb.c -o tlb -lm 
run:
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_small_addr_range 0

clean:
	rm -rf tlb && clear

demo:
	gcc demo.c -o demo -pthread && clear && ./demo

ddemo:
	rm -rf demo && clear