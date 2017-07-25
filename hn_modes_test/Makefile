#################################################
# MAKEFILE FOR: hn_modes_test                   #
#                                               #
# PROJECT NAME: Hopfield Network simulation     #
#               (prototype)                     #
#                                               #
# SUPERVISOR: Mark van Rossum                   #
#                                               #
# AUTHOR:  Lorenzo Mella                        #
# VERSION: 01/07/2016                           #
#                                               #
#################################################

CFLAGS = -pedantic -Wall
CC = clang

hn_modes_test: hn_modes_test.o hn_modes.o
	$(CC) $(CFLAGS) -o test hn_modes_test.o hn_modes.o

hn_modes_test.o: hn_modes_test.c
hn_modes.o: hn_modes.c

clean:
	rm *.o