#include<avr/io.h>
//#include"lcd.h"



#define F_CPU 16000000UL

#define SpeedMotor_OCR OCR2 //setup the speed
#define ServoMotor_OCR OCR1B //setup the direction


#define Servo_PIN PD4 //servo motor PIN
#define Motor_PIN PD7 //DC motor PIN

/* Motor Rotation Setup*/

#define VNH_INA PB4  // drive motor direction PIN1
#define VNH_INB PB7	 // drive motor direction PIN2


/* Motor rotation direction*/

#define CLOCKWISE 1
#define COUNTERCLOCKWISE 2
#define BRAKE 3



#define Servo_Initial_Direction 2900
#define Motor_Initial_Speed 40



void Motor_Start(unsigned char motor_direction); 
void Motor_Brake(unsigned char motor_direction); 

void Init_PWM_Timer1();
void Init_PWM_Timer2();
void Basic_Init();
void Car_Drive();
void Delay_Us(short time);



int Motor_Direction=0;





int main()
{
 
  Basic_Init();
  Init_PWM_Timer1();	//Initializing the Timer1 for controlling the Servo Motor.
  Init_PWM_Timer2();   // Initializing the Timer2 for controlling the Speed Motor.
  Motor_Start(CLOCKWISE);   
  Car_Drive();
  
return 0;
}



void Car_Drive()
{
  
  int servo_DirectionInc=0;
  unsigned int i,tempPinA=0, old_ServoDir=0; 
  unsigned int last_Sensor=0, new_Sensor=0,speed_Increase=0; 
	
 PORTB = (1 << VNH_INA) | (0 << VNH_INB); 
 OCR2=220;
  
  //for(i=0; i<10; i++) 
  //	Delay_Us(20);
  
 
  SpeedMotor_OCR=Motor_Initial_Speed;
  
 while(1)
  {
	
	
	//last_Sensor=new_Sensor;
  tempPinA= PINA;
  if(tempPinA!=0)
  {
	switch(tempPinA)
	{
		case 0x01:
				{
					
					speed_Increase=0;
     					servo_DirectionInc=480; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=3;
					break;
				}

		case 0x03:
				{
					
					speed_Increase=0;
					servo_DirectionInc=480; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}

		case 0x02:
				{
					
					speed_Increase=0;
					servo_DirectionInc=384; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		case 0x06:
				{
					
					speed_Increase=0;
					servo_DirectionInc=384; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		/*case 0x04:

				{
					
					speed_Increase=0;
					servo_DirectionInc=192; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=1;
					break;
				}*/
		case 0x0C:
				{
					
					speed_Increase=0;
					servo_DirectionInc=128; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		case 0x08:

				{
					
					speed_Increase=0;
					servo_DirectionInc=64; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=0;
					break;
				}
		case 0x18:
				{
					
					speed_Increase=0;
					servo_DirectionInc=0; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		case 0x10:

				{
					
					speed_Increase=0;
					servo_DirectionInc=-64; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=0;
					break;
				}
		case 0x30:
				{
					
					speed_Increase=0;
					servo_DirectionInc=-128; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		case 0x20:

				{
					
					speed_Increase=0;
					servo_DirectionInc=-192;
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=1; 					
					break;
				}
		case 0x60:
				{
					
					speed_Increase=0;
					servo_DirectionInc=-384; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		case 0x40:

				{
					
					speed_Increase=0;
					servo_DirectionInc=-384; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=3; 					
					break;
				}
		case 0xC0:
				{
					
					speed_Increase=0;
					servo_DirectionInc=-480; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=2;
					break;
				}
		case 0x80:

				{
					
					speed_Increase=0;
					servo_DirectionInc=-480; 
					old_ServoDir=servo_DirectionInc;
					//new_Sensor=3;
					break;
				}


			
	}//end of switch
	
   }	
	
    
	//if((new_Sensor-last_Sensor)>2 || (new_Sensor-last_Sensor)<-2)
     //   speed_Increase = 0;
	
	ServoMotor_OCR=Servo_Initial_Direction + servo_DirectionInc;
	SpeedMotor_OCR=Motor_Initial_Speed + speed_Increase; 
  
  }//end of the while loop

}//end of the Car_Drive method






/* Initalize Timer1 */

void Init_PWM_Timer1()
{
	//setting the prescaler 8
	TCCR1B|=(0<<CS12)|(1<<CS11)|(0<<CS10);
	//setting the compare mode in no-inverting
	TCCR1A|=(1<<COM1B1)|(0<<COM1B0);
	//
	TCCR1B|=(1<<WGM13)|(1<<WGM12);
	TCCR1A|=(1<<WGM11)|(0<<WGM10);
	//setting the TOP value
	ICR1=20000;
	//OCR1B=200;
}

/* Initalize Timer2 */
void Init_PWM_Timer2()
{
	
	TCNT2 = 0;	
	//Fast PWM	
	TCCR2|=(1<<WGM21)|(1<<WGM20);
	//Non Inverting Mode 
	TCCR2|=(1<<COM21)|(0<<COM20);
	//Seting prescaler 8
	TCCR2|=(0<<CS22)|(1<<CS21)|(0<<CS20);             

}


/* Port initialization for Sensor and Motors*/
void Basic_Init()
{
    DDRA  = 0x00;
    PORTA = 0xFF;
    DDRD = _BV(Servo_PIN)|_BV(Motor_PIN);
    DDRB = _BV(VNH_INA) | _BV(VNH_INB);

}



/* Motor initialize for Clockwise rotation */
void Motor_Start(unsigned char motor_direction) 
{    
   
   Motor_Direction = CLOCKWISE;
}


/* Brake to the Motor */
void Motor_Brake(unsigned char motor_direction)
{

   PORTB = (1 << VNH_INA) | (1 << VNH_INB); 
   Motor_Direction = BRAKE;

}


/* Micro second delay */
void Delay_Us (short time)
{
	short i=0;
	for(i=0;i<time;++i)
	{
		__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //250 nano second |
		__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //250 nano second |
		__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //250 nano second | one micro second
		__asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t"); //250 nano second |
	}
}






