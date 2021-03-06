
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <stdio.h>


#define UPPER_LIMIT 100

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(Contador);


DeclareTask(Avance);
DeclareTask(Final);
DeclareTask(Correccion);



/*--------------------------------------------------------------------------*/
/* Definitions                                                              */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(){
  	//inicializamos el contador
	SignalCounter(Contador);
}

//Inicializamos las velocidades. Una más rapida que otra debido a la diferencia de motores
int speedB = 98;
int speedC = 100;

char str[50];

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Avance                                                         */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                   */
/* Objective: Calibrate servomotors function                                */
/*--------------------------------------------------------------------------*/
TASK(Avance)
{
   int time_out = systick_get_ms() + 10000 ; //Calculamos ?? segundos
    
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
  
    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out)
    {

	nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
	nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
    }

    // Despierta la tarea de parada de motores
    ActivateTask(Final);

    // Termina la tarea actual
    TerminateTask();
    
}



/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Correccion                                                     */
/* Priority: 2                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL	                                                   */
/* Objective: Correct the servomotors speed                                 */
/*--------------------------------------------------------------------------*/
TASK(Correccion)
{
	//Obtenemos las revoluciones
	int revB = nxt_motor_get_count(NXT_PORT_B);
	int revC = nxt_motor_get_count(NXT_PORT_C);
	
	
	
	//Comprobamos y corregimos la velocidad de los motores
	if(revB > revC)
	{
	  if(speedC<UPPER_LIMIT)
	    speedC++;
	  else
	    speedB--;
	}
	else if(revC > revB)
	{
	  if(speedB<UPPER_LIMIT)
	    speedB++;
	  else
	    speedC--;	  
	}
	
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);
	
	//Termina la tarea actual
	TerminateTask();
}


/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Final                                                          */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ?? 	                                                           */
/* Objective: Stop servomotors                                              */
/*--------------------------------------------------------------------------*/
TASK(Final)
{
	sprintf(str, "Vel. B: %i \n Vel. C: %i\n", speedB, speedC);
	display_string(str);
	display_update();
  
    // Para los motores
    nxt_motor_set_speed(NXT_PORT_B, 0, 0);
    nxt_motor_set_speed(NXT_PORT_C, 0, 0);    
    
    // Termina la tarea actual
    TerminateTask();

}
