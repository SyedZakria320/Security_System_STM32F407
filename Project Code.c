#include<stm32f4xx.h>

// PASSWORD ARRAYS FOR STORING PASSWORDS OF USERS

volatile unsigned char Password[4] = {'1', '5', '9', 'D'};		// Pre-Defined Password
volatile unsigned char User_Password[4] = {0};					// User-Password

// VARIABLES DECLARATIONS OF USED VARIABLES IN PROJECT

int size = 0;				// variable for password array index
int length = 0;				// variable for user password array index
int get_password = 0;		// variable for getting password function (if 1 user wants to set new password)
int loop = 0;				// loop variable iterator
int wrong_Password = 0;		// wrong password status (if wrong_Passwprd == 4 then entered password is wrong)
unsigned char character;	// variable for keypad key character

// FUNCTIONS DECLARATIONS OF USED FUNCTIONS IN PROJECT

void delay_us(int);
void Configure_Relay();
void Configure_Keypad();
void Configure_LCD();
void LCD_Write_Com(unsigned char);
void LCD_Initialization();
void LCD_Write_Data(unsigned char);
void LCD_String(unsigned char*);
void Reset();
void Password_Saved();
void User_Data_Checker(unsigned char);
void Lock_Open();
void New_Password();
void Wrong_Password();
void Checker(unsigned char);
void COL_1();
void COL_2();
void COL_3();
void COL_4();
void Keypad();

// MAIN FUNCTION OF PROJECT

int main()
{	
	Configure_Relay();
	
	Configure_Keypad();
	
	Configure_LCD();
	LCD_Initialization();
	
	delay_us(10000);
	LCD_String("==HELLO  WORLD==");
	delay_us(100000);
	LCD_Write_Com(0xC0);
	LCD_String("HOW ARE YOU GUYS");
	delay_us(100000);
	LCD_Write_Com(0x01);
	
	delay_us(10000);
	LCD_String("[ENTER PASSWORD]");
	LCD_Write_Com(0xC0);
	delay_us(100);
	
	while (1)
	{
		Keypad();
	}
}

void delay_us(int x)	// Microsecond Delay Function
{
  volatile unsigned int i;
  volatile unsigned int j;
  for (i=0; i<x; i++)
  {
    j = 11;
    while (j != 0)
    {
      j--;
    }
  }
}

void Configure_Relay()
{
	int pin;
	RCC->AHB1ENR |= 0x2;				// Enable GPIO Port B
	pin = 1;
	GPIOB->MODER &= ~(0x3<<pin*2);		// Output mode for PB1
	GPIOB->MODER |=  (0x1<<pin*2);
	GPIOB->OTYPER &= ~(0x1<<pin);		// Open-Drain Output Type
	GPIOB->OTYPER |=  (0x1<<pin);
	GPIOB->ODR |= (0x1<<1);				// Logic 1 for PB1 but for Open-Drain it means logic 0
}

void Configure_Keypad()
{
	RCC->AHB1ENR |= 0x8;					// CLK Enable Port D
	for (int pin=0; pin<4; pin++)			// Input mode for PD0-PD3 or R1-R4
	{
		GPIOD->MODER &= ~(0x3<<pin*2);
		GPIOD->PUPDR &= ~(0x3<<pin*2);		
		GPIOD->PUPDR |=  (0x2<<pin*2);		// Pull Down Input
	}
	for (int pin=4; pin<8; pin++)			// Output mode for PD4-PD7 or C1-C4
	{
		GPIOD->MODER &= ~(0x3<<pin*2);
		GPIOD->MODER |=  (0x1<<pin*2);
		GPIOD->PUPDR &= ~(0x3<<pin*2);
		GPIOD->PUPDR |=  (0x2<<pin*2);		// Pull Down Output
	}
}

