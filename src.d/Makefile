INSTALL = install

all:	add_file \
	delete_files \
	display_files \
	logout_user \
	lookup_user

-include ./Makefile.tmp

GCC		=	gcc
GCC_OPTIONS	=	-Wall -Werror -Wextra -Wstack-protector \
			-fPIE -fstack-protector-all \
	                -pedantic -pie $(LDEFINES)
INC		=	-I/usr/include -I/usr/local/include
INCLUDES	=	common.h
LIB		=

ADD_FILE_SRC		=	add_file.c
DELETE_FILES_SRC	=	delete_files.c
DISPLAY_FILES_SRC	=	display_files.c
LOGOUT_USER_SRC		=	logout_user.c
LOOKUP_USER_SRC		=	lookup_user.c

add_file:	$(INCLUDES) $(ADD_FILE_SRC)
		$(GCC) $(GCC_OPTIONS) $(INC) -o add_file \
		$(ADD_FILE_SRC) $(LIB)

delete_files:	$(INCLUDES) $(DELETE_FILES_SRC)
		$(GCC) $(GCC_OPTIONS) $(INC) -o delete_files \
		$(DELETE_FILES_SRC) $(LIB)

display_files:	$(INCLUDES) $(DISPLAY_FILES_SRC)
		$(GCC) $(GCC_OPTIONS) $(INC) -o display_files \
		$(DISPLAY_FILES_SRC) $(LIB)

logout_user:	$(INCLUDES) $(LOGOUT_USER_SRC)
		$(GCC) $(GCC_OPTIONS) $(INC) -o logout_user \
		$(LOGOUT_USER_SRC) $(LIB)

lookup_user:	$(INCLUDES) $(LOOKUP_USER_SRC)
		$(GCC) $(GCC_OPTIONS) $(INC) -o lookup_user \
		$(LOOKUP_USER_SRC) $(LIB)

clean:
	rm -f add_file
	rm -f delete_files
	rm -f display_files
	rm -f logout_user
	rm -f lookup_user
	rm -f *.cgi

distclean: clean
	rm -f Makefile.tmp

purge:
	rm -f *~
