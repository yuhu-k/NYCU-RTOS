/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                          (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #3
*********************************************************************************************************
*/
#include <stdio.h>
#include "includes.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          example             0

#define          TASK_STK_SIZE       2048             /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       5                /* Application tasks                             */
#define          TASK_1_ID           0
#define          TASK_2_ID           1
#define          TASK_3_ID           2

#define          TASK_START_PRIO     5                /* Application tasks priorities                  */
#define          TASK_1_PRIO         1
#define          TASK_2_PRIO         2
#define          TASK_3_PRIO         3

/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef struct {
    char    TaskName[30];
    INT16U  TaskCtr;
    INT16U  TaskExecTime;
    INT32U  TaskTotExecTime;
} TASK_USER_DATA1;

typedef struct period{
    int exeTime;
    int period;
} TASK_PARAMETER_DATA;

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/

OS_STK          TaskStartStk[TASK_STK_SIZE];          /* Startup    task stack                         */
OS_STK          Task1Stk[TASK_STK_SIZE];              /* Task #1    task stack                         */
OS_STK          Task2Stk[TASK_STK_SIZE];              /* Task #2    task stack                         */
OS_STK          Task3Stk[TASK_STK_SIZE];              /* Task #3    task stack                         */

TASK_USER_DATA1  TaskUserData1[7];

#if example == 0
    #define UserProcessNum  2
#elif example == 1
    #define UserProcessNum  3
#endif

static TASK_PARAMETER_DATA  TaskPdata[UserProcessNum];       /* The number of user's process for lab 1*/


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  TaskStart(void *data);                  /* Function prototypes of tasks                  */
static  void  TaskStartCreateTasks(void);
        void  Task(void *data);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/
INT16U global_start = 0;
void  main (void)
{
	TaskStartCreateTasks();
    strcpy(TaskUserData1[TASK_START_ID].TaskName, "StartTask");
    OSTaskCreateExt(TaskStart,
                    (void *)0,
                    &TaskStartStk[TASK_STK_SIZE - 1],
                    TASK_START_PRIO,
                    TASK_START_ID,
                    &TaskStartStk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_START_ID],
                    0);
    OSTimeSet(0);
    global_start = OSTimeGet();
    OSStart();                                   /* Start multitasking                       */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               STARTUP TASK
*********************************************************************************************************
*/
LAB1_INFO     OSTASKDmp;

void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;                                         /* Prevent compiler warning                 */
    for (;;) {
        OS_ENTER_CRITICAL();
        while(OSTASKDmp.queue_tail != OSTASKDmp.queue_head || OSTASKDmp.full == 1){
            OS_EXIT_CRITICAL();
            printf("%s",OSTASKDmp.queue[OSTASKDmp.queue_head]);
            OS_ENTER_CRITICAL();
            OSTASKDmp.queue_head = (OSTASKDmp.queue_head+1) % 32;
            OSTASKDmp.full = 0;
        }
        OS_EXIT_CRITICAL();

        OSCtxSwCtr = 0;                                    /* Clear the context switch counter         */
        OSTimeDlyHMSM(0, 0, 3, 0);                         /* Wait three seconds                       */
    }
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*********************************************************************************************************
*/

void  TaskStartCreateTasks (void)
{
#if example == 0
    strcpy(TaskUserData1[TASK_1_ID].TaskName, "Task1(1,3)");
    TaskPdata[0].exeTime = 1;
    TaskPdata[0].period = 3;
    OSTaskCreateExt(Task,
                    (void *)(&TaskPdata[0]),
                    &Task1Stk[TASK_STK_SIZE - 1],
                    TASK_1_PRIO,
                    TASK_1_ID,
                    &Task1Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_1_ID],
                    0);
    strcpy(TaskUserData1[TASK_2_ID].TaskName, "Task2(3,6)");
    TaskPdata[1].exeTime = 3;
    TaskPdata[1].period = 6;
    OSTaskCreateExt(Task,
                    (void *)(&TaskPdata[1]),
                    &Task2Stk[TASK_STK_SIZE - 1],
                    TASK_2_PRIO,
                    TASK_2_ID,
                    &Task2Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_2_ID],
                    0);


#elif example == 1
    strcpy(TaskUserData1[TASK_1_ID].TaskName, "Task1(1,3)");
    TaskPdata[0].exeTime = 1;
    TaskPdata[0].period = 3;
    OSTaskCreateExt(Task,
                    (void *)(&TaskPdata[0]),
                    &Task1Stk[TASK_STK_SIZE - 1],
                    TASK_1_PRIO,
                    TASK_1_ID,
                    &Task1Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_1_ID],
                    0);
    strcpy(TaskUserData1[TASK_2_ID].TaskName, "Task2(3,6)");
    TaskPdata[1].exeTime = 3;
    TaskPdata[1].period = 6;
    OSTaskCreateExt(Task,
                    (void *)(&TaskPdata[1]),
                    &Task2Stk[TASK_STK_SIZE - 1],
                    TASK_2_PRIO,
                    TASK_2_ID,
                    &Task2Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_2_ID],
                    0);
    strcpy(TaskUserData1[TASK_3_ID].TaskName, "Task3(4,9)");
    TaskPdata[2].exeTime = 4;
    TaskPdata[2].period = 9;
    OSTaskCreateExt(Task,
                    (void *)(&TaskPdata[2]),
                    &Task3Stk[TASK_STK_SIZE - 1],
                    TASK_3_PRIO,
                    TASK_3_ID,
                    &Task3Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_3_ID],
                    0);
#endif

}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #1
*********************************************************************************************************
*/

char Break = 0;

void  Task (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    TASK_PARAMETER_DATA *argv = (TASK_PARAMETER_DATA*)pdata;
    INT16U start;
    INT16U end;
    INT32U toDelay;

    INT16U c = argv->exeTime;
    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = c;
    OSTCBCur->period = argv->period;
    OS_EXIT_CRITICAL();

    start = global_start;

    while (1)
    {
        while(OSTCBCur->compTime > 0)if (Break == 1) break;
        if (Break == 1) break;
        end = OSTimeGet();
        toDelay = OSTCBCur->period - (end - start);
        if (toDelay > OSTCBCur->period || toDelay < 0) {
        	OS_ENTER_CRITICAL();
        	if(OSTASKDmp.full == 0 || OSTASKDmp.queue_head != OSTASKDmp.queue_tail){
        	    sprintf(OSTASKDmp.queue[OSTASKDmp.queue_tail],"%5d Delay     %5d\n",start + OSTCBCur->period, OSTCBCur->OSTCBPrio);
        	    OSTASKDmp.queue_tail = (OSTASKDmp.queue_tail+1) % 32;
        	    OSTASKDmp.full = 1;
        	    Break = 1;
        	}
        	OS_EXIT_CRITICAL();
        }
        start = start + OSTCBCur->period;
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = c;
        OS_EXIT_CRITICAL();
        OSTimeDly(toDelay);
    }

}

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/