void Configure_LCD()
{
	RCC->AHB1ENR |=0x4;			// CLK Enable Port C
	RCC->AHB1ENR |=0x10;  		// CLK Enable Port E
	
	for(int pin=0; pin<=7; pin++)		// Output Logic 01 for PC 0-7 that are Data pins from D0-D7
	{
		GPIOC->MODER &= ~(0x3<<(pin*2));
		GPIOC->MODER |=  (0x01<<(pin*2));      
	}
	for(int pin=1; pin<=3; pin++)		// Output Logic 01 of PE 1-3 that are Rs, Rw, En respectively
	{
		GPIOE->MODER &= ~(0x3<<(pin*2));
		GPIOE->MODER |=  (0x01<<(pin*2));      
	}
}

void LCD_Write_Com(unsigned char command)		// Function for Writing Commands
{
	GPIOE->ODR &= ~(0x2);		// PE1 -> RS logic 0 because Command Instruction
	GPIOE->ODR &= ~(0x4);   	// PE2 -> RW logic 0 
	GPIOE->ODR |=  (0x8);  		// PE3 -> Enable logic 1
	delay_us(2);
	GPIOC->ODR &= ~(0xFF);		// PC 0-7 clears bits
	GPIOC->ODR |= command;		// Command transfer on PC 0-7
	delay_us(2);
	GPIOE->ODR &= ~(0x8);		// Enable logic 0
	delay_us(2);
}

void LCD_Initialization()
{
	LCD_Write_Com(0x38);		// Function Set
	LCD_Write_Com(0x0C);  		// Display ON - Cursor OFF
	LCD_Write_Com(0x06); 		// Cursor Shift to Right
	LCD_Write_Com(0x01);  		// Clear Display
	LCD_Write_Com(0x80);		// Home location of Cursor
}

void LCD_Write_Data(unsigned char data)		// Function for Writing Data on LCD
{
	GPIOE->ODR |=  (0x2);  		// PE1 -> RS logic 1 because Data
	GPIOE->ODR &= ~(0x4);     	// PE2 -> RW logic 0
	GPIOE->ODR |=  (0x8);  	  	// PE3 -> Enable logic 1
	delay_us(2);
	GPIOC->ODR &= ~(0xFF);		// PC 0-7 clears bits
	GPIOC->ODR |= data;			// Data transfer on PC 0-7
	delay_us(2);
	GPIOE->ODR &= ~(0x8);		// Enable logic 0
	delay_us(2);
}	

void LCD_String(unsigned char *str)		// Function for Printing String on LCD
{
	for (int i=0; str[i] !=0; i++)
	{
		LCD_Write_Data(str[i]);
	}
}

void Reset()
{
	size = 0;
	length = 0;
	loop = 0;
	wrong_Password = 0;
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	LCD_String("[ENTER PASSWORD]");
	delay_us(10000);
	LCD_Write_Com(0xC0);
	delay_us(10000);
}

void Password_Saved()
{
	loop = 0;
	get_password = 0;
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	LCD_String("    [STORED]    ");
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	Reset();
//	LCD_Write_Com(0xC0);
//	delay_us(100);
//	delay_us(10000);
//	for (int i=0; i<4; i++)
//	{
//		delay_us(1000);
//		LCD_Write_Data(User_Password[i]);
//	}
//	delay_us(10000);
}

void User_Data_Checker(unsigned char key)
{
	if (get_password==1 && key!='#')
	{
		User_Password[loop] = key;
		loop++;
	}
	if (loop==4)
	{
		Password_Saved();
	}
}

