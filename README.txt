1.To compile type 'make -B' and  press Enter

2.To run first program type './a' and press Enter
To run second program type './b' and press Enter

3.The content of files:

5_glodomorow.c contains my solution to the Dining Philosophers Problem. The program uses Inter Process Communication(IPC).

Short Describtion of the Project:

There are 5 processes which present 5 Philosophers who are seated around a circural table. Between each 2 philosophers there is one fork. A Philosopher can't grab 2 forks simultaneously.To eat his meal, a philosopher needs to obtain 2 forks, so there is a possibility of the Deadlock. My program solves it by introducing additional process - one that grants the permission to alocate the recources.

port.c contains my solution to the Port Synchronization Problem. The program uses POSIX libary.

Short Describtion of the Project:

Thera are n ships present in the program, each presented by the thread. They have diffrent size and weigh, so they requre diffrent amount of resources to enter or leave the port. The port has a limited number of free docks and they cannot be shared. My program solves it by introducing another thread that synchronizes the allocation of the the recources and prevents the deadlock.

Bush did 911