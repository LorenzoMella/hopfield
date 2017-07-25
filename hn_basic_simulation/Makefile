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

CFLAGS = -ansi -Wall -pedantic -O0
PROF = # This can be defined as -pg on the command line if required
OFILES = hn_basic_simulation.o hn_network.o hn_modes.o hn_data_io.o hn_parser.o

hn_basic_simulation: $(OFILES)
	$(CC) -o $@ $(CFLAGS) $(PROF) $(OFILES)

hn_basic_simulation.o: hn_basic_simulation.c hn_types.h hn_macro_utils.h       \
  hn_data_io.h hn_network.h hn_modes.h hn_parser.h debug_log.h
hn_network.o: hn_network.c hn_types.h hn_macro_utils.h hn_network.h            \
  debug_log.h
hn_modes.o: hn_modes.c hn_types.h hn_macro_utils.h hn_modes.h debug_log.h
hn_data_io.o: hn_data_io.c hn_types.h hn_data_io.h debug_log.h
hn_parser.o: hn_parser.c hn_types.h hn_parser.h debug_log.h

clean:
	rm -f $(OFILES)