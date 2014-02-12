
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define UPPER_LIMIT 100
#define SPEEDB 100
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

DeclareEvent(TouchSensor);



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
int correccionB, correccionC;
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
    ClearEvent(TouchSensor);
    // Espera hasta que se agote el time_out
    while(1)
    {
      

      
      nxt_motor_set_speed(NXT_PORT_B, speedB-correccionB, 1);    
      nxt_motor_set_speed(NXT_PORT_C, speedC-correccionC, 1);
      
      WaitEvent(TouchSensor);
      ClearEvent(TouchSensor);
      ActivateTask(Retroceso);
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
  
        display_clear(0);
      display_goto_xy(0,0);
      display_int(distancia, 5);
      display_string("\n");
      display_int(speedB-correccionB,5);
      display_int(speedC-correccionC,5);
      display_string("\n");
      display_int(ecrobot_get_touch_sensor(PULSADOR1_PORT), 1);
      
      display_update();
  
	int revB = nxt_motor_get_count(NXT_PORT_B);
	int revC = nxt_motor_get_count(NXT_PORT_C);
	
	//Comprobamos y corregimos la velocidad de los motores
	
	if(revB > revC)
	{
	  if(correccionC>0)
	    correccionC--;
	  else
	    correccionB++;
	}
	else if(revC > revB)
	{
	  if(correccionB>0)
	    correccionB--;
	  else
	    correccionC++;  
	}
	
	nxt_motor_set_count(NXT_PORT_B,0);
	nxt_motor_set_count(NXT_PORT_C,0);
	
	
	//Ajustamos la velocidad base en base a la distancia a la pared
	
	distancia = ecrobot_get_sonar_sensor(SONAR_PORT);
	
	if(distancia < 25)
	{
	  speedB = SPEEDB - 75;
	  speedC = SPEEDC - 75;
	  //ecrobot_sound_tone(1000, 75, 75);
	}
	else if(distancia < 105)
	{
	  speedB = SPEEDB - 50;
	  speedC = SPEEDC - 50;
	}
	else
	{
	  speedB = SPEEDB;
	  speedC = SPEEDC;
	  
	}
	
	
	if(ecrobot_get_touch_sensor(PULSADOR1_PORT)==1)
	{
	  SetEvent(Avance, TouchSensor);
	}
	else
	{
	  nxt_motor_set_speed(NXT_PORT_B, speedB-correccionB, 1);    
	  nxt_motor_set_speed(NXT_PORT_C, speedC-correccionC, 1);
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
    
    speedB = SPEEDB-40;
    speedC = SPEEDC-40;
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, correccionB-speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, correccionC-speedC, 1);
    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out)
    {
    }
    
    ActivateTask(CambioDir);
    

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
  
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    
    systick_wait_ms(500);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, -75, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 75, 1);
    
    int revC = nxt_motor_get_count(NXT_PORT_C);    

    while(revC < 185)
    {
      revC = nxt_motor_get_count(NXT_PORT_C);
    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);    
    
    
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
    
    systick_wait_ms(500);
    
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
  systick_wait_ms(1000);
  ActivateTask(Avance);
  TerminateTask();
}
