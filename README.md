# line_processor
Title: Multi-threaded Producer Consumer Pipeline

A program with a pipeline of 4 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user or fileand puts it in a buffer it shares with the next thread in the pipeline.
- Line separator thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It removes the line separator '\n' from 
  the value and replaces it with a space, and places value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Plus sign thread is the third thread in the pipeline. It consumes items from the buffer it shares with the line separator thread. It finds '++' in the item and replaces with '^', 
  and places value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the fourth thread in the pipeline. It consumes items from the buffer it shares with the plus sign thread and prints the items (only if 80 characters per line)
Source code for producer consumer threads adapted from https://replit.com/@cs344/65prodconspipelinec (example CS344 code for this assignment)

Compile and run:
1. In terminal type command "make [enter]"
2. This compiles main.c into line_processor, runs input1.txt and outputs to output1.txt
3. To change which file line_processor runs on, navigate to makefile and update input1.txt to any of the .txt file within the directory. Or create your own .txt to test.
  "./line_processor < [input1.txt] > output1.txt" 
