#include "io430.h"

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  BCSCTL1 = CALBC1_1MHZ; //SMCLK= DCO = 1MHz T=1us
  DCOCTL  = CALDCO_1MHZ;
  
  TACTL = TASSEL_2+MC_2; //SMCLK TAR(Continuo)
  
  //La detección de la señal de sincronía
  TACCTL0 = CM_3+CAP+CCIE; //CM_3(Detectar ambos flancos Subida y bajada)
  P1SEL_bit.P1 = 1; //TA0.0 <-- Entrada de Captura(P1.1)     TAR --> TACCR0
  P1DIR_bit.P1 = 0;
  
  //Señal de disparo
  TACCTL1 = CCIE;  //TACCR1
  P2DIR_bit.P0 = 1; //P2.0 Salida del disparo se conecta al MOC3020
  P2OUT_bit.P0 = 0;
  
  __bis_SR_register(GIE);//Permiso a nivel Global GIE = 1

  while(1)
  {}
}
#pragma vector=TIMER0_A0_VECTOR //TACCR0
__interrupt void Deteccion_Flanco(void)
{
  TACCR1 = TACCR0 + 6000; // 0.5ms a 8ms
}
// Timer0_A2 Interrupt Vector (TA0IV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Tigger(void)
{
  switch( TA0IV )
  {
    case  2:  P2OUT_bit.P0 = 1; //TACCR1
              __delay_cycles(50);
              P2OUT_bit.P0 = 0;
              break;    
    case 10:            // TAIFG no se usa
              break;
  }
}



