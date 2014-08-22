CC = gcc
DEBUG = -g
RELEASE = -Ofast
CFLAGS = $(DEBUG) -Wall -Werror

LIBS = general.h move.h
COMMON = state_type.o $(LIBS)
OBJS = main.o letter_bag_type.o rail_type.o state_type.o letter_frequency.o input.o rule_checker.o scorer.o tile_drawer.o trie.o
EXE = scrabble
DRIVER = driver
DRIVEROBJS = driver.o trie.o

all: $(EXE) $(DRIVER)

$(EXE): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)
$(DRIVER): $(DRIVEROBJS)
	$(CC) $(CFLAGS) -o $(DRIVER) $(DRIVEROBJS)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
bag_type.o: letter_bag_type.h letter_bag_type.c $(LIBS)
	$(CC) $(CFLAGS) -c letter_bag_type.c
rail_type.o: rail_type.h rail_type.c $(LIBS)
	$(CC) $(CFLAGS) -c rail_type.c
state_type.o: state_type.h state_type.c $(LIBS)
	$(CC) $(CFLAGS) -c state_type.c
letter_frequency.o: letter_frequency.h letter_frequency.c $(LIBS)
	$(CC) $(CFLAGS) -c letter_frequency.c
input.o: input.h input.c $(COMMON)
	$(CC) $(CFLAGS) -c input.c
rule_checker.o: rule_checker.h rule_checker.c $(COMMON)
	$(CC) $(CFLAGS) -c rule_checker.c
scorer.o: scorer.h scorer.c $(LIBS)
	$(CC) $(CFLAGS) -c scorer.c
tile_drawer.o: tile_drawer.h tile_drawer.c
	$(CC) $(CFLAGS) -c tile_drawer.c
trie.o: trie.h trie.c
	$(CC) $(CFLAGS) -c trie.c
driver.o: driver.c
	$(CC) $(CFLAGS) -c driver.c
.PHONY: clean
clean:
	rm -rf $(OBJS) $(EXE)
