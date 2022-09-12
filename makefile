setup:
	gcc -std=gnu99 -w -pthread -o line_processor main.c
	./line_processor < input1.txt > output1.txt
clean:
	rm line_processor