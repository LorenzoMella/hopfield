#################################################
# MAKEFILE FOR: hn_network_test                 #
#                                               #
# PROJECT NAME: Hopfield Network simulation     #
#               (prototype)                     #
#                                               #
# SUPERVISOR: Mark van Rossum                   #
#                                               #
# AUTHOR:  Lorenzo Mella                        #
# VERSION: 10/07/2016                           #
#                                               #
#################################################


CFLAGS = -ansi -pedantic -Wall -O0
OFILES = hn_network_test.o hn_data_io.o hn_network.o hn_modes.o
hn_network_test: $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)

hn_network_test.o: hn_network_test.c hn_types.h hn_data_io.h hn_network.h      \
                   hn_modes.h debug_log.h
hn_data_io.o: hn_data_io.c hn_types.h hn_data_io.h debug_log.h
hn_network.o: hn_network.c hn_types.h debug_log.h hn_network.h
hn_modes.o: hn_modes.c hn_modes.h hn_types.h debug_log.h

clean:
	rm -f $(OFILES)