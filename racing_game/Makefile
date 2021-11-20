#Makefile for racing-game

MYLIBDIR = ../lib
OBJS1 = server.o session_man.o setup_m_srv.o
OBJS2 = client.o session.o

all: s c
s: ${OBJS1} race.h
	cc -o s ${OBJS1} ${MYLIBDIR}/snet.a

c: ${OBJS2} race.h
	cc -o c ${OBJS2} ${MYLIBDIR}/snet.a -lcurses -ltermcap

clean: rm -f ${OBJS1} ${OBJS2} s c