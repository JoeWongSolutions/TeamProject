OBJS = teamproject.o MU_HashTable.o MU_HashTableSimpleBiDi.o 
CC = gcc  
CFLAGS = -Wall -O -g 

teamproject : $(OBJS)  
	$(CC) $(OBJS) -o teamproject  

teamproject.o : teamproject.c MU_HashTable.h MU_HashTableSimpleBiDi.h 
	$(CC) $(CFLAGS) -c teamproject.c -o teamproject.o  

MU_HashTable.o : MU_HashTable.c MU_HashTable.h MU_HashTableSimpleBiDi.h 
	$(CC) $(CFLAGS) -c MU_HashTable.c -o MU_HashTable.o  

MU_HashTableSimpleBiDi.o : MU_HashTableSimpleBiDi.c MU_HashTable.h MU_HashTableSimpleBiDi.h 
	$(CC) $(CFLAGS) -c MU_HashTableSimpleBiDi.c -o MU_HashTableSimpleBiDi.o  

clean:  
	rm -rf *.o teamproject
