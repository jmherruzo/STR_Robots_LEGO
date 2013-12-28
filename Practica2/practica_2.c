
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"
#include <math.h>

#define RADS 57.2957795
#define RUEDA 17.5
#define UPPER_LIMIT 100
#define LIMITE_CERCA 15
#define LIMITE_LEJOS 18
#define DIS_MEDIA 17
#define SPEEDB 49
#define SPEEDC 50
#define DIR_DERECHA 0
#define MAX_CORRECCION 3
#define DIR_IZQUIERDA 1
#define DIR_REC_DER 2
#define DIR_REC_IZ 3
#define SONAR_PORT NXT_PORT_S1
#define PULSADOR1_PORT NXT_PORT_S2
/*--------------------------------------------------------------------------*/
/* OSEK declarations                                                        */
/*--------------------------------------------------------------------------*/
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
      

      
      //nxt_motor_set_speed(NXT_PORT_B, 100, 1);    
      //nxt_motor_set_speed(NXT_PORT_C, 100, 1);
      
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
      display_int(correccionB,5);
      display_int(correccionC,5);
      display_string("\n");
      display_int(ecrobot_get_touch_sensor(PULSADOR1_PORT), 1);
      
      display_update();
  
	
	
	//Ajustamos la velocidad base basandonos a la distancia a la pared
	
	distancia = ecrobot_get_sonar_sensor(SONAR_PORT);
	distancia=(ecrobot_get_sonar_sensor(SONAR_PORT)+distancia)/2;
	rev = (nxt_motor_get_count(NXT_PORT_C)+nxt_motor_get_count(NXT_PORT_B))/2;
	int alejamiento = distancia - distancia2;
	if(ecrobot_get_touch_sensor(PULSADOR1_PORT)==1)
	{
	    direccion = DIR_IZQUIERDA;
	    SetEvent(Avance, Giro);
	}else if(distancia<LIMITE_LEJOS && distancia>LIMITE_CERCA && distancia2 > LIMITE_LEJOS && distancia3 > LIMITE_LEJOS)
	{
	    direccion = DIR_REC_IZ;
	    SetEvent(Avance, Giro);	  	  
	}	
	else if(distancia<LIMITE_LEJOS && distancia>LIMITE_CERCA && distancia2 < LIMITE_CERCA && distancia3< LIMITE_CERCA)
	{
	    direccion = DIR_REC_DER;
	    SetEvent(Avance, Giro);	  	  
	}
	else if (distancia>LIMITE_LEJOS || distancia < LIMITE_CERCA || (distancia < LIMITE_LEJOS && alejamiento<0))
	{
	  
	  if(distancia < LIMITE_CERCA || (distancia < LIMITE_LEJOS && alejamiento<0)) //Corrige hacia fuera
	  {
	    ecrobot_sound_tone(1200, 75, 75);
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
	  }else if(alejamiento<20 && alejamiento>=0) //Corrige hacia dentro
	  {
	    ecrobot_sound_tone(200, 75, 75);
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
	  }else if(alejamiento >20)
	  {
	    direccion = DIR_DERECHA;
	    distancia2 = 14;
	    SetEvent(Avance, Giro);
	    
	  }
	  distancia3 = distancia2;
	  distancia2 = distancia;
	  rev3 = rev2;
	  rev2 = rev;
	  nxt_motor_set_speed(NXT_PORT_B, SPEEDB-correccionB, 1);    
	  nxt_motor_set_speed(NXT_PORT_C, SPEEDC-correccionC, 1);
	}	
	else 
	{
	  correccionB = 0;
	  correccionC = 0;
	}
	

	
	//Termina la tarea actual
	TerminateTask();
}


TASK(RecolocarDer)
{
  int dif = distancia3-distancia;
  int difRevs = rev-rev3;
  float dis = ((float)difRevs/360.0)*(float)RUEDA;
  float angulo = asin(dif/dis);
  int grados = angulo*RADS;

  nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
  nxt_motor_set_speed(NXT_PORT_C, 0, 1);
	  
  systick_wait_ms(500);
	  
  nxt_motor_set_count(NXT_PORT_B,0);
  nxt_motor_set_count(NXT_PORT_C,0);
	  
  // Activar servomotores para realizar un giro a la izquierda
  nxt_motor_set_speed(NXT_PORT_B, SPEEDB, 1);    
  nxt_motor_set_speed(NXT_PORT_C, -SPEEDC, 1);
	  

  int revB = nxt_motor_get_count(NXT_PORT_B);    

  while(revB < (grados*2))
  {
    revB = nxt_motor_get_count(NXT_PORT_B);
  }

  nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
  nxt_motor_set_speed(NXT_PORT_C, 0, 1);    
	  
	  
  nxt_motor_set_count(NXT_PORT_B,0);
  nxt_motor_set_count(NXT_PORT_C,0);
	 
  distancia3 = DIS_MEDIA;
  distancia2 = DIS_MEDIA;
  rev2 = 0;
  rev3 = 0;
	  
    systick_wait_ms(500);
    TerminateTask();
}

