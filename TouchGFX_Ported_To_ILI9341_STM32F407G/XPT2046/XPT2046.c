/*
 * XPT2064.c
 *
 *  Created on: 16.06, 2021
 *      Author: Teodor
 In fact, this file was directly copied from https://kursstm32.pl/
 */
#include "../ili9341/core.h"
#include "XPT2046.h"

void XPT2046_GetTouchPoint(uint16_t *X, uint16_t *Y);

#define MAX_SAMPLES 8
#define SAMPLE_INTERVAL	1

static SPI_HandleTypeDef *Xpt2046SpiHandler;
static IRQn_Type Xpt2046Irqn;

static uint8_t ChannelSettingsX, ChannelSettingsY;
static uint8_t SendBuffer[5];
static uint8_t ReceiveBuffer[5];

uint16_t TouchSamples[2][MAX_SAMPLES];
uint8_t SampleCounter;
uint32_t SampleTimer;

//
// Calibration data for make a new coordinates calculation
//
typedef struct
{
	long double alpha_x;
	long double beta_x;
	long double delta_x;
	long double alpha_y;
	long double beta_y;
	long double delta_y;
} CalibData_t;

volatile XPT2046_State TouchState; // Current state
static uint8_t CalibrationMode; // Is calibration mode active

#if (TOUCH_ROTATION == 0)
CalibData_t CalibrationData = {-.0009337, -.0636839, 250.342, -.0889775, -.00118110, 356.538}; // default calibration data
#endif
#if (TOUCH_ROTATION == 1)
CalibData_t CalibrationData = {-.0885542, -.0016532, 350.800, .00173, -.06543699, 249.7890}; // default calibration data
#endif
#if (TOUCH_ROTATION == 2)
CalibData_t CalibrationData = {.0006100, .0647828, -13.634, .0890609, .0001381, -35.73}; // default calibration data
#endif
#if (TOUCH_ROTATION == 3)
CalibData_t CalibrationData = {.0902216, .0006510, -38.657, -.0010005, -.0667030, 258.08}; // default calibration data
#endif
//
//	Calibration data - pattern points
//
#if (TOUCH_ROTATION == 0 || TOUCH_ROTATION == 2)
uint16_t calA[] = {10, 10};	// Calibration points
uint16_t calB[] = {80, 280};
uint16_t calC[] = {200, 170};
#endif
#if (TOUCH_ROTATION == 1 || TOUCH_ROTATION == 3)
uint16_t calA[] = {20, 25};	// Calibration points
uint16_t calB[] = {160, 220};
uint16_t calC[] = {300, 110};
#endif
//
// Calibration points - read from panel
//
uint16_t calA_raw[] = {0, 0}; // Read data
uint16_t calB_raw[] = {0, 0};
uint16_t calC_raw[] = {0, 0};
//
// HW function
//
//
// Read Raw data from controller XPT2046
//
void XPT2046_GetRawData(void)
{
#if (XPT2046_USE_CS == 1)
	HAL_GPIO_WritePin(SPI1_CS_Touch_GPIO_Port, SPI1_CS_Touch_Pin, GPIO_PIN_RESET); // CS Active
#endif
	// Send Control bytes and receive raw ADC values from controler
	HAL_SPI_TransmitReceive(Xpt2046SpiHandler, SendBuffer, ReceiveBuffer, 5, XPT2046_SPI_TIMEOUT);
#if (XPT2046_USE_CS == 1)
	HAL_GPIO_WritePin(SPI1_CS_Touch_GPIO_Port, SPI1_CS_Touch_Pin, GPIO_PIN_SET); // CS Idle
#endif
}

//
// Rest
//

//
// Read Raw ADC data to variables
//
void XPT2046_ReadRawData(uint16_t *X, uint16_t *Y)
{
	*X = (uint16_t)((ReceiveBuffer[1]<<8) | (ReceiveBuffer[2]));
	*Y = (uint16_t)((ReceiveBuffer[3]<<8) | (ReceiveBuffer[4]));
}

//
// Read Raw ADC data to variables and calculate coords in pixels based on Calibration Data
//
void XPT2046_ReadTouchPoint(uint16_t *X, uint16_t *Y)
{
	uint16_t _x, _y;
	XPT2046_ReadRawData(&_x, &_y);

	if(CalibrationMode == 0)
	{
		// Calculate in Normal Mode
		*X = CalibrationData.alpha_x * _x + CalibrationData.beta_x * _y + CalibrationData.delta_x;
		*Y = CalibrationData.alpha_y * _x + CalibrationData.beta_y * _y + CalibrationData.delta_y;
	}
	else
	{
		// Get Raw data in Calibration Mode
		*X = _x;
		*Y = _y;
	}
}

