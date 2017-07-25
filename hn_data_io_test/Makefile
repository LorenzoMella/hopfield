#################################################
# MAKEFILE FOR: hn_data_io_test                 #
#                                               #
# PROJECT NAME: Hopfield Network simulation     #
#               (prototype)                     #
#                                               #
# SUPERVISOR: Mark van Rossum                   #
#                                               #
# AUTHOR:  Lorenzo Mella                        #
# VERSION: 14/07/2016                           #
#                                               #
#################################################

CC = clang
CFLAGS = -pedantic -Wall

test: hn_data_io_test.o hn_data_io.o hn_network.o
	$(CC) $(CFLAGS) -o hn_data_io_test hn_data_io_test.o hn_data_io.o          \
                    hn_network.o

hn_data_io_test.o: hn_data_io_test.c hn_types.h hn_data_io.h hn_network.h
hn_network.o: hn_network.c hn_types.h hn_macro_utils.h hn_network.h            \
  debug_log.h
hn_data_io.o: hn_data_io.c hn_types.h hn_data_io.h debug_log.h
