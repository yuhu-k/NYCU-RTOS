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

#include "includes.h"

/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/

#define          example             0

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       0                /* Application tasks                             */
#define          TASK_CLK_ID         1
#define          TASK_1_ID           2
#define          TASK_2_ID           3
#define          TASK_3_ID           4

#define          TASK_START_PRIO    10                /* Application tasks priorities                  */
#define          TASK_CLK_PRIO      11
#define          TASK_1_PRIO        12
#define          TASK_2_PRIO        13
#define          TASK_3_PRIO        14

#define          MSG_QUEUE_SIZE     20                /* Size of message queue used in example         */

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
} TASK_USER_DATA;

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

TASK_USER_DATA  TaskUserData[7];

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
        void  Task1(void *data);
        void  Task2(void *data);
        void  Task3(void *data);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                  MAIN
*********************************************************************************************************
*/

void  main (void)
{
    PC_DispClrScr(DISP_BGND_BLACK);                        /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */

    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    PC_ElapsedInit();                                      /* Initialized elapsed time measurement     */
    strcpy(TaskUserData[TASK_START_ID].TaskName, "StartTask");
    OSTaskCreateExt(TaskStart,
                    (void *)0,
                    &TaskStartStk[TASK_STK_SIZE - 1],
                    TASK_START_PRIO,
                    TASK_START_ID,
                    &TaskStartStk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_START_ID],
                    0);  
    OSStart();                                             /* Start multitasking                       */
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
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */


    OS_ENTER_CRITICAL();                                   /* Install uC/OS-II's clock tick ISR        */
    PC_VectSet(0x08, OSTickISR);
    PC_SetTickRate(OS_TICKS_PER_SEC);                                     /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    TaskStartCreateTasks();

    for (;;) {
//
        if (PC_GetKey(&key)) {                             /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Yes, return to DOS                       */
            }
        }

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
        OSTimeDly(10);
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
    strcpy(TaskUserData[TASK_1_ID].TaskName, "Task1(1,3)");
    TaskPdata[0].exeTime = 1;
    TaskPdata[0].period = 3;
    OSTaskCreateExt(Task1,
                    (void *)(&TaskPdata[0]),
                    &Task1Stk[TASK_STK_SIZE - 1],
                    TASK_1_PRIO,
                    TASK_1_ID,
                    &Task1Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_1_ID],
                    0);
    strcpy(TaskUserData[TASK_2_ID].TaskName, "Task2(3,6)");
    TaskPdata[1].exeTime = 3;
    TaskPdata[1].period = 6;
    OSTaskCreateExt(Task1,
                    (void *)(&TaskPdata[1]),
                    &Task2Stk[TASK_STK_SIZE - 1],
                    TASK_2_PRIO,
                    TASK_2_ID,
                    &Task2Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_2_ID],
                    0);

    
#elif example == 1
    strcpy(TaskUserData[TASK_5_ID].TaskName, "Task1(1,3)");
    OSTaskCreateExt(Task5,
                    (void *)0,
                    &Task5Stk[TASK_STK_SIZE - 1],
                    TASK_5_PRIO,
                    TASK_5_ID,
                    &Task5Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_5_ID],
                    0);
    strcpy(TaskUserData[TASK_5_ID].TaskName, "Task2(3,6)");
    OSTaskCreateExt(Task5,
                    (void *)0,
                    &Task5Stk[TASK_STK_SIZE - 1],
                    TASK_5_PRIO,
                    TASK_5_ID,
                    &Task5Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_5_ID],
                    0);
    strcpy(TaskUserData[TASK_5_ID].TaskName, "Task3(4,9)");
    OSTaskCreateExt(Task5,
                    (void *)0,
                    &Task5Stk[TASK_STK_SIZE - 1],
                    TASK_5_PRIO,
                    TASK_5_ID,
                    &Task5Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_5_ID],
                    0);
#endif

}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #1
*********************************************************************************************************
*/

void  Task1 (void *pdata)
{
    char  *msg;
    INT8U  err;


    TASK_PARAMETER_DATA *argv = (TASK_PARAMETER_DATA*)pdata;
    INT16U start;
    INT16U end;
    INT16U toDelay;

    INT16U c = argv->exeTime;
    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = c;
    OSTCBCur->period = argv->period;
    OS_EXIT_CRITICAL();

    start = OSTimeGet();

    while (1)
    {
        while(OSTCBCur->compTime > 0);
        end = OSTimeGet();
        toDelay = OSTCBCur->period - (end - start);
        start = start + OSTCBCur->period;
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime = c;
        OS_EXIT_CRITICAL();
        OSTimeDly(toDelay);
    }
    
}
/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*********************************************************************************************************
*/
void  OSInitHookBegin (void)
{
}

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*********************************************************************************************************
*/
void  OSInitHookEnd (void)
{
}

/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*********************************************************************************************************
*/
void  OSTaskCreateHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}

/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*********************************************************************************************************
*/
void  OSTaskDelHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*********************************************************************************************************
*/
void  OSTaskIdleHook (void)
{
}

/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*********************************************************************************************************
*/
void  OSTaskSwHook (void)
{
    INT16U           time;
    TASK_USER_DATA  *puser;


    time  = PC_ElapsedStop();                    /* This task is done                                  */
    PC_ElapsedStart();                           /* Start for next task                                */
    puser = OSTCBCur->OSTCBExtPtr;               /* Point to used data                                 */
    if (puser != (TASK_USER_DATA *)0) {
        puser->TaskCtr++;                        /* Increment task counter                             */
        puser->TaskExecTime     = time;          /* Update the task's execution time                   */
        puser->TaskTotExecTime += time;          /* Update the task's total execution time             */
    }
}

/*
*********************************************************************************************************
*                                           OSTCBInit() HOOK
*********************************************************************************************************
*/
void  OSTCBInitHook (OS_TCB *ptcb)
{
    ptcb = ptcb;                                           /* Prevent Compiler warning                 */
}

/*
*********************************************************************************************************
*                                               TICK HOOK
*********************************************************************************************************
*/
void  OSTimeTickHook (void)
{
}
