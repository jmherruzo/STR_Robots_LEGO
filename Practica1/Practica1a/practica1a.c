
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

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
   int time_out = systick_get_ms() + 5000 ; //Calculamos ?? segundos
    
    // Activa motores para comprobar si existe desvio en la navegacion
    nxt_motor_set_speed(NXT_PORT_B, 100, 0);    
    nxt_motor_set_speed(NXT_PORT_C, 100, 0);
    
    // Espera hasta que se agote el time_out
    while(systick_get_ms()<time_out);

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
