#################################################
# MAKEFILE                                      #
#                                               #
# PROJECT NAME: Hopfield Network Simulation     #
#               (prototype)                     #
#                                               #
# SUPERVISOR: Mark van Rossum                   #
#                                               #
# AUTHOR:  Lorenzo Mella                        #
#                                               #
#################################################


# Remove the comment in CFLAGS to activate the logger (expect a lot of text on screen)
CFLAGS = -std=c11 -g -pedantic -Wall -O2 # -D DEBUG_LOG
LDFLAGS = -lm

OFILES = hn_data_io.o hn_network.o hn_modes.o hn_parser.o

all: capacity_test time_complexity hn_basic_simulation


capacity_test: capacity_test.o $(OFILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

time_complexity: time_complexity.o $(OFILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

hn_basic_simulation: hn_basic_simulation/hn_basic_simulation.o $(OFILES)
	$(CC) -o hn_basic_simulation/$@ $(CFLAGS) $(LDFLAGS) $^


capacity_test.o: capacity_test.c debug_log.h hn_types.h \
  hn_data_io.h hn_macro_utils.h hn_modes.h hn_network.h

hn_data_io.o: hn_data_io.c debug_log.h hn_data_io.h \
  hn_types.h

hn_modes.o: hn_modes.c hn_types.h hn_macro_utils.h \
  debug_log.h hn_modes.h

hn_network.o: hn_network.c debug_log.h hn_macro_utils.h \
  hn_network.h hn_types.h

hn_parser.o: hn_parser.c hn_parser.h hn_types.h hn_macro_utils.h \
  debug_log.h

time_complexity.o: time_complexity.c hn_types.h hn_macro_utils.h \
  debug_log.h hn_data_io.h hn_modes.h hn_network.h

time_complexity_nested.o: time_complexity_nested.c \
  debug_log.h hn_types.h hn_macro_utils.h hn_data_io.h \
  hn_modes.h hn_network.h

hn_basic_simulation/hn_basic_simulation.o: hn_basic_simulation/hn_basic_simulation.c \
  hn_basic_simulation/../debug_log.h \
  hn_basic_simulation/../hn_types.h \
  hn_basic_simulation/../hn_macro_utils.h \
  hn_basic_simulation/../hn_data_io.h \
  hn_basic_simulation/../hn_network.h hn_basic_simulation/../hn_modes.h \
  hn_basic_simulation/../hn_parser.h


clean:
	rm -f $(OFILES)
