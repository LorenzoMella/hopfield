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
OFILES = hn_parser_test.o ../hn_parser.o

hn_parser_test: $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)

hn_parser_test.o: hn_parser_test.c ../../debug_log/debug_log.h \
 ../hn_parser.h ../hn_types.h ../hn_types.h
hn_parser.o: ../hn_parser.c ../hn_types.h ../hn_parser.h \
 ../../debug_log/debug_log.h

clean:
	rm -f *.o
