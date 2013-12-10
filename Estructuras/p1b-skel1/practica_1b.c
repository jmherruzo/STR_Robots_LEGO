
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareTask(Retroceso);
DeclareTask(Avance);
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

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Retroceso                                                      */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Activate servomotors backward, taking into acount calibration.*/
/*--------------------------------------------------------------------------*/
TASK(Retroceso)
{

   // Determinar cuanto tiempo va a estar en retroceso


   // Activar servomotores para retroceder en linea recta


   // Esperar hasta la finalizacion del tiempo de retroceso


   // Terminar la tarea actual

}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Avance                                                         */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Activate servomotors forward, taking into acount calibration. */
/*--------------------------------------------------------------------------*/
TASK(Avance)
{

   // Determinar cuanto tiempo va a estar en avance


   // Activar servomotores para avanzar en linea recta


   // Esperar hasta la finalizacion del tiempo de retroceso   


   // Terminar la tarea actual

}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : GiroIzq                                                        */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Activate servomotors to make a left corner, taking into       */ 
/*            acount calibration.	    				    */
/*--------------------------------------------------------------------------*/
TASK(GiroIzq)
{

    // Determinar cuanto tiempo va a estar girando.


    // Activar servomotores para realizar un giro a la izquierda


   // Esperar hasta la finalizacion del tiempo de giro


    // Terminar la tarea actual
      
}


/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : GiroDer                                                        */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Activate servomotors to make a right corner, taking into      */ 
/*            acount calibration.	    				    */
/*--------------------------------------------------------------------------*/
TASK(GiroDer)
{

    // Determinar cuanto tiempo va a estar girando.


    // Activar servomotores para realizar un giro a la derecha


   // Esperar hasta la finalizacion del tiempo de giro


    // Terminar la tarea actual
    
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Final                                                          */
/* Priority: ??                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: ??		                                                    */
/* Objective: Stop servomotors						    */
/*--------------------------------------------------------------------------*/
TASK(Final)
{
    // Parar motores

    // Terminar la tarea actual

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

    // Inicializar variables


    // Organizar las tareas


    // Esperar que todas finalicen


    // Terminar la tarea actual

}
