
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <stdio.h>

#define LOWER_LIMIT 90
#define UPPER_LIMIT 100
#define FACTOR_CORRECCION 50
#define MIN_DIF 1

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareTask(Avance);
DeclareTask(Final);

/*--------------------------------------------------------------------------*/
/* Definitions                                                              */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(){}

int speedB = UPPER_LIMIT;
int speedC = UPPER_LIMIT;
int revB;
int revC;

char velocidades[20];
//char revs[20];
/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Avance                                                         */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Calibrate servomotors function                                */
/*--------------------------------------------------------------------------*/
TASK(Avance)
{
   int time_out = systick_get_ms() + 100000 ; //Calculamos ?? segundos
    
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
    
    int dif=0;

    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out)
    {
      revB = nxt_motor_get_count(NXT_PORT_B);
      revC = nxt_motor_get_count(NXT_PORT_C);
      sprintf(velocidades, "%i %i\n %i %i\n", speedB, speedC, revB, revC);
      display_clear(0);
      display_goto_xy(0, 0);
      display_string(velocidades);
      
      
      dif = revB - revC;
      if(dif<0)
	dif = -dif;
      display_int(dif, 6);
      display_string("\n");
      dif = dif/FACTOR_CORRECCION;

      display_int(dif, 6);

      
      if(dif >= MIN_DIF)
      {
	if(revB>revC)
	{
	  if(speedC<UPPER_LIMIT)
	  {
	    speedC = speedC +  dif;
	    if(speedC > UPPER_LIMIT)
	    {
	      speedB = speedB - (speedC-UPPER_LIMIT);
	      speedC = UPPER_LIMIT;
	    }
	  }
	  else
	  {
	    speedB = speedB - dif;
	  }
	  
	  if(speedB < LOWER_LIMIT)
	  {
	    speedB = LOWER_LIMIT;
	  }
	}
	else if(revC>revB)
	{
	  if(speedB<UPPER_LIMIT)
	  {
	    speedB = speedB +  dif;
	    if(speedB > UPPER_LIMIT)
	    {
	      speedC = speedC -  (speedB-UPPER_LIMIT);
	      speedB = UPPER_LIMIT;
	    }
	  }
	  else
	  {
	    speedC  = speedC - dif;
	  }
	  
	  if(speedC < LOWER_LIMIT)
	  {
	    speedC = LOWER_LIMIT;
	  }
	}

      }
      
      nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
      nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
      display_update();
    }

    // Despierta la tarea de parada de motores
    ActivateTask(Final);

    // Termina la tarea actual
    TerminateTask();
    
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Final                                                          */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ?? 	                                                    */
/* Objective: Stop servomotors                                              */
/*--------------------------------------------------------------------------*/
TASK(Final)
{
    // Para los motores
    nxt_motor_set_speed(NXT_PORT_B, 0, 0);
    nxt_motor_set_speed(NXT_PORT_C, 0, 0);

    // Termina la tarea actual
    TerminateTask();

}
