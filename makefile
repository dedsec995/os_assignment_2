tlb: tlb.c
	gcc tlb.c -o tlb -lm 
run:
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_small_addr_range 2 20 > test.txt

clean:
	rm -rf tlb && rm -rf test.txt && clear 

demo: demo.c
	gcc demo.c -o demo -lm && ./demo

ddemo:
	rm -rf demo && clear

makeall:
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_small_addr_range 0 20 > zzfifo0.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_large_addr_range 0 20 > zzfifo1.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_small_addr_range 0 20 > zzfifo2.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_large_addr_range 0 20 > zzfifo3.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_small_addr_range 1 20 > zzrand0.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_large_addr_range 1 20 > zzrand1.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_small_addr_range 1 20 > zzrand2.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_large_addr_range 1 20 > zzrand3.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_small_addr_range 2 20 > zzclock0.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/30_large_addr_range 2 20 > zzclock1.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_small_addr_range 2 20 > zzclock2.txt
	gcc tlb.c -o tlb -lm && ./tlb traces_and_answers/practice_files/10k_large_addr_range 2 20 > zzclock3.txt