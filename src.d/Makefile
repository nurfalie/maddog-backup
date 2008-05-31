INSTALL = install

standard:	add_file \
		logout_user \
		lookup_user \
		delete_files \
		display_files

include ./Makefile.tmp

OPTIONS		=	-Wall -pedantic $(LDEFINES)
GCC		=	gcc

INCLUDES		=	common.h
ADD_FILE_SRC		=	add_file.c
LOGOUT_USER_SRC		=	logout_user.c
LOOKUP_USER_SRC		=	lookup_user.c
DELETE_FILES_SRC	=	delete_files.c
DISPLAY_FILES_SRC	=	display_files.c

INC		=	-I/usr/include -I/usr/local/include
LIB		=

add_file:	$(INCLUDES) $(ADD_FILE_SRC)
		$(GCC) $(OPTIONS) $(INC) -o add_file \
		$(ADD_FILE_SRC) $(LIB)

logout_user:	$(INCLUDES) $(LOGOUT_USER_SRC)
		$(GCC) $(OPTIONS) $(INC) -o logout_user \
		$(LOGOUT_USER_SRC) $(LIB)

lookup_user:	$(INCLUDES) $(LOOKUP_USER_SRC)
		$(GCC) $(OPTIONS) $(INC) -o lookup_user \
		$(LOOKUP_USER_SRC) $(LIB)

delete_files:	$(INCLUDES) $(DELETE_FILES_SRC)
		$(GCC) $(OPTIONS) $(INC) -o delete_files \
		$(DELETE_FILES_SRC) $(LIB)

display_files:	$(INCLUDES) $(DISPLAY_FILES_SRC)
		$(GCC) $(OPTIONS) $(INC) -o display_files \
		$(DISPLAY_FILES_SRC) $(LIB)

lint:	$(LOOKUP_USER_SRC) $(DISPLAY_FILES_SRC)
	lint $(INC) $>

clean:
	rm -f add_file
	rm -f logout_user
	rm -f lookup_user
	rm -f delete_files
	rm -f display_files
	rm -f *.cgi

purge:
	rm -f *~
