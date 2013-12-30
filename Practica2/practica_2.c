#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <math.h>
#include <practica2.h>

/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
//Declaramos tareas, contador y evento:
DeclareCounter(Contador);
DeclareTask(Avance);
DeclareTask(Correccion);
DeclareTask(Derecha);
DeclareTask(Izquierda);
DeclareTask(RecolocarIz);
DeclareTask(RecolocarDer);

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

//Variables globales:
int speedB;
int speedC;
int correccionB=0, correccionC=0;
S32 distancia, distancia2=DIS_MEDIA, distancia3=DIS_MEDIA;

int rev, rev2, rev3;

int correccion=-1;
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
/* Priority: 1                                                              */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                           */
/* Objective: Espera a que se active el evento giro y ejecuta la tarea	*/
/* que sea necesario dependiendo del valor de 'direccion'			*/
/*--------------------------------------------------------------------------*/
TASK(Avance)
{     
    ClearEvent(Giro);

    while(1)
    {
      WaitEvent(Giro);
      ClearEvent(Giro);
      if(direccion == DIR_DERECHA)
	ActivateTask(Derecha);
      else if(direccion == DIR_IZQUIERDA)
	ActivateTask(Izquierda);
      else if(direccion == DIR_REC_DER)
	ActivateTask(RecolocarDer);
      else if(direccion == DIR_REC_IZ)
	ActivateTask(RecolocarIz);
      direccion = -1;
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
	//Ajustamos la velocidad base basandonos a la distancia a la pared
	distancia = getDistancia();
	rev = getMediumRevs();
	int alejamiento = distancia - distancia2;
	
	
	if(ecrobot_get_touch_sensor(PULSADOR1_PORT)==1)
	{//Comprobamos si se ha pulsado el boton frontal
		direccion = DIR_IZQUIERDA;
		SetEvent(Avance, Giro);
	}
	else if(distancia<LIMITE_LEJOS && distancia>LIMITE_CERCA && distancia2 > LIMITE_LEJOS && distancia3 > LIMITE_LEJOS)
	{ //Correccion acercandose a la pared
		correccionB = 0;
		correccionC = 0;
		direccion = DIR_REC_IZ;
		SetEvent(Avance, Giro);	  	  
	}	
	else if(distancia<LIMITE_LEJOS && distancia>LIMITE_CERCA && distancia2 < LIMITE_CERCA && distancia3< LIMITE_CERCA)
	{ //Correccion alejandose de la pared
		correccionB = 0;
		correccionC = 0;
		direccion = DIR_REC_DER;
		SetEvent(Avance, Giro);	  	  
	}
	else if (distancia>LIMITE_LEJOS || distancia < LIMITE_CERCA || (distancia < LIMITE_LEJOS && alejamiento<0))
	{		
		if(distancia < LIMITE_CERCA || (distancia < LIMITE_LEJOS && alejamiento<0)) //Corrige hacia fuera
		{
			ecrobot_sound_tone(1200, 75, 75);
			correccionIzquierda();
		}
		else if(alejamiento<20 && alejamiento>=0 && distancia<LIMITE_MAX) //Corrige hacia dentro
		{
			ecrobot_sound_tone(200, 75, 75);
			correccionDerecha();
		}
		else if(alejamiento >20 || distancia>= LIMITE_MAX)//Gira hacia la derecha
		{
			direccion = DIR_DERECHA;
			distancia2 = DIS_MEDIA;
			SetEvent(Avance, Giro);
		}

		//Actualizamos valores antiguos:
		distancia3 = distancia2;
		distancia2 = distancia;
		rev3 = rev2;
		rev2 = rev;

		setVelocidad(SPEEDB-correccionB, SPEEDC-correccionC);
	}	
	else 
	{
	  correccionB = 0;
	  correccionC = 0;
	}
	//Termina la tarea actual
	TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : RecolocarIz                                                      */
/* Priority: 3                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Recoloca el robot hacia la izquierda 						*/
/*--------------------------------------------------------------------------*/
TASK(RecolocarIz)
{
	//Cálculo de grados a corregir
  int dif = distancia3-distancia;
  int difRevs = rev-rev3;
  int grados = calcularGradosCorreccion(dif, difRevs);

  //Se para durante 500ms
  setVelocidad(0,0); 
  systick_wait_ms(500);
  resetMotorCounters();	  
  
  // Activar servomotores para realizar un giro a la izquierda
  setVelocidad(SPEEDB, -SPEEDC);
	  

  int revB = nxt_motor_get_count(NXT_PORT_B);    

  while(revB < (grados*1.5)+10)
  {
    revB = nxt_motor_get_count(NXT_PORT_B);
  }
	setVelocidad(0,0);
	resetMotorCounters();
	resetOldValues();
	  
    systick_wait_ms(500);
    TerminateTask();
}
/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : RecolocarDer                                                      */
/* Priority: 3                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Recoloca el robot hacia la derecha 						*/
/*--------------------------------------------------------------------------*/
TASK(RecolocarDer)
{
	//Calculo de grados:
  int dif = distancia-distancia3;
  int difRevs = rev-rev3;
  int grados = calcularGradosCorreccion(dif, difRevs);
  
  //Se para durante 500ms
	setVelocidad(0,0);
	systick_wait_ms(500);
	resetMotorCounters();  
  
  // Activar servomotores para realizar un giro a la izquierda
	setVelocidad(-SPEEDB, SPEEDC);
	int revC = nxt_motor_get_count(NXT_PORT_C);    
	
  while(revC < (grados*1.5)+10) //Hasta que este girado lo suficiente
  {
    revC = nxt_motor_get_count(NXT_PORT_C);
  }
	//Vuelve a parar los motores y a esperar 500ms:
	setVelocidad(0,0);
	resetMotorCounters();
	systick_wait_ms(500);
	
    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Derecha		                                            */
/* Priority: 3                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Gira el robot hacia la derecha								*/
/*--------------------------------------------------------------------------*/

TASK(Derecha)
{
	//Esperamos a que avance lo suficiente
    systick_wait_ms(900);

    //Paramos medio segundo
	setVelocidad(0,0);
	systick_wait_ms(500);
    resetMotorCounters();
    
    // Activar servomotores para realizar un giro a la izquierda
	setVelocidad(-SPEEDB, SPEEDC);    
    int revC = nxt_motor_get_count(NXT_PORT_C);    

    while(revC < 170)
    {
      revC = nxt_motor_get_count(NXT_PORT_C);
    }
	
	setVelocidad(0,0);
    resetMotorCounters();
    
    systick_wait_ms(500);
	setVelocidad(SPEEDB-correccionB, SPEEDC-correccionC);
    
    systick_wait_ms(1000);
    
    // Terminar la tarea actual
    TerminateTask();
}

/*--------------------------------------------------------------------------*/
/* Task information:                                                        */
/* -----------------                                                        */
/* Name    : Izquierda		                                            */
/* Priority: 3                                                             */
/* Typ     : EXTENDED TASK                                                  */
/* Schedule: FULL		                                                    */
/* Objective: Gira el robot hacia la izquierda							    */
/*--------------------------------------------------------------------------*/

TASK(Izquierda)
{
  
	resetMotorCounters();
	setVelocidad(-SPEEDB, -SPEEDC);
    int dist2 = getDistancia();
    
	systick_wait_ms(1000);
    
    //Cálculos para ajustar el giro
    int countR = -(nxt_motor_get_count(NXT_PORT_B)+nxt_motor_get_count(NXT_PORT_C))/2;
    int dist = getDistancia();
    int dif = dist2-dist;
	int grados = calcularGradosCorreccion(dif, countR);
 
    //Esperamos medio segundo
    setVelocidad(0,0);
    systick_wait_ms(500);
    resetMotorCounters();
    
    // Activar servomotores para realizar un giro a la izquierda
	setVelocidad(SPEEDB, -SPEEDC);
    
    int revB = nxt_motor_get_count(NXT_PORT_B);    

    while(revB < (170+grados*2))
    {
      revB = nxt_motor_get_count(NXT_PORT_B);
    }
    
    //Esperamos medio segundo    
	setVelocidad(0, 0);
    resetMotorCounters();
    systick_wait_ms(500);
	
    // Terminar la tarea actual
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

void correccionIzquierda()
{	
  if(correccion==DIR_DERECHA)
  {
    correccionB=0;
    correccionC=0;
  }else if(correccionB>0)
    correccionB--;
  else
    correccionC++;
  correccion = DIR_IZQUIERDA;
  if(correccionC>MAX_CORRECCION)
    correccionC = MAX_CORRECCION;  
}

void correccionDerecha()
{
	if(correccion==DIR_IZQUIERDA)
	{
		correccionB=0;
		correccionC=0;
	}else if(correccionC>0)
		correccionC--;
	else
		correccionB++;
	correccion = DIR_DERECHA;
	if(correccionB>MAX_CORRECCION)
		correccionB = MAX_CORRECCION; 
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

int calcularGradosCorreccion(int difDistancia, int difRevs)
{
	float dis = ((float)difRevs/360.0)*(float)RUEDA;
	float angulo = asin(difDistancia/dis);
	return (angulo*RADS);
}

void resetOldValues()
{
	distancia3 = DIS_MEDIA;
	distancia2 = DIS_MEDIA;
	rev2 = 0;
	rev3 = 0;
}
