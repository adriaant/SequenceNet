LEVEL =

include $(LEVEL)makeinclude

#########################

EXEC = seqnet

OBJS = $(LO)*.o

# uncomment or change lines in this block to set up your own simulation executable
	#SRCS = Main.cpp MultiOnline.cpp
	SRCS = Main.cpp MultiOffline.cpp MultiSequence.cpp
	#SRCS = Main.cpp SingleOffline.cpp
	#SRCS = Main.cpp SingleOnline.cpp
# do not change other lines

$(EXEC): makeinclude
	@echo -- compiling:
	@for dir in $(SRCS); do\
		echo "    $$dir";\
		$(CC) $(OPTIONS) $(INCLUDE_DIR) -c $$dir;\
		mv *.o $(LO);\
	done
	@echo
	@echo -- making $(EXEC) --
	$(CC) -g $(OPTIONS) $(OBJS) $(LIBDIRS) $(LIBS) -o $@

clean:
	@echo -- cleaning objects and executable --
	-$(RM) $(OBJS) $(EXEC)
	@echo done

clock:
	$(TOUCH) Makefile
	$(TOUCH) makeinclude
	@echo

