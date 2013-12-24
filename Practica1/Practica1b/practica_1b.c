
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareTask(Avance);
DeclareTask(Avance2);
DeclareTask(GiroIzq);
DeclareTask(GiroDer);
DeclareTask(Final);
DeclareTask(Principal);

/*--------------------------------------------------------------------------*/
/* Definitions                                                              */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Function to be invoked from a category 2 interrupt                       */
/*--------------------------------------------------------------------------*/
void user_1ms_isr_type2(){}

int time;
int speedB = 73;
int speedC = 75;


/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Avance                                                         */
/* Priority: 1                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                          */
/* Objective: Avanza un tiempo determinado hacia delante y despues gira     */
/* a la izquierda								*/
/*--------------------------------------------------------------------------*/
TASK(Avance)
{    
     int time_out = systick_get_ms() + time ;
     
     
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
  
    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out)
    {

    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    // Despierta la tarea de parada de motores
    ActivateTask(GiroIzq);
    
   // Terminar la tarea actual
    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Avance                                                         */
/* Priority: 1                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Avanza hacia delante y después activa el giro derecha         */
/*--------------------------------------------------------------------------*/
TASK(Avance2)
{    
     int time_out = systick_get_ms() + 800 ;
     
     systick_wait_ms(200);
     
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
  
    display_int(speedB, 3);
    display_int(speedC, 3);
    display_update();
    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out)
    {

    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    // Despierta la tarea de parada de motores
    ActivateTask(GiroDer);
    
   // Terminar la tarea actual
    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : GiroIzq                                                        */
/* Priority: 1                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                           */
/* Objective: Gira 90º a la izquierda       				 */
/*--------------------------------------------------------------------------*/
TASK(GiroIzq)
{
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
  
    systick_wait_ms(200);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, -speedC, 1);
    
    int revB = nxt_motor_get_count(NXT_PORT_B);    
    // Espera hasta que se agote el time_out
    while(revB < 170)
    {
      revB = nxt_motor_get_count(NXT_PORT_B);
    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
        
    ActivateTask(Avance2);
    
    // Terminar la tarea actual
    TerminateTask();
}


/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : GiroDer                                                        */
/* Priority:  1                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL                                                           */
/* Objective: Gira 90º a la derecha	    				 */
/*--------------------------------------------------------------------------*/
TASK(GiroDer)
{

    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
  
    systick_wait_ms(200);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, -speedB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, speedC, 1);
    
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
/* Priority: 1                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                           */
/* Objective: Initialize variables and organize tasks.			 */
/*--------------------------------------------------------------------------*/
TASK(Principal)
{

  time = 1450;
  systick_wait_ms(1000);
  ActivateTask(Avance);
 
  TerminateTask();
}
