#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h> //Para el mod con flotantes

#include <unistd.h> // Para la función sleep()
#include "../Headers/BenjasSript.h"

#include <unitree/robot/channel/channel_publisher.hpp>
#include <unitree/robot/channel/channel_subscriber.hpp>
#include <unitree/robot/channel/channel_factory.hpp>
#include <unitree/idl/go2/BmsState_.hpp> // De aqui leemos el porcentaje de bateria
#include <unitree/idl/go2/SportModeState_.hpp> // De aqui obtenemos las distancias alrededor del robot
#include <unitree/robot/go2/sport/sport_client.hpp> // El cliente para mover al perro

#define TOPIC_HIGHSTATE "rt/sportmodestate"
#define TOPIC_BMSSTATE "rt/bmsstate"

#define VELOCIDAD_SEGURA 0.5f

// Como manejamos double para los valores, usaremos un valor aceptable
#define eps 0.000001 //Lo aplicaremos en EQ y NEQ
// --- MEMORIA DE LA MÁQUINA VIRTUAL ---
#define STACK_SIZE 100
double pila[STACK_SIZE];
int sp = -1; // Stack Pointer (Apunta al tope de la pila)


//Clases para el movimiento del robot =========================
using namespace unitree::common;

enum test_mode
{
  /*---Basic motion---*/
  normal_stand,
  balance_stand,
  velocity_move,
  stand_down,
  stand_up,
  damp,
  recovery_stand,
  /*---Special motion ---*/
  sit,
  rise_sit,
  stop_move = 99
};

const int Movement = normal_stand;

class Floppy
{
    private:        
        double ct;  //px0, py0, yaw0
        std::array<float, 3UL> pos;
        bool flag;
        float dt = 0.005;
        int option = normal_stand;

        unitree_go::msg::dds_::SportModeState_ state;
        unitree::robot::go2::SportClient sport_client;
        unitree::robot::ChannelSubscriberPtr<
            unitree_go::msg::dds_::SportModeState_> suber;
        unitree::robot::ChannelSubscriberPtr<
        unitree_go::msg::dds_::BmsState_> suber_bms; //

        //Globales del robot=========================================
        unitree_go::msg::dds_::SportModeState_ global_sport_state;
        unitree_go::msg::dds_::BmsState_ global_bms_state;

        void HighStateHandler(const void *message)
        {
            global_sport_state = *(unitree_go::msg::dds_::SportModeState_ *)message;
        }

        void BmsStateHandler(const void *message)
        {
            global_bms_state = *(unitree_go::msg::dds_::BmsState_ *)message;
        }

        void GetInitState()
        {
            pos = state.position();

            std::cout << "x0: " << pos[0] << std::endl
            << "y0: " << pos[1] << std::endl
            << "yaw0: " << pos[2] << std::endl;
        }

    public:
        Floppy()
        {
            sport_client.SetTimeout(10.0f);
            sport_client.Init();

            suber.reset(new unitree::robot::ChannelSubscriber<unitree_go::msg::dds_::SportModeState_>(TOPIC_HIGHSTATE));
            suber->InitChannel(std::bind(&Floppy::HighStateHandler, this, std::placeholders::_1), 1);

            suber_bms.reset(new unitree::robot::ChannelSubscriber<unitree_go::msg::dds_::BmsState_>(TOPIC_BMSSTATE));
            suber_bms->InitChannel(std::bind(&Floppy::BmsStateHandler, this, std::placeholders::_1), 1);

            sleep(2);

            GetInitState();
        }

        double getInFrontDistance()
        {
            return (double)global_sport_state.range_obstacle()[0];
        }
        
        int get_BatteryPercentage()
        {
            return (int)global_bms_state.soc();
        }

        void set_OptionControl(int option)
        {
            this->option = option;
        }
        void BasicRobotControl()
        {
            ct += dt;
            double px_local, py_local, yaw_local;
            double vx_local, vy_local, vyaw_local;
            double px_err, py_err, yaw_err;
            double time_seg, time_temp;

            unitree::robot::go2::PathPoint path_point_tmp;
            std::vector<unitree::robot::go2::PathPoint> path;

            switch(this->option)
            {
                case normal_stand:
                    sport_client.StandUp();
                break;

                case balance_stand:
                    sport_client.BalanceStand();
                break;

                case stand_down:
                    sport_client.StandDown();
                break;

                case stand_up:
                    sport_client.StandUp();
                break;

                case damp:
                    sport_client.Damp();
                break;

                case recovery_stand:
                    sport_client.RecoveryStand();
                break;

                case sit:
                    if (flag == 0)
                    {
                        sport_client.Sit();
                        flag = 1;
                    }
                break;

                case rise_sit:
                    if (flag == 0)
                    {
                        sport_client.RiseSit();
                        flag = 1;
                    }
                break;
                
                default:
                    sport_client.StandDown();
                break;
            }
        }  
        
