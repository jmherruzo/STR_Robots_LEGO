
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
DeclareCounter(Contador);
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
TASK(Avance)
{

   // Activar servomotores para avanzar en linea recta

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

   // Activar servomotores para avanzar en linea recta

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

    // Activar servomotores para realizar un giro a la izquierda

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

	// Activar servomotores para realizar un giro a la derecha

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