void Lock_Open()
{
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	LCD_String("Password Correct");
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	LCD_String("  [ LOCK OPEN ] ");
	delay_us(100);
	GPIOB->ODR &= ~(0x1<<1);
	delay_us(100000);
	delay_us(100000);
	delay_us(100000);
	delay_us(100000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	LCD_String(" [ LOCK CLOSE ] ");
	delay_us(100);
	GPIOB->ODR |= (0x1<<1);
	Reset();
}

void New_Password()
{
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(10000);
	LCD_String(" [NEW PASSWORD] ");
	delay_us(10000);
	delay_us(10000);
	LCD_Write_Com(0xC0);
	delay_us(10000);
	get_password = 1;
}

void Wrong_Password()
{
	delay_us(10000);
	LCD_Write_Com(0x01);
	delay_us(100000);
	delay_us(100000);
	LCD_String("[WRONG PASSWORD]");
	delay_us(100000);
	Reset();
}

void Checker(unsigned char key)
{
	if (key == Password[size])
	{
		size++;
	}
	
	if (key == User_Password[length])
	{
		length++;
	}
	
	if (key != '*' && key != '#' && get_password == 0)
	{
		wrong_Password++;
	}
	
	if (key == '*')
	{
		Reset();
	}
	if (key == '#')
	{
		New_Password();
	}
	if (size == 4 || length == 4)
	{
		Lock_Open();	
	}
	else if (wrong_Password == 4)
	{
		Wrong_Password();
	}
}

void COL_1()
{
	GPIOD->ODR |= (0x1<<4);			// Logic 1 for PD4 -> C1
	GPIOD->IDR |= 0x0;
	if (GPIOD->IDR&0x1)
	{
		delay_us(100);
		if (GPIOD->IDR&0x1)
		{
			character = '1';
			LCD_Write_Data('1');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x2)
	{
		delay_us(100);
		if (GPIOD->IDR&0x2)
		{
			character = '4';
			LCD_Write_Data('4');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x4)
	{
		delay_us(100);
		if (GPIOD->IDR&0x4)
		{
			character = '7';
			LCD_Write_Data('7');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x8)
	{
		delay_us(100);
		if (GPIOD->IDR&0x8)
		{
			character = '*';
			LCD_Write_Data('*');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	GPIOD->ODR = 0;
}

void COL_2()
{
	GPIOD->ODR |= (0x1<<5);			// Logic 1 for PD5 -> C2
	GPIOD->IDR |= 0x0;
	if (GPIOD->IDR&0x1)
	{
		delay_us(100);
		if (GPIOD->IDR&0x1)
		{
			character = '2';
			LCD_Write_Data('2');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x2)
	{
		delay_us(100);
		if (GPIOD->IDR&0x2)
		{
			character = '5';
			LCD_Write_Data('5');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x4)
	{
		delay_us(100);
		if (GPIOD->IDR&0x4)
		{
			character = '8';
			LCD_Write_Data('8');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x8)
	{
		delay_us(100);
		if (GPIOD->IDR&0x8)
		{
			character = '0';
			LCD_Write_Data('0');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	GPIOD->ODR = 0;
}

void COL_3()
{
	GPIOD->ODR |= (0x1<<6);			// Logic 1 for PD6 -> C3
	GPIOD->IDR |= 0x0;
	if (GPIOD->IDR&0x1)
	{
		delay_us(100);
		if (GPIOD->IDR&0x1)
		{
			character = '3';
			LCD_Write_Data('3');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x2)
	{
		delay_us(100);
		if (GPIOD->IDR&0x2)
		{
			character = '6';
			LCD_Write_Data('6');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x4)
	{
		delay_us(100);
		if (GPIOD->IDR&0x4)
		{
			character = '9';
			LCD_Write_Data('9');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x8)
	{
		delay_us(100);
		if (GPIOD->IDR&0x8)
		{
			character = '#';
			LCD_Write_Data('#');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	GPIOD->ODR = 0;
}

void COL_4()
{
	GPIOD->ODR |= (0x1<<7);			// Logic 1 for PD7 -> C4
	GPIOD->IDR |= 0x0;
	if (GPIOD->IDR&0x1)
	{
		delay_us(100);
		if (GPIOD->IDR&0x1)
		{
			character = 'A';
			LCD_Write_Data('A');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x2)
	{
		delay_us(100);
		if (GPIOD->IDR&0x2)
		{
			character = 'B';
			LCD_Write_Data('B');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x4)
	{
		delay_us(100);
		if (GPIOD->IDR&0x4)
		{
			character = 'C';
			LCD_Write_Data('C');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	else if (GPIOD->IDR&0x8)
	{
		delay_us(100);
		if (GPIOD->IDR&0x8)
		{
			character = 'D';
			LCD_Write_Data('D');
			delay_us(100000);
			Checker(character);
			User_Data_Checker(character);
		}
	}
	GPIOD->ODR = 0;
}

void Keypad()
{
	COL_1();
	COL_2();
	COL_3();
	COL_4();
}