//
// Get Touch Point - Average from last n (MAX_SAMPLES) measurments
//
void XPT2046_GetTouchPoint(uint16_t *X, uint16_t *Y)
{
	uint32_t AverageX = 0, AverageY = 0;
	uint8_t i;

	for(i = 0; i < MAX_SAMPLES; i++)
	{
		AverageX += TouchSamples[0][i];
		AverageY += TouchSamples[1][i];
	}

	*X = AverageX / MAX_SAMPLES;
	*Y = AverageY / MAX_SAMPLES;
}

//
// Check if screen was touched - machine state has to be in TOUCHED state
//
uint8_t XPT2046_IsTouched(void)
{
	if(TouchState == XPT2046_TOUCHED)
		return 1;

	return 0;
}

//
// Main Task - put it in main loop
//
void XPT2046_Task(void)
{
	  switch(TouchState)
	  {
	  case XPT2046_IDLE:
		  // Do nothing in ILDE - we are waiting for interrupt
		  break;

	  case XPT2046_PRESAMPLING:
		  // Get first MAX_SAMPLES samples
		  if((HAL_GetTick() - SampleTimer ) > SAMPLE_INTERVAL) // Software Timers
		  {
			  // Read Raw Data from XPT2046
			  XPT2046_GetRawData();
			  // Read calculated points to Samples array
			  XPT2046_ReadTouchPoint(&TouchSamples[0][SampleCounter], &TouchSamples[1][SampleCounter]);
			  // Increase Sample counter
			  SampleCounter++;

			  // If we reach MAX_SAMPLES samples
			  if(SampleCounter == MAX_SAMPLES)
			  {
				  // Reset Counter
				  SampleCounter = 0;
				  // Jump to TOUCHED state - we are sure that touch occurs
				  TouchState = XPT2046_TOUCHED;
			  }

			  // Check if there is no more touch (Datasheet)
			  if(GPIO_PIN_SET == HAL_GPIO_ReadPin(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin))
			  {
				  // Go to RELEASED state
				  TouchState = XPT2046_RELEASED;
			  }

			  // Feed Software Counter
			  SampleTimer = HAL_GetTick();
		  }

		  break;

	  case XPT2046_TOUCHED:
		  // Read touch points
		  if((HAL_GetTick() - SampleTimer ) > SAMPLE_INTERVAL) // Software Timers
		  {
			  // Read Raw Data from XPT2046
			  XPT2046_GetRawData();
			  // Read calculated points to Samples array
			  XPT2046_ReadTouchPoint(&TouchSamples[0][SampleCounter], &TouchSamples[1][SampleCounter]);
			  // Increase Sample counter
			  SampleCounter++;
			  // If MAX_SAMPLES reached - reset counter - simple ring buffer
			  SampleCounter %= MAX_SAMPLES;

			  // Check if there is no more touch (Datasheet)
			  if(GPIO_PIN_SET == HAL_GPIO_ReadPin(TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin))
			  {
				  // Go to RELEASED state
				  TouchState = XPT2046_RELEASED;
			  }

			  // Feed Software Counter
			  SampleTimer = HAL_GetTick();
		  }

		  break;

	  case XPT2046_RELEASED:
		  // RELEASE state

		  // Return to IDLE
		  TouchState = XPT2046_IDLE;
		  // Reset Sample counter
		  SampleCounter = 0;
		  // Clea all interrupt flags
		  while(HAL_NVIC_GetPendingIRQ(Xpt2046Irqn))
		  {
			  // GPIO FLAG
			  __HAL_GPIO_EXTI_CLEAR_IT(TOUCH_IRQ_Pin);
			  // NVIC FLAG
			  HAL_NVIC_ClearPendingIRQ(Xpt2046Irqn);
		  }
		  // Enable Interrupt for next touch
		  HAL_NVIC_EnableIRQ(Xpt2046Irqn);
		  break;

	  }
}

//
// Interrupt routine - put in IRQ handler callback
//
void XPT2046_IRQ(void)
{
	// Disable IRQ for avoid false IRQs (Datasheet)
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
	// Jump to PRESAMPLING state
	TouchState = XPT2046_PRESAMPLING;
}

