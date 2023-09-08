/***
 * Andre Marroquin
 * Joaquin puente
 * Nelson Gracia
 * Proyecto 2 Microprocesadores
 * Simulacion carreras
*/

//Importar librerias
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <random>
#include <map>

using namespace std;
#define NTHREADS 8

//Estructura de parametros para la carrera
void *simularCarrera(void *threadNum){
    long tID;
    tID = (long) threadNum;

    cout << "Hello thread : " << tID << endl;

    pthread_exit(NULL);
};

struct dataCarrera{
    int circuito;
    int gasolina;
    int llantas;
    int vueltas;
    float probabilidadLluvia;
};

double randomEntre0Y1();

int main(){
    //Definir tiempos de los circuitos
    map <int, int> circuito {
        {1, 3},
        {2, 2},
        {3, 1},
        {4, 2},
        {5, 1},
        {6, 3},
        {7, 2},
        {8, 4}
    };

    //Instaciar variables
    int i, rc;
    int gasolina,llantas,vueltas;
    int circuitoSeleccionado;
    float probabilidadLluvia;


    //Opciones de pistas para el usuario
    cout << "Ingrese el número de circuito que desea: " << endl;
    cout << "1. Rainbow road" << endl;
    cout << "2. Ghost Valley" << endl;
    cout << "3. Mushrrom bridge" << endl;
    cout << "4. Dessert hills" << endl;
    cout << "5. Frappe Snowland" << endl;
    cout << "6. Electrodome" << endl;
    cout << "7. Luigi's Mansion" << endl;
    cout << "8. Bowser's Castle" << endl;
    cin >> circuitoSeleccionado;

    //Duracion Gasolina
    cout << "Ingrese el número de vueltas que durará el tanque de gasolina: " << endl;
    cin >> gasolina;

    //Duración llantas
    cout << "Ingrese el número de vueltas que durarán las llantas: " << endl;
    cin >> llantas;

    //Duración Carrera
    cout << "Ingrese de cuantas vueltas será la carrera: " << endl;
    cin >> vueltas;

    //Definir Probabilidad de lluvia
    probabilidadLluvia = randomEntre0Y1();

    //Intanciar objeto de parametros a pasar en el hilo
    dataCarrera data;
    data.circuito = circuito[circuitoSeleccionado];
    data.gasolina = gasolina;
    data.llantas = llantas;
    data.vueltas = vueltas;
    data.probabilidadLluvia = probabilidadLluvia;


    //Crear hilos
    pthread_t tid[vueltas];
    pthread_attr_t attr;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    for (i=0; i<NTHREADS; i++)
    {
        rc = pthread_create(&tid[i], &attr, simularCarrera, (void *)i);
        if(rc)
        {
            printf("ERROR: return code from pthread_create() is %d \n",rc);
            exit(-1);
        }
    }

    //Esperar a que los hilos terminen
    for(i=0; i<NTHREADS;i++)
    {
        rc = pthread_join(tid[i], NULL);

        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d \n", rc);
            exit(-1);
        }
        
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);

    return 0;
}

double randomEntre0Y1() {
    return (double)rand() / (double)RAND_MAX;
}


//Compile with g++ -std=c++11 -pthread Carreras.cpp -o Carreras