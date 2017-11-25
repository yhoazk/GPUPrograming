# 

Micheal Abrahms


Tipical GPU programm:

1. CPU allocates storage on GPU (malloc)
2. CPU  copies input data from cpu -> GPU memcpy
3. CPU launches kernel(s) on the GPU to process the data (kernel lauch)
4. CPU copies results back to CPU from GPU (memcpy)

The main idea of GPU computations is to write a program as if it were 
to run on a single thread, but the GPU will run it on multiple threads.

Try to maximize the number of thread and the mumber of operations made
in a thread.


Thread: One independent path of execution through the code.