//
// Init function
//
void XPT2046_Init(SPI_HandleTypeDef *hspi, IRQn_Type TouchIRQn)
{
	// Get SPI handler and IRQ number
	Xpt2046SpiHandler = hspi;
	Xpt2046Irqn = TouchIRQn;

	// Default State
	TouchState = XPT2046_IDLE;

#if (XPT2046_USE_CS == 1)
	HAL_GPIO_WritePin(SPI1_CS_Touch_GPIO_Port, SPI1_CS_Touch_Pin, GPIO_PIN_SET); // CS Idle
#endif

	//
	// Prepare Send Buffer
	//

	//     (     X    )           (     Y 	 )
	// (000 10010)(000 00000) (000 11010)(000 00000) (00000000)
	//	SendBuffer
	// (    0    )(    1    ) (    2    )(    3    ) (    4   )
		ChannelSettingsX = 0b10010000;
		ChannelSettingsY = 0b11010000;

		SendBuffer[0] = 0x80; // Clear settings in IC
		XPT2046_GetRawData(); // Send clearing command
		HAL_Delay(1); // Wait for clear

		// Fulfill Send Buffer with Channel control bytes
		SendBuffer[0] = (ChannelSettingsX>>3);
		SendBuffer[1] = (ChannelSettingsX<<5);
		SendBuffer[2] = (ChannelSettingsY>>3);
		SendBuffer[3] = (ChannelSettingsY<<5);
		SendBuffer[4] = 0;
}

//
// Calibration stuff
//

//
// Draw X in circle as calibration point indicator
//
void CalibrationPoint(uint16_t calX, uint16_t calY)
{
//Draw Calibration Point
}

//
// Calculate new Calibration data - mathematics with well known 3-point calibration
//
void CalculateCalibrationData(void)
{
	int32_t delta = (calA_raw[0]-calC_raw[0])*(calB_raw[1]-calC_raw[1]) -
	       (calB_raw[0]-calC_raw[0])*(calA_raw[1]-calC_raw[1]);

	CalibrationData.alpha_x = (float)((calA[0]-calC[0])*(calB_raw[1]-calC_raw[1]) -
	       (calB[0]-calC[0])*(calA_raw[1]-calC_raw[1])) / delta;

	CalibrationData.beta_x = (float)((calA_raw[0]-calC_raw[0])*(calB[0]-calC[0]) -
	       (calB_raw[0]-calC_raw[0])*(calA[0]-calC[0])) / delta;

	CalibrationData.delta_x = ((float)calA[0]*(calB_raw[0]*calC_raw[1]-calC_raw[0]*calB_raw[1]) -
	       (float)calB[0]*(calA_raw[0]*calC_raw[1]-calC_raw[0]*calA_raw[1]) +
	       (float)calC[0]*(calA_raw[0]*calB_raw[1]-calB_raw[0]*calA_raw[1])) / delta;

	CalibrationData.alpha_y = (float)((calA[1]-calC[1])*(calB_raw[1]-calC_raw[1]) -
	       (calB[1]-calC[1])*(calA_raw[1]-calC_raw[1])) / delta;

	CalibrationData.beta_y = (float)((calA_raw[0]-calC_raw[0])*(calB[1]-calC[1]) -
	       (calB_raw[0]-calC_raw[0])*(calA[1]-calC[1])) / delta;

	CalibrationData.delta_y = ((float)calA[1]*(calB_raw[0]*calC_raw[1]-calC_raw[0]*calB_raw[1]) -
		       (float)calB[1]*(calA_raw[0]*calC_raw[1]-calC_raw[0]*calA_raw[1]) +
		       (float)calC[1]*(calA_raw[0]*calB_raw[1]-calB_raw[0]*calA_raw[1])) / delta;
}

//
// It's blocking function
//
void DoCalibration(void)
{
	uint8_t calCount = 0;
	ILI9341_ClearDisplay(ILI9341_BLACK);
	CalibrationMode = 1;

	while(calCount < 4)
	{
		XPT2046_Task();

		switch(calCount)
		{
		case 0: // 1st point
			CalibrationPoint(calA[0], calA[1]);
			if(TouchState == XPT2046_TOUCHED)
			{
				XPT2046_GetTouchPoint(&calA_raw[0], &calA_raw[1]);
			}
			if(TouchState == XPT2046_RELEASED)
			{
				HAL_Delay(200);
				calCount++;
			}
			break;
		case 1: // 2nd point
		//DrawFillRectangle

			CalibrationPoint(calB[0], calB[1]);
			if(TouchState == XPT2046_TOUCHED)
			{
				XPT2046_GetTouchPoint(&calB_raw[0], &calB_raw[1]);
			}
			if(TouchState == XPT2046_RELEASED)
			{
				HAL_Delay(200);
				calCount++;
			}
			break;
		case 2: // 3rd point
			//DrawFillRectangle

			CalibrationPoint(calC[0], calC[1]);
			if(TouchState == XPT2046_TOUCHED)
			{
				XPT2046_GetTouchPoint(&calC_raw[0], &calC_raw[1]);
			}
			if(TouchState == XPT2046_RELEASED)
			{
				HAL_Delay(200);
				calCount++;
			}
			break;
		case 3: // calculate and save calibration data,
			//DrawFillRectangle

			CalculateCalibrationData();
			calCount++;
			break;
		}
	}
	CalibrationMode = 0; // Disable Calibration mdoe
}
