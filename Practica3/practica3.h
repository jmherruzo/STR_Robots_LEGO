//Limite superior de velocidad
#define UPPER_LIMIT 100
//Velocidades iniciales de los motores
#define MAXSPEED 100
#define MINSPEED 25
//Grados de luz
#define MAXVARIACION 35
//Puertos usados
#define SONAR_PORT NXT_PORT_S4
#define PULSADOR1_PORT NXT_PORT_S2
#define LIGHT_PORT NXT_PORT_S1

//Rueda
#define RUEDA 25.44

//Direciones
#define ADELANTE 1
#define ATRAS 2

//Obtiene la distancia a la pared derecha haciendo la media entre 2 mediciones
int getDistancia();
//Obtiene los valores de revoluciones medias entre los dos motores
int getMediumRevs();
//Corrige hacia la derecha
void setVelocidad(int motorB, int motorC);
//Reinicia los contadores de los motores
void resetMotorCounters();
//Mueve el robot una distancia dada
void moverDelante(float distancia);
//Calcula la distancia que es necesario mover
float calcularDistancia(int light);
