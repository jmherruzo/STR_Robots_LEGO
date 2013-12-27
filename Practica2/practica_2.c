
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define UPPER_LIMIT 100
#define SPEEDB 80
#define SPEEDC 80
#define DIR_DERECHA 0
#define DIR_IZQUIERDA 1
#define SONAR_PORT NXT_PORT_S1
#define PULSADOR1_PORT NXT_PORT_S2
/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(Contador);
DeclareTask(Derecha);
DeclareTask(Izquierda);
DeclareTask(Avance);
DeclareTask(Correccion);

DeclareEvent(Giro);



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

int direccion=-1;

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
    ClearEvent(Giro);
    // Espera hasta que se agote el time_out
    while(1)
    {
      

      
      nxt_motor_set_speed(NXT_PORT_B, speedB-correccionB, 1);    
      nxt_motor_set_speed(NXT_PORT_C, speedC-correccionC, 1);
      
      WaitEvent(Giro);
      ClearEvent(Giro);
      if(direccion = DIR_DERECHA)
	ActivateTask(Derecha);
      else if(direccion = DIR_IZQUIERDA)
	ActivateTask(Izquierda);
      direccion = -1;
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
	
	
	//Ajustamos la velocidad base basandonos a la distancia a la pared
	
	distancia = ecrobot_get_sonar_sensor(SONAR_PORT);

	
	if(ecrobot_get_touch_sensor(PULSADOR1_PORT)==1)
	{
	  direccion = DIR_IZQUIERDA;
	  SetEvent(Avance, Giro);
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
/* Name    : CambioDir		                                            */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Change direction of the robot.				    */
/*--------------------------------------------------------------------------*/

TASK(Derecha)
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

    while(revC < 160)
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
/* Name    : CambioDir		                                            */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Change direction of the robot.				    */
/*--------------------------------------------------------------------------*/

TASK(Izquierda)
{
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
  
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    
    systick_wait_ms(500);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, 75, 1);    
    nxt_motor_set_speed(NXT_PORT_C, -75, 1);
    
    int revB = nxt_motor_get_count(NXT_PORT_B);    

    while(revB < 160)
    {
      revB = nxt_motor_get_count(NXT_PORT_B);
    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);    
    
    
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
    
    systick_wait_ms(500);
    
    // Terminar la tarea actual
    TerminateTask();
}
