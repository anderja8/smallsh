# Program name: SMALLSH
# Author: Jacob Anderson
# Date: 02/09/2020
# Description: Makefile for the smallsh project. Makes my life easier as I compile over and over again.

# This makefile is inspired by the example makefile from Oregon State.
# Available at: https://oregonstate.instructure.com/courses/1692879/files/71792854

# Which compiler to use
CXX = gcc

# Which flags to pas to compiler
CXXFLAGS = -std=gnu99
CXXFLAGS += -Wall
CXXFLAGS += -pedantic-errors

# Variable name for my objects
OBJS = backgroundNode.o builtInCommands.o helperFunctions.o smallsh.o

# Variable name for my sources (.cpp)
SRCS = backgroundNode.c builtInCommands.c helperFunctions.c smallsh.c

# Variable name for my headers (.hpp)
HEADERS = backgroundNode.h builtInCommands.h helperFunctions.h

# Command to compile my executable from my objects
Project4_Anderson_Jacob: ${OBJS} ${HEADERS}
	${CXX} ${OBJS} -o smallsh

# Command to compile my objects from my sources
${OBJS}: ${SRCS}
	${CXX} ${CXXFLAGS} -c $(@:.o=.c)

# Command to clean the files
clean:
	rm ${OBJS} smallsh
