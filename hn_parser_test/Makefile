#################################################
# MAKEFILE FOR: hn_parser_test                  #
#                                               #
# PROJECT NAME: Hopfield Network simulation     #
#               (prototype)                     #
#                                               #
# SUPERVISOR: Mark van Rossum                   #
#                                               #
# AUTHOR:  Lorenzo Mella                        #
# VERSION: 30/06/2016                           #
#                                               #
#################################################

CFLAGS = -pedantic -Wall
CC = clang

hn_parser_test: hn_parser_test.o hn_parser.o
	$(CC) $(CFLAGS) -o hn_parser_test hn_parser_test.o hn_parser.o

hn_parser_test.o: hn_parser_test.c
hn_parser.o: hn_parser.c

clean:
	rm *.o