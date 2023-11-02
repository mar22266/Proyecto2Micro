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
#include <thread>
#include <random>
#include <map>
#include <unistd.h>

using namespace std;
using namespace std::chrono;
#define NTHREADS 8

const std::string RED = "\033[31m";   
const std::string GREEN = "\033[32m";   
const std::string YELLOW = "\033[33m";  
const std::string BLUE = "\033[34m";     
const std::string RESET = "\033[0m";    

void *simularCarrera(void *dataCarrera);
double randomEntre0Y1();
void simularEmergencia();

struct raceParams {
    int gasolina;
    int llantas;
    int vueltas;
    float probabilidadLluvia;
    int circuitoTiempo;
    int carName;
};

struct result {
    int carName;
    double time;
};

pthread_cond_t accidente;
pthread_mutex_t mutex;
bool accidenteActivo = false;

int main(){
    //Definir tiempos de los circuitos
    int gasolina,llantas,vueltas,circuitoTiempo;
    float probabilidadLluvia;
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
    int circuitoSeleccionado;

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

    circuitoTiempo = circuito[circuitoSeleccionado];

    //Crear hilos
    pthread_t tid[NTHREADS];
    pthread_attr_t attr;
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //Crear estructura


    pthread_mutex_init(&mutex, NULL); 
    pthread_cond_init(&accidente, NULL); 

    for (i=0; i<NTHREADS; i++)
    {
        raceParams* params = new raceParams;
        params->gasolina = gasolina;
        params->llantas = llantas;
        params->vueltas = vueltas;
        params->probabilidadLluvia = probabilidadLluvia;
        params->circuitoTiempo = circuitoTiempo;
        params->carName = i;
        rc = pthread_create(&tid[i], &attr, simularCarrera, (void *)params);
        if(rc)
        {
            printf("ERROR: return code from pthread_create() is %d \n",rc);
            exit(-1);
        }
    }

    std::vector<result*> resultados(NTHREADS);

    void* carResult;
    
    for(i=0; i<NTHREADS;i++)
    {
        rc = pthread_join(tid[i], &carResult);
        if (rc)
        {
            printf("ERROR: return code from pthread_joid() is %d \n", rc);
            exit(-1);
        }
        resultados[i] = (result*) carResult;
    }

    //Imprimir resultados
    cout << RESET << "Resultados: " << endl;
    for (int i = 0; i < NTHREADS; i++)
    {
        int carName = resultados[i]->carName;
        double tiempo = resultados[i]->time;
        if (tiempo == -1.0) {
            cout << "Auto " << carName << " se accidentó y quedó descalificado" << endl;
            continue;
        }
        else {
            cout << "Auto " << carName << " terminó en " << tiempo << " segundos" << endl;
        }
    }

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);

    return 0;
}

double randomEntre0Y1() {
    return (double)rand() / (double)RAND_MAX;
}

void simularEmergencia(){
    if (accidenteActivo) {
        this_thread::sleep_for(chrono::seconds(10));
        accidenteActivo = false;
        pthread_cond_broadcast(&accidente);
    }
}

//Estructura de parametros para la carrera
void *simularCarrera(void* raceParameters){

    raceParams* parameters = (raceParams*) raceParameters;
    int gasolina = parameters->gasolina;
    int llantas = parameters->llantas;
    int vueltas = parameters->vueltas;
    float probabilidadLluvia = parameters->probabilidadLluvia;
    int circuitoTiempo = parameters->circuitoTiempo;
    int carName = parameters->carName;


    auto startTimeG = std::chrono::high_resolution_clock::now();
    auto endTime = std::chrono::high_resolution_clock::now();
    duration<double> elapsedSeconds;
    
    for (int j = 0; j < vueltas; j++)
    {

        if (accidenteActivo) {
            pthread_mutex_lock(&mutex);
            cout << RED << "El auto " << carName << " se ha detenido por la emergencia" << endl; 
            pthread_cond_wait(&accidente, &mutex);
            cout << RESET << "El auto " << carName << " ha salido nuevamente ante la finalizacion de la emergencia" << endl;
            pthread_mutex_unlock(&mutex);
        }

        auto startTime = std::chrono::high_resolution_clock::now();
        if (j % gasolina*llantas){
            cout << RESET << "El auto " << carName << " se detuvo a cambiar llantas y a cargar gasolina en la vuelta: " << j << endl;
            this_thread::sleep_for(chrono::seconds(2));
        }
        else if (j % gasolina){
            cout << RESET << "El auto " << carName << " se detuvo a cargar gasolina en la vuelta: " << j << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
        else if (j % llantas){
            cout << RESET << "El auto " << carName << " se detuvo a cambiar llantas en la vuelta: " << j << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
        else{
            cout << RESET << "El auto " << carName << " está corriendo en la vuelta: " << j << endl;
        }

        float isRain = randomEntre0Y1();
        float isAccident = randomEntre0Y1();
        if (probabilidadLluvia > isRain){
            if (isAccident < 0.05)
            {
                cout << RED << "El auto " << carName << " se accidentó en la vuelta y queda descalificado en la vuelta: " << j << endl;

                cout << RED << "El auto " << carName << " se ha accidentado" << endl;
                cout << YELLOW << "Accidente activo: El auto de emergencia ha salido" << endl;

                accidenteActivo = true;
                simularEmergencia();

                result* carResult = new result;
                carResult->carName = carName;
                carResult->time = -1.0;
                pthread_exit((void *)carResult);
            }
            else
            {
                cout << YELLOW << "Por la lluvia el auto " << carName << " se retrasa 1 segundo en la vuelta en la vuelta: " << j << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
        else {
            if (isAccident < 0.01)
            {
                cout << RED << "El auto " << carName << " se accidentó en la vuelta y queda descalificado en la vuelta: " << j << endl;

                cout << RED << "El auto " << carName << " se ha accidentado" << endl;
                cout << YELLOW << "Accidente activo: El auto de emergencia ha salido" << endl;

                accidenteActivo = true;
                simularEmergencia();

                result* carResult = new result;
                carResult->carName = carName;
                carResult->time = -1.0;
                pthread_exit((void *)carResult);
            }
            else 
            {
                cout << RESET << "El auto " << carName << " está corriendo en la vuelta: " << j << endl;
            }
        }

        this_thread::sleep_for(chrono::seconds(circuitoTiempo));
        endTime = std::chrono::high_resolution_clock::now();
        elapsedSeconds = endTime - startTime;
        cout << BLUE << "El auto " << carName << " ha terminado la vuelta: " << j << " en " << elapsedSeconds.count() << " segundos" << endl;
        startTime = endTime;

    }
    elapsedSeconds = endTime - startTimeG;
    cout << GREEN << "El auto " << carName << " ha terminado la carrera en: " << elapsedSeconds.count() << " segundos" << endl;


    result* carResult = new result;
    carResult->carName = carName;
    carResult->time = elapsedSeconds.count();

    pthread_exit((void *)carResult);
};

//Compile with g++ -std=c++11 -pthread Carreras.cpp -o Carreras