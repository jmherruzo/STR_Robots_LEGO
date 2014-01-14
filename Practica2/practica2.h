
//Variable para transformar de radianes a grados
#define RADS 57.2957795
//Longitud de la circunferencia de la rueda
#define RUEDA 17.5
//Limite superior de velocidad
#define UPPER_LIMIT 100
//Limites de la "zona de confort"
#define LIMITE_CERCA 15
#define LIMITE_LEJOS 18
#define LIMITE_MAX 40
#define DIS_MEDIA 17
//Velocidades iniciales de los motores
#define SPEEDB 49
#define SPEEDC 50
//Maxima correccion que se aplicara a los motores
#define MAX_CORRECCION 3
//Datos para selecionar una tarea u otra
#define DIR_DERECHA 0
#define DIR_IZQUIERDA 1
#define DIR_REC_DER 2
#define DIR_REC_IZ 3
//Puertos usados
#define SONAR_PORT NXT_PORT_S4
#define PULSADOR1_PORT NXT_PORT_S12*

//Obtiene la distancia a la pared derecha haciendo la media entre 2 mediciones
int getDistancia();
//Obtiene los valores de revoluciones medias entre los dos motores
int getMediumRevs();
//Corrige hacia la derecha
void correccionDerecha();
//Corrige hacia la izquierda
void correccionIzquierda();
//Establece la velocidad de los motores
void setVelocidad(int motorB, int motorC);
//Reinicia los contadores de los motores
void resetMotorCounters();
//Calcula los grados de corrección
int calcularGradosCorreccion(int difDistancia, int difRevs);
//Reinicia los valores de revoluciones y distancia antiguos
void resetOldValues();