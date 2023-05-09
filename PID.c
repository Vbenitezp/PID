#include "io430.h"
#include <stdlib.h>
#include <stdio.h>

char Dmillar, Umillar, Centena, Decena, Unidad;
char pwm;
//char temperatura[5] = {0,0,0,0,0};
char PWM[3];
int i;
unsigned long Vout;
unsigned int Temperatura, temperatura;
int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;

  //DCO Trabajando a 1MHz aprox.
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
  //---------------------------------------------------------------- PWM -----

  //Timer1 , UP, SMCLK
  TA1CTL = TASSEL_2+MC_1;
  TA1CCR0 = 255;       //Valor para tener 255 niveles de brillo
  
  TA1CCTL1 = OUTMOD_7;   //Reset/set
  TA1CCR1 = 0;         //LED ROJO (Brillo entre 0 y 255)

  P2SEL_bit.P1 = 1;     //La terminal P2.1 E/S del sistema temporizador TIMER1
  P2DIR_bit.P1 = 1;     //La terminal P2.1 salida de comparación PWM

  //------------------------------------------------ SE CONFIGURA EL ADC-------------------------------
  ADC10CTL0 = SREF_1+ADC10SHT_2+MSC+REFON+ADC10ON;
  ADC10CTL1 = INCH_5+CONSEQ_2;
  ADC10AE0 |= BIT5; 
  ADC10CTL0 |= ENC+ADC10SC;
  
  //------------------------------------------- CONFIGURACIÓN DEL PUERTO P1.7 COMO SALIDA (FOCO)-------
  P1SEL_bit.P7 = 0;
  P1SEL2_bit.P7 = 0;
  P1DIR_bit.P7 = 1;
  //------------------------------------------------ SE CONFIGURA EL UART------------------------------
  P1SEL  |= BIT1 + BIT2 ;               // Configurando las terminales P1.1 y P1.2
  P1SEL2 |= BIT1 + BIT2 ;               // como parte del sistema UART
  P1DIR &= ~BIT1;                       // P1.1 <-- RXD
  P1DIR |= BIT2;                        // P1.2 --> TXD
  UCA0CTL1 |= UCSSEL_2;                 // SMCLK
  //---------------------------9600bps-------------------------
  UCA0BR0 = 104;                       // 1MHz 9600
  UCA0BR1 = 0;                         // 1MHz 9600
  UCA0MCTL = UCBRS0;                   // Modulation UCBRSx = 1
  
  UCA0CTL1 &= ~UCSWRST;                 // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                      // Enable USCI_A0 RX interrupt
  

  
  __bis_SR_register(GIE); 
  
  while(1)
  {
    Vout = ADC10MEM;                            // SE ESTÁ HACIENDO LA CONVERSIÓN ANALÓGICA DEL VOLTAJE
    Vout = (Vout * 1500)/1023;
  __delay_cycles(50000);
  
  
  Temperatura = Vout;  
  Dmillar = Umillar = Centena = Decena = Unidad = 0x30;
  
  if (Vout <= 225)
    {     P1OUT_bit.P7 =0; }                // PUERTO DE SALIDA DEL ENCENDIDO DEL FOCO (P1.7)
    else
    {     P1OUT_bit.P7 =1; }                  // PUERTO DE SALIDA DEL ENCENDIDO DEL FOCO (P1.7) 
  
    while ( Temperatura >= 1000)
    {
      Temperatura -= 1000;
      Umillar ++;
    }
    while ( Temperatura >=100)
    {
      Temperatura -= 100;
      Centena ++;
    }
    while ( Temperatura >=10)
    {
      Temperatura -=10;
      Decena ++;
    }
    Unidad += Temperatura;
    
    UCA0TXBUF = Centena;
    while (!(IFG2&UCA0TXIFG))              
      {}
    UCA0TXBUF = Decena;
    while (!(IFG2&UCA0TXIFG))              
      {}
    UCA0TXBUF = '.'; 
    while (!(IFG2&UCA0TXIFG))              
      {}

    UCA0TXBUF = Unidad;
    while (!(IFG2&UCA0TXIFG))              
      {}
    UCA0TXBUF = '\n'; 
    while (!(IFG2&UCA0TXIFG))              
      {}
    //UCA0TXBUF = '\r'; 
    //while (!(IFG2&UCA0TXIFG))              
      //{}
    __delay_cycles(50000);
 }
}

 //Echo back RXed character, confirm TX buffer is ready first
#pragma vector = USCIAB0RX_VECTOR         
__interrupt void USCI0RX_ISR(void)
{
    pwm = UCA0RXBUF;
    if (pwm == '*')
      {i = 0;}
    else 
    {
      PWM[i] = pwm;
      i++;
      if (i>=3)
      { 
        temperatura = atoi(PWM);
        TA1CCR1 =  temperatura;
        i=0;
       }
    }  
    
}
  
    //255- TEMPERATURA
    
    
    

