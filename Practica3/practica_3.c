#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <math.h>
#include <practica3.h>

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
//Declaramos tareas, contador y evento:
DeclareCounter(Contador);
DeclareTask(Avance);
DeclareTask(Correccion);




void ecrobot_device_initialize()
{
	// Inicializar los sensores activos
	//ecrobot_init_sonar_sensor(SONAR_PORT);
}
void ecrobot_device_terminate()
{
	// Finalizar los sensores activos
	//ecrobot_term_sonar_sensor(SONAR_PORT);
}

//Variables globales
int ref = -1;
int light, light_old = 0, gyro;
int maxDif = 0, counter=0;
int ultimaDireccion=0;
int velocidad;
int gyro_offset = 602;


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
/* Priority: 1                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                           */
/* Objective: Espera a que se active el evento giro y ejecuta la tarea	*/
/* que sea necesario dependiendo del valor de 'direccion'			*/
/*--------------------------------------------------------------------------*/
TASK(Avance)
{     
	ecrobot_set_light_sensor_active(LIGHT_PORT);
	int aux;
	int max_gyro=0;
	setVelocidad(0,0);
    while(1)
    {
		systick_wait_ms(200);
		aux = ecrobot_get_light_sensor(LIGHT_PORT);
		if(gyro>max_gyro)
			max_gyro = gyro;
		display_clear(0);
		display_goto_xy(0,0);
		display_string("Referencia: ");
		display_int(ref, 4);
		display_string("\nActual: ");
		display_int(aux,4);
		if(ultimaDireccion==0)
			display_string("\nQuieto\n");
		else if(ultimaDireccion==ADELANTE)
			display_string("\nAdelante\n");
		else if(ultimaDireccion==ATRAS)
			display_string("\nAtras\n");
		display_int(max_gyro, 4);
		display_update();
    }

    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Correccion                                                      */
/* Priority: 2                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Corrige movimiento	         				    */
/*--------------------------------------------------------------------------*/
TASK(Correccion)
{
	int touch = ecrobot_get_touch_sensor(PULSADOR1_PORT);
	if(ref==-1 && touch==1)
	{
		systick_wait_ms(500);
		ref = ecrobot_get_light_sensor(LIGHT_PORT);
	}
	else if(ref!=-1 && touch==1)
	{
		systick_wait_ms(500);
		ref=-1;
		setVelocidad(0,0);
	} else if(ref!=-1)
	{
		light = ecrobot_get_light_sensor(LIGHT_PORT);
		gyro = ecrobot_get_gyro_sensor(GYRO_PORT)-gyro_offset;
		
		
		if(light>(ref+2)) //Inclinado hacia delante
		{
			if(gyro>0)
				velocidad = MAXSPEED;
			else if(gyro > -GYRO_BAJO)
				velocidad = MAXSPEED * 0.8;
			else if(gyro > -GYRO_MEDIO)
				velocidad = MAXSPEED * 0.6;
			else 
				velocidad = 0;
		}
		else if(light<(ref-2)) //Inclinado hacia atrás
		{
			if(gyro<0)
				velocidad = -MAXSPEED;
			else if(gyro < GYRO_BAJO)
				velocidad = -MAXSPEED * 0.8;
			else if(gyro < GYRO_MEDIO)
				velocidad = -MAXSPEED * 0.6;
			else 
				velocidad = 0;
		}
		else
			velocidad = 0;
		
		setVelocidad(velocidad, velocidad);
			
	}
	
	
	TerminateTask();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 					FUNCIONES PROPIAS
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


int getDistancia()
{
  int auxDist = ecrobot_get_sonar_sensor(SONAR_PORT);
  auxDist=(ecrobot_get_sonar_sensor(SONAR_PORT)+auxDist)/2;
  return auxDist;  
}

int getMediumRevs()
{
  return (nxt_motor_get_count(NXT_PORT_C)+nxt_motor_get_count(NXT_PORT_B))/2; 
}


void setVelocidad(int motorB, int motorC)
{
    nxt_motor_set_speed(NXT_PORT_B, motorB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, motorC, 1);  
}

void resetMotorCounters()
{
	nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
}

void mover(float distancia)
{
	resetMotorCounters();
}

