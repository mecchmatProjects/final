# set compiler
CC = gcc

# set compilation flags
CFLAGS   = -Wall

# set linker
LINKER = gcc

# set linking flags
LFLAGS = -Wall

# header files directory
INCDIR = include

# server source files directory
SRVDIR = server

# client source files directory
CLTDIR = client

# common source file directory
CMNDIR = common

# object files directory
OBJDIR = obj

# executable for server  directory
BINSRV = server

# executable for client  directory
BINCLT = client

# define rm as rm --force
rm  = rm -f

# all targets
all: $(OBJDIR)/common.o $(OBJDIR)/tftp_server.o $(OBJDIR)/tftp_client.o $(BINSRV)/tftp_server $(BINCLT)/tftp_client

# compile common source files
$(OBJDIR)/common.o: $(CMNDIR)/common.c
	@$(CC) $(CFLAGS) -c $^ -o $@
	@echo "Compiled "$^" successfully."

# compile TFTP Server source files
$(OBJDIR)/tftp_server.o: $(SRVDIR)/tftpserver.c
	@$(CC) $(CFLAGS) -c $^ -o $@
	@echo "Compiled "$^" successfully."

# compile TFTP Client source files
$(OBJDIR)/tftp_client.o: $(CLTDIR)/tftpclient.c
	@$(CC) $(CFLAGS) -c $^ -o $@
	@echo "Compiled "$^" successfully."

# link TFTP Server object files
$(BINSRV)/tftp_server: $(OBJDIR)/tftp_server.o $(OBJDIR)/common.o
	@$(LINKER) $^ $(LFLAGS) -o $@
	@echo "Linking "$^" completed."

# link TFTP Client object files
$(BINCLT)/tftp_client: $(OBJDIR)/tftp_client.o $(OBJDIR)/common.o
	@$(LINKER) $^ $(LFLAGS) -o $@
	@echo "Linking "$^" completed."

# clean up utility
clean:
	@$(rm) $(OBJDIR)/tftp_server.o $(OBJDIR)/tftp_client.o $(OBJDIR)/common.o
	@$(rm) $(BINSRV)/tftp_server $(BINCLT)/tftp_client
	@echo "Cleanup completed."

