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


CFLAGS = -std=c11 -pedantic -Wall -O2 -lm
OFILES = hn_data_io.o hn_network.o hn_modes.o

all: capacity_test time_complexity

capacity_test: capacity_test.o $(OFILES)
	$(CC) -o $@ $(CFLAGS) capacity_test.o $(OFILES)

time_complexity: time_complexity.o $(OFILES)
	$(CC) -o $@ $(CFLAGS) $(PROF) time_complexity.o $(OFILES)

capacity_test.o: capacity_test.c ../debug_log/debug_log.h hn_types.h \
  hn_data_io.h hn_macro_utils.h hn_modes.h hn_network.h
hn_data_io.o: hn_data_io.c ../debug_log/debug_log.h hn_data_io.h \
  hn_types.h
hn_modes.o: hn_modes.c hn_types.h hn_macro_utils.h \
  ../debug_log/debug_log.h hn_modes.h
hn_network.o: hn_network.c ../debug_log/debug_log.h hn_macro_utils.h \
  hn_network.h hn_types.h
hn_parser.o: hn_parser.c hn_parser.h hn_types.h hn_macro_utils.h \
  ../debug_log/debug_log.h
small_dim_test.o: small_dim_test.c hn_types.h hn_macro_utils.h \
  ../debug_log/debug_log.h hn_modes.h hn_network.h hn_data_io.h
time_complexity.o: time_complexity.c hn_types.h hn_macro_utils.h \
  ../debug_log/debug_log.h hn_data_io.h hn_modes.h hn_network.h
time_complexity_nested.o: time_complexity_nested.c \
  ../debug_log/debug_log.h hn_types.h hn_macro_utils.h hn_data_io.h \
  hn_modes.h hn_network.h

clean:
	rm -f *.o
