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


CFLAGS = -pedantic -Wall -O0
OFILES = hn_network_test.o ../hn_data_io.o ../hn_network.o ../hn_modes.o
hn_network_test: $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES)


hn_network_test.o: hn_network_test.c ../hn_types.h ../hn_data_io.h \
 ../hn_types.h ../hn_network.h ../hn_modes.h ../hn_macro_utils.h \
 ../../debug_log/debug_log.h ../../debug_log/debug_log.h
hn_data_io.o: ../hn_data_io.c ../../debug_log/debug_log.h ../hn_data_io.h \
 ../hn_types.h
hn_network.o: ../hn_network.c ../../debug_log/debug_log.h \
 ../hn_macro_utils.h ../hn_network.h ../hn_types.h
hn_modes.o: ../hn_modes.c ../hn_types.h ../hn_macro_utils.h \
 ../../debug_log/debug_log.h ../hn_modes.h

clean:
	rm -f $(OFILES)
