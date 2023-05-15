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

#define          SCENARIO            2

#define          TASK_STK_SIZE       2048             /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       4                /* Application tasks                             */
#define          TASK_1_ID           0
#define          TASK_2_ID           1
#define          TASK_3_ID           2

#define          TASK_START_PRIO     15                /* Application tasks priorities                  */
#define          TASK_1_PRIO         3
#define          TASK_2_PRIO         4
#define          TASK_3_PRIO         5

#define          MUTEX_1_PRIO        1
#define          MUTEX_2_PRIO        2

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

#if SCENARIO == 1
    #define UserProcessNum  3
#elif SCENARIO == 2
    #define UserProcessNum  2
#endif

static OS_EVENT           *resource[2];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  TaskStart(void *data);                  /* Function prototypes of tasks                  */
static  void  TaskStartCreateTasks(void);
        void  UserTask(void *data);
        INT8U OSLabLogPrint (const char *buffer);


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
    INT8U   err1,err2;
    resource[0] = OSMutexCreate(MUTEX_1_PRIO,&err1);
    resource[1] = OSMutexCreate(MUTEX_2_PRIO,&err2);
#if SCENARIO == 1
    strcpy(TaskUserData1[TASK_1_ID].TaskName, "Task1");
    strcpy(TaskUserData1[TASK_2_ID].TaskName, "Task2");
    strcpy(TaskUserData1[TASK_3_ID].TaskName, "Task3");

    OSTaskCreateExt(UserTask,
                    (void*)TASK_3_ID,
                    &Task3Stk[TASK_STK_SIZE - 1],
                    TASK_3_PRIO,
                    TASK_3_ID,
                    &Task3Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_3_ID],
                    0);


    OSTaskCreateExt(UserTask,
                    (void*)TASK_1_ID,
                    &Task1Stk[TASK_STK_SIZE - 1],
                    TASK_1_PRIO,
                    TASK_1_ID,
                    &Task1Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_1_ID],
                    0);

    OSTaskCreateExt(UserTask,
                    (void*)TASK_2_ID,
                    &Task2Stk[TASK_STK_SIZE - 1],
                    TASK_2_PRIO,
                    TASK_2_ID,
                    &Task2Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_2_ID],
                    0);
#elif SCENARIO == 2
    strcpy(TaskUserData1[TASK_1_ID].TaskName, "Task1");
    strcpy(TaskUserData1[TASK_2_ID].TaskName, "Task2");

    OSTaskCreateExt(UserTask,
                    (void *)TASK_1_ID,
                    &Task1Stk[TASK_STK_SIZE - 1],
                    TASK_1_PRIO,
                    TASK_1_ID,
                    &Task1Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_1_ID],
                    0);
    OSTaskCreateExt(UserTask,
                    (void *)TASK_2_ID,
                    &Task2Stk[TASK_STK_SIZE - 1],
                    TASK_2_PRIO,
                    TASK_2_ID,
                    &Task2Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData1[TASK_2_ID],
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

void  UserTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    int toDelay;
    INT8U  TaskID;
    INT8U  err;
    INT16U time;

    time = OSTimeGet();
    TaskID = (INT8U) pdata;
#if SCENARIO == 1
    switch (TaskID)
    {
    case TASK_1_ID:
        toDelay = 4 - (int)time;
        if (toDelay > 0) OSTimeDly(toDelay);

        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[0],0,&err);
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[1],0,&err);
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);
        OSMutexPost(resource[1]);
        OSMutexPost(resource[0]);
        break;

    case TASK_2_ID:
        toDelay = 8 - (int)time;
        if (toDelay > 0) OSTimeDly(toDelay);

        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[1],0,&(err));
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 4;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);
        OSMutexPost(resource[1]);
        break;

    case TASK_3_ID:
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[0],0,&(err));
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 7;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);
        OSMutexPost(resource[0]);
        break;
    }
#elif SCENARIO == 2
    switch (TaskID)
    {
    case TASK_1_ID:
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[0],0,&err);
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 6;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[1],0,&err);
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPost(resource[1]);
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPost(resource[0]);
        break;

    case TASK_2_ID:
        toDelay = 5 - (int)time;
        if (toDelay > 0) OSTimeDly(toDelay);

        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 2;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[1],0,&(err));
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 3;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPend(resource[0],0,&(err));
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 3;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPost(resource[0]);
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = 3;
        OS_EXIT_CRITICAL();
        while(OSTCBCur->compTime > 0);

        OSMutexPost(resource[1]);
        break;
    }
#endif
    OSTaskDel(OS_PRIO_SELF);

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

INT8U OSLabLogPrint (const char *buffer){
    if(OSTASKDmp.full == 0 || OSTASKDmp.queue_head != OSTASKDmp.queue_tail){
        sprintf(OSTASKDmp.queue[OSTASKDmp.queue_tail],buffer);
        OSTASKDmp.queue_tail = (OSTASKDmp.queue_tail+1) % 32;
        OSTASKDmp.full = 1;
        return 1;
    }
    return 0;
}
