//Código de envio dados MSP430 para ESP8266
// Por Jonathan Hochstedler, adaptado por Juliana 
//P4.2 is for TXD, P4.3 is for RXD
//Usar função send_uart_data para passar os dados dos sensores

#include <msp430.h>

#define ENABLE_PINS   0xFFFE    // Required to use inputs and outputs
#define UART_CLK_SEL  0x0080    // Specifies accurate clock for UART peripheral
#define BR0_FOR_9600  0x34    // Value required to use 9600 baud
#define BR1_FOR_9600  0x00    // Value required to use 9600 baud
#define CLK_MOD     0x4911    // Microcontroller will "clean-up" clock signal
#define ACLK      0x0100    // Timer_A SMCLK source
#define UP        0x0010    // Timer_A UP mode
#define BUTTON11    0x02    // P1.1 is the button
#define RED_ON      0x01    // Used to turn-on/enable P1.0 LED
#define RED_OFF     0xFE    // Used to turn-off the P1.0 LED
#define GREEN_ON    0x80    // P9.7 is the green LED
#define GREEN_OFF   0x7F    // Used to turn off the green LE
#define cmd 0xAE // comando para acionar bomba (a ser recebido do esp)

unsigned int test_pushbutton(void);
void select_clock_signals(void);  // Assigns microcontroller clock signals
void assign_pins_to_uart(void);   // P4.2 is for TXD, P4.3 is for RXD
void use_9600_baud(void);       // UART operates at 9600 bits/second
void enable_TA0CTL(void);     // Enable the timer 0 counter
void set_timer0_count(int);

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;     // Stop WDT
  PM5CTL0 = ENABLE_PINS;      // Enable pins

  select_clock_signals();     // Assigns microcontroller clock signals
  assign_pins_to_uart();      // P4.2 is for TXD, P4.3 is for RXD
  use_9600_baud();        // UART operates at 9600 bits/second
  enable_TA0CTL();

  P9DIR = GREEN_ON;       // P9.7 will be an output
  P1DIR = RED_ON;         // P1.0 will be an output
  P1OUT = BUTTON11;         // P1.1 will be input
  P1REN = BUTTON11;         // with a pull-up resistor

  UCA0IE = UCRXIE;        // Enable RX interupt
  _BIS_SR(GIE);           // Activate enabled UART RXD interrupt

  set_timer0_count(4690);     // Count to this number
                  // Calculated using 104us * # = .025

  while(1)            // Wait here unless you get UART interrupt
  {

  }
}

//*********************************************************************************
//* UART Interrupt Service Routine *
//* This is the ISR for both the TX interrupt and the RX interrupt *
// PODE SER USADO PARA ACIONAMENTO DA BOMBA
//*********************************************************************************
#pragma vector=USCI_A0_VECTOR
__interrupt void UART_ISR(void)
{
  int i = 0;

  P1OUT = BIT0 | BUTTON11;    // Turn on the red led

  for(i = 0; i < 5000; i++);

  P1OUT = BUTTON11;

  if(UCA0RXBUF = cmd)      // If the messaged received at P4.3 is great that 0x3A
  {
    P9OUT = GREEN_ON;     // Turn on the green LED
    //Aciona bomba:
  }
  else
  {
    P9OUT = GREEN_OFF;      //Else the green LED should be off
  }


  UCA0IFG = UCA0IFG & (~UCRXIFG); // Clear RX Interrupt FlaG
}
//*********************************************************************************
//* Enviar dados UART *
//*********************************************************************************
void send_uart_data(int value){
  UCA0TXBUF = value;
  Serial.println("value <%d> successfully sent", value);
}

//*********************************************************************************
//* Select Clock Signals *
//*********************************************************************************
void select_clock_signals(void)
{
  CSCTL0 = 0xA500; // "Password" to access clock calibration registers
  CSCTL1 = 0x0046; // Specifies frequency of main clock
  CSCTL2 = 0x0133; // Assigns additional clock signals
  CSCTL3 = 0x0000; // Use clocks at intended frequency, do not slow them down
}
//*********************************************************************************
//* Used to Give UART Control of Appropriate Pins *
//*********************************************************************************
void assign_pins_to_uart(void)
{
  P4SEL1 = 0x00; // 0000 0000
  P4SEL0 = BIT3 | BIT2; // 0000 1100
  // ^^
  // ||
  // |+---- 01 assigns P4.2 to UART Transmit (TXD)
  // |
  // +----- 01 assigns P4.3 to UART Receive (RXD)
}
//*********************************************************************************
//* Specify UART Baud Rate *
//*********************************************************************************
void use_9600_baud(void)
{
  UCA0CTLW0 = UCSWRST; // Put UART into SoftWare ReSeT
  UCA0CTLW0 = UCA0CTLW0 | UART_CLK_SEL; // Specifies clock source for UART
  UCA0BR0 = BR0_FOR_9600; // Specifies bit rate (baud) of 9600
  UCA0BR1 = BR1_FOR_9600; // Specifies bit rate (baud) of 9600
  UCA0MCTLW = CLK_MOD; // "Cleans" clock signal
  UCA0CTLW0 = UCA0CTLW0 & (~UCSWRST); // Takes UART out of SoftWare ReSeT
}

void enable_TA0CTL(void)
{
  TA0CTL = ACLK | UP;       // Set ACLK, UP MODE
  TA0CCTL0 = CCIE;        // Enable interrupt for Timer_0
}
void set_timer0_count(int x)
{
  TA0CCR0 = x;            //Used to set the timer0 counter
}
