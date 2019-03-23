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


CFLAGS = -pedantic -Wall -O0

OBJS = hn_data_io_test.o ../hn_data_io.o ../hn_network.o 
test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

hn_data_io_test.o: hn_data_io_test.c ../../debug_log/debug_log.h \
 ../hn_types.h ../hn_data_io.h ../hn_types.h ../hn_network.h
hn_data_io.o: ../hn_data_io.c ../../debug_log/debug_log.h ../hn_data_io.h \
 ../hn_types.h
hn_network.o: ../hn_network.c ../../debug_log/debug_log.h \
 ../hn_macro_utils.h ../hn_network.h ../hn_types.h
