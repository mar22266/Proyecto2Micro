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
#include <thread>
#include <random>
#include <map>
#include <omp.h>

using namespace std;
using namespace std::chrono;
#define NTHREADS 8

void simularCarrera(int gasolina, int llantas, int vueltas, int circuitoTiempo, float probabilidadLluvia, int circuitoSeleccionado);
double randomEntre0Y1();

double resultados[NTHREADS];

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

    //Definir threads
    simularCarrera(gasolina, llantas, vueltas, circuitoTiempo, probabilidadLluvia, circuitoSeleccionado);


    //Imprimir resultados
    cout << "Resultados: " << endl;
    for (int i = 0; i < NTHREADS; i++)
    {
        if (resultados[i] == -1) {
            cout << "Auto " << i << " se accidentó y quedó descalificado" << endl;
            continue;
        }
        else {
            cout << "Auto " << i << " terminó en " << resultados[i] << " segundos" << endl;
        }
    }

    return 0;
}

double randomEntre0Y1() {
    return (double)rand() / (double)RAND_MAX;
}

//Estructura de parametros para la carrera
void simularCarrera(int gasolina, int llantas, int vueltas, int circuitoTiempo, float probabilidadLluvia, int circuitoSeleccionado){


    #pragma omp parallel num_threads(NTHREADS)
    {
        int carName = omp_get_thread_num();
        auto startTimeG = std::chrono::high_resolution_clock::now();
        auto endTime = std::chrono::high_resolution_clock::now();
        duration<double> elapsedSeconds;
        
        for (int j = 0; j < vueltas; j++)
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            if (j % gasolina*llantas){
                cout << "El auto " << carName << " se detuvo a cambiar llantas y a cargar gasolina en la vuelta: " << j << endl;
                this_thread::sleep_for(chrono::seconds(2));
            }
            else if (j % gasolina){
                cout << "El auto " << carName << " se detuvo a cargar gasolina en la vuelta: " << j << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }
            else if (j % llantas){
                cout << "El auto " << carName << " se detuvo a cambiar llantas en la vuelta: " << j << endl;
                this_thread::sleep_for(chrono::seconds(1));
            }
            else{
                cout << "El auto " << carName << " está corriendo en la vuelta: " << j << endl;
            }

            float isRain = randomEntre0Y1();
            float isAccident = randomEntre0Y1();
            if (probabilidadLluvia > isRain){
                if (isAccident < 0.05)
                {
                    cout << "El auto " << carName << " se accidentó en la vuelta y queda descalificado en la vuelta: " << j << endl;
                    resultados[carName] = -1;
                }
                else
                {
                    cout << "Por la lluvia el auto " << carName << " se retrasa 1 segundo en la vuelta en la vuelta: " << j << endl;
                    this_thread::sleep_for(chrono::seconds(1));
                }
            }
            else {
                if (isAccident < 0.01)
                {
                    cout << "El auto " << carName << " se accidentó en la vuelta y queda descalificado en la vuelta: " << j << endl;
                    resultados[carName] = -1;
                }
                else 
                {
                    cout << "El auto " << carName << " está corriendo en la vuelta: " << j << endl;
                }
            }

            this_thread::sleep_for(chrono::seconds(circuitoTiempo));
            endTime = std::chrono::high_resolution_clock::now();
            elapsedSeconds = endTime - startTime;
            cout << "El auto " << carName << " ha terminado la vuelta: " << j << " en " << elapsedSeconds.count() << " segundos" << endl;
            startTime = endTime;

        }
        elapsedSeconds = endTime - startTimeG;
        cout << "El auto " << carName << " ha terminado la carrera en: " << elapsedSeconds.count() << " segundos" << endl;
        if (resultados[carName] != -1) {
            resultados[carName] = elapsedSeconds.count();
        }
    }
};


//Compile with: g++ -std=c++11 -fopenmp Proyecto2.cpp -o Proyecto2