/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "LCD.h"
#include "Menu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

int32_t   Temp=10;
int32_t   Humid=50;

void  SelectMenuCallBack(Menu_Key_t Menu_key);
void  AdjustTempCallBack(Menu_Key_t Menu_key);
void  AdjustHumidCallBack(Menu_Key_t Menu_key);


void  AdjustTempCallBack(Menu_Key_t Menu_key)
{
  if(Menu_key==Menu_Key_Select)
  {
    Temp = Menu_AdjustValueGetInt32();        
    Menu_ScrollingVerticalCreate(SelectMenuCallBack,2,"Temp","Humid");   
  }
  else if(Menu_key==Menu_Key_Back)
  {
    Menu_ScrollingVerticalCreate(SelectMenuCallBack,2,"Temp","Humid");    
  }  
}

void  AdjustHumidCallBack(Menu_Key_t Menu_key)
{
  if(Menu_key==Menu_Key_Select)
  {
    Humid = Menu_AdjustValueGetInt32();        
    Menu_ScrollingVerticalCreate(SelectMenuCallBack,2,"Temp","Humid");   
  }
  else if(Menu_key==Menu_Key_Back)
  {
    Menu_ScrollingVerticalCreate(SelectMenuCallBack,2,"Temp","Humid");    
  }    
}

void  SelectMenuCallBack(Menu_Key_t Menu_key)
{
  if(Menu_key==Menu_Key_Select)
  {
    if(Menu_ScrollingVerticalGetSelected()==0)
    {
      Menu_AdjustValueCreateInt32(AdjustTempCallBack,"Temp","Set: ",Temp,-20,50,1);
    }
    if(Menu_ScrollingVerticalGetSelected()==1)
    {
      Menu_AdjustValueCreateInt32(AdjustHumidCallBack,"Humid","Set: ",Humid,0,100,1);
    }
  }  
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask_GUIHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask_GUI */
  osThreadDef(myTask_GUI, StartTask02, osPriorityIdle, 0, 256);
  myTask_GUIHandle = osThreadCreate(osThread(myTask_GUI), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  LCD_Init();
  Menu_Init();
  Menu_StaticTextCreate(NULL,"www.github.com\r\n/nimaltd");
  osDelay(2000);
  Menu_ScrollingVerticalCreate(SelectMenuCallBack,2,"Temp","Humid");
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask_GUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    Menu_Loop();
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