        float get_dt()
        {
            return this->dt;
        }
};



//===============================POP y PUSH =========================
// Funciones auxiliares de la pila
void push(double valor) 
{
    if (sp < STACK_SIZE - 1) 
    {
        sp++;
        pila[sp] = valor;
    }
    else 
    {
        printf("Error: Stack Overflow in VM\n");
    }
}

double pop()
{
    if (sp >= 0)
    {
        double valor = pila[sp];
        sp--;
        return valor;
    }
    else
    {
        printf("Error: Stack Underflow in VM\n");
        return 0.0;
    }
}

// --- EL CEREBRO EJECUTOR ---
void ejecutar_VM(const char* networkInt)
{
    // Inicializa la red. Cambia "eno2" por tu interfaz real si conectas al robot
    // Para pruebas usa "lo" (loopback local)
    unitree::robot::ChannelFactory::Instance()->Init(0, networkInt);
    Floppy floppy = Floppy();
   
    //Repetir la accion hasta que se complete
    ThreadPtr thread = CreateRecurrentThread(floppy.get_dt() * 1000000, std::bind(&Floppy::BasicRobotControl, &floppy));

    

    // Abrimos el archivo en MODO BINARIO (Ignoramos el .txt por completo)
    FILE *binFile = fopen("../../Object.bin", "rb");
    if (binFile == NULL) {
        printf("Cant open the Bytecode.\n");
        return;
    }

    int opCode;
    int ejecutando = 1;

    printf("\n=== STARTING VM BENJAS_SCRIPT ===\n");

    // fread lee el siguiente entero. Si logra leer 1, entra al ciclo.
    while (ejecutando && fread(&opCode, sizeof(int), 1, binFile))
    {
        switch (opCode)
        {
            case OP_HALT: // 0
                printf("[MV] Process end.\n");
                ejecutando = 0;
                //sport_client.StopMove();
                floppy.set_OptionControl(stand_down);
                floppy.BasicRobotControl();
            
                break;

            case OP_PUSH: // OP_PUSH (Ajusta los números a tu enum)
            {
                double constante;
                // Como es PUSH, sabemos que los siguientes 8 bytes son un double
                fread(&constante, sizeof(double), 1, binFile);
                push(constante);
                break;
            }

            case OP_LOAD: // OP_LOAD
            {
                int index;
                // Leemos el indice de la variable
                fread(&index, sizeof(int), 1, binFile);
                // Sacamos su valor de la tabla de simbolos global y lo metemos a la pila
                push(symbolArray[index].doubleValue);
                break;
            }

            case OP_STORE: // OP_STORE
            {
                int index;
                fread(&index, sizeof(int), 1, binFile);
                // Sacamos el valor de la pila y lo guardamos en la memoria
                symbolArray[index].doubleValue = pop();
                break;
            }

            case OP_SUB: // OP_SUB (Resta)
            {
                // OJO AL ORDEN: El primero que sale es el lado DERECHO
                double der = pop(); 
                double izq = pop();
                push(izq - der);
                break;
            }

            case OP_ADD:
            {
                double der = pop(); 
                double izq = pop();
                push(izq + der);
                break;
            }

            case OP_MUL:
            {
                double der = pop(); 
                double izq = pop();
                push(izq * der);
                break;
            }

            case OP_DIV:
            {
                double der = pop(); 
                double izq = pop();
                push(izq / der);
                break;
            }

            case OP_MOD:
            {
                double der = pop(); 
                double izq = pop();
                
                push(fmod(izq,der));
                break;
            }

            case OP_GT: // OP_GT (Mayor que)
            {
                double der = pop();
                double izq = pop();
                if (izq > der) push(1.0); // Verdadero
                else push(0.0);           // Falso
                break;
            }

            case OP_LT: // OP_GT (Mayor que)
            {
                double der = pop();
                double izq = pop();
                if (izq < der) push(1.0); // Verdadero
                else push(0.0);           // Falso
                break;
            }

            case OP_EQ:
            {
                double der = pop();
                double izq = pop();
                if (fabs(izq - der) < eps) push(1.0); // Verdadero
                else push(0.0);           // Falso
                break;
            }

            case OP_NEQ:
            {
                double der = pop();
                double izq = pop();
                if (fabs(izq - der) > eps) push(1.0); // Verdadero
                else push(0.0);           // Falso
                break;
            }

            case OP_NOT_JUMP: // OP_JMP_FALSE (Salto Condicional)
            {
                int destino;
                fread(&destino, sizeof(int), 1, binFile); // Leemos el byte de destino
                
                double condicion = pop();
                if (condicion == 0.0) // Si la condicion fue falsa
                {
                    // ¡VIAJAMOS POR EL ARCHIVO!
                    fseek(binFile, destino, SEEK_SET); 
                }
                break;
            }

            case OP_JUMP: // OP_JMP (Salto Incondicional de regreso)
            {
                int destino;
                fread(&destino, sizeof(int), 1, binFile);
                fseek(binFile, destino, SEEK_SET); // Regresamos directo al inicio del While
                break;
            }

            case OP_WALK: // OP_WALK (Ajusta al numero de tu enum)
            {
                double distanciaX = pop();
        
                //float rotacionYawFloat = float(rotacionYaw);
                
                printf("[Robot Go2] Walking %lfm...\n", distanciaX);
                
                //floppy.BasicRobotControl(walk_forward);

                float time = fabs((float)distanciaX) / VELOCIDAD_SEGURA;

                //Microsegundos
                usleep((int)(time * 1000000.0f));
                //floppy.BasicRobotControl(stand_down);
                usleep(500000);
                break;
            }

            case OP_GETDIST: // El famoso 15
            {
                // 'range_obstacle' es un arreglo de distancias alrededor del robot
                // El índice [0] corresponde al sector directamente AL FRENTE (cabeza)
                double obtenerDist = floppy.getInFrontDistance();

                printf("[Robot Go2] Reading distance... Next wall in %lfm...\n", obtenerDist);
                // ¡MUY IMPORTANTE! GetDist devuelve un valor, así que debe hacer un push a la pila
                push(obtenerDist);
                break;
            }

            case OP_GETBATTERY: // 16
            {
                //Obtener la información de la batería desde el estado global
                int porcentajeBateria = floppy.get_BatteryPercentage();
                //int porcentajeBateria = global_bms_state.soc();

                printf("[Robot Go2] Baterry: %d...\n", porcentajeBateria);
                push((double)porcentajeBateria);
                break;
            }

            case OP_ROTATE: // 18
            {
                double radianes = pop();
                
                printf("[Robot Go2] Rotate: %lf...\n", radianes);

                float dirGiro = radianes > 0 ? VELOCIDAD_SEGURA : -VELOCIDAD_SEGURA;

                //sport_client.Move(0.0f, 0.0f, dirGiro);

                float time = fabs((float)radianes) / VELOCIDAD_SEGURA;

                //Microsegundos
                usleep((int)time * 1000000);
                //sport_client.StopMove();
                usleep(500000);
                break;
            }

            case OP_CLIMB: // 19 //todavia esta experimental
            {
                double valor = pop(); 
                printf("[Robot Go2] Climbing %lf...\n", valor);
                break;
            }

            case OP_SIT: // 20
            {
                printf("[Robot Go2] Sit...\n");

                //sport_client.Sit();
                floppy.set_OptionControl(sit);
                floppy.BasicRobotControl();
                sleep(4);
                break;
            }

            case OP_STAND: // 21
            {
                printf("[Robot Go2] Stand Up...\n");

                //sport_client.StandUp();
                floppy.set_OptionControl(stand_up);
                floppy.BasicRobotControl();
                sleep(4);
                break;
            }
            case OP_AND: // 24
            {
                // El primero que sale es el operando derecho
                double derecho = pop(); 
                double izquierdo = pop();
                
                // En C, si ambos son diferentes de 0.0, el AND es verdadero
                if (izquierdo != 0.0 && derecho != 0.0)
                {
                    push(1.0); // Verdadero
                }
                else
                {
                    push(0.0); // Falso
                }
                break;
            }

            case OP_OR: // 25
            {
                double derecho = pop();
                double izquierdo = pop();
                
                // Si al menos uno es diferente de 0.0, el OR es verdadero
                if (izquierdo != 0.0 || derecho != 0.0)
                {
                    push(1.0); // Verdadero
                }
                else
                {
                    push(0.0); // Falso
                }
                break;
            }

            

            default:
                printf("[MV] OpCode not recognized: %d\n", opCode);
                ejecutando = 0;
                break;
        }
    }

    fclose(binFile);
    printf("=== MAQUINA VIRTUAL END ===\n");
}