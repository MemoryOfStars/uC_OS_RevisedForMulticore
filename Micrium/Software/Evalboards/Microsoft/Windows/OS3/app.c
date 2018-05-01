/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*                          (c) Copyright 2009-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                          APPLICATION CODE
*
*                                          Microsoft Windows
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JJL
*                 FGK
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include  <windows.h>
#include  <mmsystem.h>
#include  <stdio.h>

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/
#include <time.h> 
void delay(int sec)
{
	time_t start_time, cur_time; // 变量声明 
time(&start_time);
do {
	time(&cur_time);
} while ((cur_time - start_time) < sec);
}
/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  OS_TCB   MyTaskTCB;
static  CPU_STK  MyTaskStk[APP_TASK_START_STK_SIZE];
 

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart(void  *p_arg);
static  void  MyTask(void * p_arg);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  err;


    OSInit(&err);                                               /* Init uC/OS-III.                                      */
	//system("pause");
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO/*11u*/,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10u,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),//任务运行时要检查栈的使用情况并且创建任务时要清空栈
                 (OS_ERR     *)&err);							/*OS还未开始运行，不调用create中的OSSched()*/

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg       is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Note(s)     : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                   used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
    OS_ERR  err;
	HANDLE hProc;
	PDWORD_PTR proAff;
	PDWORD_PTR sysAff;
	BOOL error;
	volatile int i, j, k;
	i = 0, j = 0, k = 0;int flag = 0;
   (void)p_arg;

                                             /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize uC/CPU services                           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    APP_TRACE_DBG(("uCOS-III is Running...\n\r"));

	OSTaskCreate((OS_TCB     *)&MyTaskTCB,                /* Create the start task                                */
		(CPU_CHAR   *)"My Task ",
		(OS_TASK_PTR)MyTask,
		(void       *)0,
		(OS_PRIO)	5u,
		(CPU_STK    *)&MyTaskStk[0],
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10u,
		(CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
		(OS_MSG_QTY)0u,
		(OS_TICK)0u,
		(void       *)0,
		(OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),//任务运行时要检查栈的使用情况并且创建任务时要清空栈
		(OS_ERR     *)&err,
		GetCurrentProcessorNumber());

    while (DEF_ON) {                                            /* Task body, always written as an infinite loop.       */
        OSTimeDlyHMSM(0, 0, 2, 0,
                      OS_OPT_TIME_DLY,
                      &err, GetCurrentProcessorNumber()
						);
		
		
		APP_TRACE_DBG(("APP Task CPU: %d ", GetCurrentProcessorNumber()));
		APP_TRACE_DBG(("In Time: %d \n\r", OSTimeGet(&err)));
		
		
		for (int i = 0;i < 4 && CPU_STATS[i];i++)
		{
			
			APP_TRACE_DBG(("CPU %d : %s \n\r", i, CPU_STATS[i]->CPU_CurTask_TCB->NamePtr));
		}
		OSTimeDlyHMSM(0, 0, 3, 0,
			OS_OPT_TIME_DLY,
			&err,
			GetCurrentProcessorNumber());
        APP_TRACE_DBG(("APP Out Time: %d\n\r", OSTimeGet(&err)));
    }
}

static void MyTask(void *p_arg)
{
	volatile int i, j, k;
	i = 0, j = 0, k = 0;int flag = 0;
	OS_ERR err;
	(void)p_arg;

	/* Initialize BSP functions                             */
	CPU_Init();                                                 /* Initialize uC/CPU services                           */

#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

	APP_TRACE_DBG(("MyTask created by APP TASK START.....\n\r"));
	while (DEF_ON)
	{
		OSTimeDlyHMSM(0,0,1,0, OS_OPT_TIME_DLY,&err, GetCurrentProcessorNumber());
		APP_TRACE_DBG(("MyTask CPU: %d ", GetCurrentProcessorNumber()));
		APP_TRACE_DBG(("In Time: %d \n\r", OSTimeGet(&err)));
		
		for (int i = 0;i < 4 && CPU_STATS[i];i++)
		{
			
				APP_TRACE_DBG(("CPU %d : %s \n\r", i, CPU_STATS[i]->CPU_CurTask_TCB->NamePtr));
		}
		OSTimeDlyHMSM(0, 0, 2, 0,
			OS_OPT_TIME_DLY,
			&err, GetCurrentProcessorNumber());
		APP_TRACE_DBG(("My Out Time: %d\n\r", OSTimeGet(&err)));
	}
}
