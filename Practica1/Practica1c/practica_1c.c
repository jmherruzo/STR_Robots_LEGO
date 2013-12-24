
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define UPPER_LIMIT 100
#define SPEEDB 98
#define SPEEDC 100
#define SONAR_PORT NXT_PORT_S1
#define PULSADOR1_PORT NXT_PORT_S2
/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(Contador);
DeclareTask(Principal);
DeclareTask(Avance);
DeclareTask(Correccion);
DeclareTask(Retroceso);
DeclareTask(CambioDir);

void ecrobot_device_initialize()
{
	// Inicializar los sensores activos
	ecrobot_init_sonar_sensor(SONAR_PORT);
}
void ecrobot_device_terminate()
{
	// Finalizar los sensores activos
	ecrobot_term_sonar_sensor(SONAR_PORT);
}

int speedB;
int speedC;
S32 distancia;


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

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Avance                                                         */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Move forward.						    */
/*--------------------------------------------------------------------------*/
TASK(Avance)
{     
     
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
  
    
    // Espera hasta que se agote el time_out
    while(1)
    {
      systick_wait_ms(200);
      display_clear(0);
      display_goto_xy(0,0);
      display_int(distancia, 5);
      display_string("\n");
      display_int(speedB,5);
      display_int(speedC,5);

      display_update();
      nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
      nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
    }

    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Correccion                                                      */
/* Priority: 3                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Corrige movimiento	         				    */
/*--------------------------------------------------------------------------*/

TASK(Correccion)
{
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
	
	
	if(ecrobot_get_touch_sensor(PULSADOR1_PORT)>0)
	{
	  ActivateTask(Retroceso);
	}
	
	distancia = ecrobot_get_sonar_sensor(SONAR_PORT);
	
	if(distancia < 20 && speedB > 73 && speedB < 85)
	{
	  speedB = speedB - 10;
	  speedC = speedC - 10;
	}
	else if(distancia < 100  && speedB > 90)
	{
	  speedB = speedB - 20;
	  speedC = speedC - 20;
	}
	
	//Termina la tarea actual
	TerminateTask();
}


/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Retroceso                                                      */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                   */
/* Objective: Move backward.						 */
/*--------------------------------------------------------------------------*/

TASK(Retroceso)
{
    int time_out = systick_get_ms() + 1000 ; //Calculamos ?? segundos
    
    speedB = -SPEEDB;
    speedC = -SPEEDC;
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
    
  
    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out)
    {
    }
    speedB = -SPEEDB;
    speedC = -SPEEDC;
    
    //ActivateTask(CambioDir);
    

    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : CambioDir		                                            */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Change direction of the robot.				    */
/*--------------------------------------------------------------------------*/

TASK(CambioDir)
{
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
  
    systick_wait_ms(200);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, -SPEEDB+15, 1);    
    nxt_motor_set_speed(NXT_PORT_C, SPEEDC-15, 1);
    
    int revC = nxt_motor_get_count(NXT_PORT_C);    
    // Espera hasta que se agote el time_out
    while(revC < 165)
    {
      revC = nxt_motor_get_count(NXT_PORT_C);
    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);    
    
    // Terminar la tarea actual
    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Principal                                                      */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Initialize variables and organize tasks.			    */
/*--------------------------------------------------------------------------*/
TASK(Principal)
{
  speedB = SPEEDB;
  speedC = SPEEDC;
  systick_wait_ms(1000);
  ActivateTask(Avance);
  TerminateTask();
}
