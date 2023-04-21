ECHO OFF
ECHO *******************************************************************************
ECHO *                                  uC/OS-II
ECHO *                           The Real-Time Kernel
ECHO *
ECHO *          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
ECHO *                           All Rights Reserved
ECHO *
ECHO * Filename    : MAKETEST.BAT
ECHO * Description : Batch file to create the application.
ECHO * Output      : TEST.EXE will contain the DOS executable
ECHO * Usage       : MAKETEST
ECHO * Note(s)     : 1) This file assume that we use a MAKE utility.
ECHO *******************************************************************************
ECHO *
ECHO ON
CD    ../WORK
DEL   *.*
CD    ../OBJ
DEL   *.*
CD    ../LST
DEL   *.*
CD    ..
RD WORK
RD OBJ
RD LST
CD    TEST
DEL   TEST.EXE
DEL   TEST.MAP