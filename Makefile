# This sample Makefile allows you to make any OpenGL application
#   whose source is exactly one .c file.
#
# To use this Makefile, you must type:
#
#        make PROG=xxxx
#                  ----
# where
#       xxxx.c is the name of the file you wish to compile.
#       ----

CC = gcc

CFLAGS = -g

INCLUDE = -I/usr/local/glut/include -I/usr/openwin/include -I/usr/local/X11/include

LDLIBS =  -lglut -lGLU -lGLw -lGL -lGLw -lXmu -lXext -lX11 -lm

LDFLAGS = -L/usr/local/glut/lib/glut -L/usr/openwin/lib -L/usr/local/X11/lib

executable:
	$(CC) $(INCLUDE) $(CFLAGS) $(PROG).c $(LDFLAGS) $(LDLIBS) -o $(PROG)
