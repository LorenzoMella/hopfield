#################################################
# MAKEFILE FOR: hn_basic_simulation             #
#                                               #
# PROJECT NAME: Hopfield Network simulation     #
#               (prototype)                     #
#                                               #
# SUPERVISOR: Mark van Rossum                   #
#                                               #
# AUTHOR:  Lorenzo Mella                        #
# VERSION: 07/08/2016                           #
#                                               #
#################################################

CFLAGS = -Wall -pedantic -O0
PROF = # This can be defined as -pg on the command line if required
OFILES = hn_basic_simulation.o ../hn_network.o ../hn_modes.o ../hn_data_io.o ../hn_parser.o

hn_basic_simulation: $(OFILES)
	$(CC) -o $@ $(CFLAGS) $(PROF) $(OFILES)

hn_basic_simulation.o: hn_basic_simulation.c ../hn_types.h \
 ../hn_macro_utils.h ../../debug_log/debug_log.h ../hn_data_io.h \
 ../hn_types.h ../hn_network.h ../hn_modes.h ../hn_parser.h \
 ../../debug_log/debug_log.h
hn_network.o: ../hn_network.c ../../debug_log/debug_log.h \
 ../hn_macro_utils.h ../hn_network.h ../hn_types.h
hn_modes.o: ../hn_modes.c ../hn_types.h ../hn_macro_utils.h \
 ../../debug_log/debug_log.h ../hn_modes.h
hn_data_io.o: ../hn_data_io.c ../../debug_log/debug_log.h ../hn_data_io.h \
 ../hn_types.h
hn_parser.o: ../hn_parser.c ../hn_parser.h ../hn_types.h \
 ../../debug_log/debug_log.h
