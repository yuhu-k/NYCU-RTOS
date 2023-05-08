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

#define          SCENARIO            1

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */

#define          TASK_START_ID       0                /* Application tasks                             */
#define          TASK_1_ID           1
#define          TASK_2_ID           2
#define          TASK_3_ID           3

#define          TASK_START_PRIO     15               /* Application tasks priorities                  */
#define          TASK_1_PRIO         3
#define          TASK_2_PRIO         4
#define          TASK_3_PRIO         5

#define          MUTEX_1_PRIO        1
#define          MUTEX_2_PRIO        2

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

//typedef struct period{
//    MUTEX_USER* resource[2];
//} TASK_PARAMETER_DATA;

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


#if SCENARIO == 1
    #define UserProcessNum  3
#elif SCENARIO == 2
    #define UserProcessNum  2
#endif

static OS_EVENT           *resource[2];
//static TASK_PARAMETER_DATA  TaskPdata[UserProcessNum];       /* The number of user's process for lab*/


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
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();
    OSTimeSet(0);

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
#if SCENARIO == 1
    INT8U   err1,err2;
    resource[0] = OSMutexCreate(MUTEX_1_PRIO,&err1);
    resource[1] = OSMutexCreate(MUTEX_2_PRIO,&err2);
    strcpy(TaskUserData[TASK_1_ID].TaskName, "Task1");
    strcpy(TaskUserData[TASK_2_ID].TaskName, "Task2");
    strcpy(TaskUserData[TASK_3_ID].TaskName, "Task3");

    OSTaskCreateExt(UserTask,
                    (void*)TASK_3_ID,
                    &Task3Stk[TASK_STK_SIZE - 1],
                    TASK_3_PRIO,
                    TASK_3_ID,
                    &Task3Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_3_ID],
                    0);
    

    OSTaskCreateExt(UserTask,
                    (void*)TASK_1_ID,
                    &Task1Stk[TASK_STK_SIZE - 1],
                    TASK_1_PRIO,
                    TASK_1_ID,
                    &Task1Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_1_ID],
                    0);
    
    OSTaskCreateExt(UserTask,
                    (void*)TASK_2_ID,
                    &Task2Stk[TASK_STK_SIZE - 1],
                    TASK_2_PRIO,
                    TASK_2_ID,
                    &Task2Stk[0],
                    TASK_STK_SIZE,
                    &TaskUserData[TASK_2_ID],
                    0);
#elif SCENARIO == 2
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
#endif

}
/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK #1
*********************************************************************************************************
*/

void  UserTask (void *pdata)
{
    int toDelay;
    INT8U  TaskID;
    INT8U  err;
    INT16U time;

    time = OSTimeGet();
    TaskID = (INT8U) pdata;
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
    OSTaskDel(OS_PRIO_SELF);
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

INT8U OSLabLogPrint (const char *buffer){
    if(OSTASKDmp.full == 0 || OSTASKDmp.queue_head != OSTASKDmp.queue_tail){
        sprintf(OSTASKDmp.queue[OSTASKDmp.queue_tail],buffer);
        OSTASKDmp.queue_tail = (OSTASKDmp.queue_tail+1) % 32;
        OSTASKDmp.full = 1;
        return 1;
    }
    return 0;
}