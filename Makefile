# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -g3 -O0

# Fichiers sources
SRC_DIR = src
TRACKER_DIR = $(SRC_DIR)/Tracker
SRCS = $(TRACKER_DIR)/handle_tracker.c
OBJS = $(SRCS:.c=.o)


# Fichiers Java
JAVADIR = $(SRC_DIR)/Peer
JAVAC = javac
JAVA = java
JAVASRCS = $(wildcard $(JAVADIR)/*.java)
JAVACLASSPATH = $(JAVADIR)

# Cibles

all: tracker-objs peer

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tracker-objs : $(OBJS)

peer: $(JAVASRCS) $(wildcard $(JAVADIR)/test/*.java)
	$(JAVAC) -d $(JAVADIR) -cp $(JAVACLASSPATH) $(JAVASRCS) $(wildcard $(JAVADIR)/test/*.java)
clean:
	rm -f $(OBJS) handle_tracker
	rm -rf $(JAVADIR)/*.class
	rm -rf $(JAVADIR)/test/*.class
	rm -rf $(JAVADIR)/project
	rm -rf tracker tstHandlegetfile tstHandleLook
run-tracker: tracker
	./tracker

run-peer: peer
	$(JAVA) -cp $(JAVACLASSPATH) Peer

test: peer
	$(JAVA) -cp $(JAVACLASSPATH) test.testINIReader
	$(JAVA) -cp $(JAVACLASSPATH) test.TestPeer

test-peer: peer
	$(JAVA) -cp $(JAVACLASSPATH) test.TestPeer

test-args: peer 
	$(JAVA) -cp $(JAVACLASSPATH) $(ARGS)
test-config: peer
	$(JAVA)  -cp $(JAVACLASSPATH) project.Peer $(JAVADIR)/config.ini $(JAVADIR)/peer1filepath/file1.txt  $(JAVADIR)/peer1filepath/file2.txt 
test-tracker : 
	$(CC) $(CFLAGS) -o tstHandlegetfile tst/tstHandlegetfile.c  $(OBJS)
	$(CC) $(CFLAGS) -o tstHandleLook tst/tstHandlelook.c  $(OBJS)
	./tstHandlegetfile
	./tstHandleLook
tracker :
	$(CC) $(CFLAGS) -o tracker $(TRACKER_DIR)/tracker.c  $(OBJS)
	./tracker
.PHONY: all clean run-tracker run-peer test test-peer