TASK(RecolocarIz)
{
  int dif = distancia-distancia3;
  int difRevs = rev-rev3;
  float dis = ((float)difRevs/360.0)*(float)RUEDA;
  float angulo = asin(dif/dis);
  int grados = angulo*RADS;

  nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
  nxt_motor_set_speed(NXT_PORT_C, 0, 1);
	  
  systick_wait_ms(500);
	  
  nxt_motor_set_count(NXT_PORT_B,0);
  nxt_motor_set_count(NXT_PORT_C,0);
	  
  // Activar servomotores para realizar un giro a la izquierda
  nxt_motor_set_speed(NXT_PORT_B, -SPEEDB, 1);    
  nxt_motor_set_speed(NXT_PORT_C, SPEEDC, 1);
	  

  int revC = nxt_motor_get_count(NXT_PORT_C);    

  while(revC < (grados*2))
  {
    revC = nxt_motor_get_count(NXT_PORT_C);
  }

  nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
  nxt_motor_set_speed(NXT_PORT_C, 0, 1);    
	  
	  
  nxt_motor_set_count(NXT_PORT_B,0);
  nxt_motor_set_count(NXT_PORT_C,0);
	 
  distancia3 = DIS_MEDIA;
  distancia2 = DIS_MEDIA;
  rev2 = 0;
  rev3 = 0;
	  
    systick_wait_ms(500);
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

  
    int time_out = systick_get_ms() + 900 ; //Calculamos ?? segundos

    while(systick_get_ms()<time_out)
    {
    }
        
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    
    systick_wait_ms(500);
    
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, -SPEEDB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, SPEEDC, 1);
    
    int revC = nxt_motor_get_count(NXT_PORT_C);    

    while(revC < 170)
    {
      revC = nxt_motor_get_count(NXT_PORT_C);
    }

    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);    
    
    
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
    
    systick_wait_ms(500);
    
    nxt_motor_set_speed(NXT_PORT_B, SPEEDB-correccionB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, SPEEDC-correccionC, 1);
    
    
    time_out = systick_get_ms() + 1000 ; //Calculamos ?? segundos
    
    while(systick_get_ms()<time_out)
    {
    }
    
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
  
    nxt_motor_set_speed(NXT_PORT_B, -SPEEDB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, -SPEEDC, 1);
    
    int dist2 = ecrobot_get_sonar_sensor(SONAR_PORT);
    dist2=(ecrobot_get_sonar_sensor(SONAR_PORT)+distancia)/2;
    
    int time_out = systick_get_ms() + 1000 ; //Calculamos ?? segundos
    
    while(systick_get_ms()<time_out)
    {
    }
    
    //Cálculos para ajustar el giro
    int countR = -(nxt_motor_get_count(NXT_PORT_B)+nxt_motor_get_count(NXT_PORT_C))/2;
    int dist = ecrobot_get_sonar_sensor(SONAR_PORT);
    dist=(ecrobot_get_sonar_sensor(SONAR_PORT)+distancia)/2;
    
    int dif = dist2-dist;
    float dis = ((float)countR/360.0)*(float)RUEDA;
    float angulo = asin(dif/dis);
    int grados = angulo*RADS;
    
    
    nxt_motor_set_speed(NXT_PORT_B, 0, 1);    
    nxt_motor_set_speed(NXT_PORT_C, 0, 1);
    
    systick_wait_ms(500);
    
    nxt_motor_set_count(NXT_PORT_B,0);
    nxt_motor_set_count(NXT_PORT_C,0);
    
    // Activar servomotores para realizar un giro a la izquierda
    nxt_motor_set_speed(NXT_PORT_B, SPEEDB, 1);    
    nxt_motor_set_speed(NXT_PORT_C, -SPEEDC, 1);
    
    int revB = nxt_motor_get_count(NXT_PORT_B);    

    while(revB < (170+grados*2))
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

