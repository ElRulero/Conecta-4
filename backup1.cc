#include "jugadormaquina.h"
#include <iostream>
#include "tablero.h"
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int valor1 = 1;
int valor2 = 10;
int valor3 = 100;
int valor4 = INT_MAX-1; // inf

int valorOp1 = 10;
int valorOp2 = 100;
int valorOp3 = 1000;
int valorOp4 = INT_MAX;

//#include <QTime>
/*
auto start = high_resolution_clock::now()

       QTime dieTime= QTime::cureentTime().addSecs(1);
while(QTime::currentTime() < dieTime){

}
auto stop = high_resolution_clock::now()

       qDebug() << "Duracion: " << duration_cast<milliseconds>(stop - start).count() <<
}*/

// //                        A-D    D     B-D      B     B-I     I      A-I    A
//vector<Direccion> Dirs = {{1,1},{0,1},{-1,+1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0}};
vector<Direccion> Dirs = {{0,1},{1,0},{1,1},{1,-1},{0,-1},{-1,0},{-1,-1},{-1,+1}};

/**
 * @brief JugadorMaquina::JugadorMaquina Función que se ejecuta en el thread
 */
JugadorMaquina::JugadorMaquina(Jugador *parent): Jugador(parent)
{
    tiempoExcedido = false;
}

void JugadorMaquina::comenzarBusqueda(const int jug)
{
    jugador = jug;
    //Llama a la función Minimax que implementa el algoritmo para calcular la jugada

    cout << "JUGADOR: " << jug << endl;
    minimax(jugador,0);
    columna = iterador;
    //No borrar esta línea!!
    emit trabajoFinalizado(columna, jugador);
}
void JugadorMaquina::setTiempoExcedido(bool excedido) {
    tiempoExcedido = excedido;
}

/**
 * @brief JugadorMaquina::minimax Se debe determinar la mejor jugada mediante el algoritmo Minimax.
 * El tablero de juego se encuentra en la variable tablero.
 * Al final de la función de la variable columna debe contener la tirada.
 */

void JugadorMaquina::imprimirTablero(){
    for(int i = FILAS_TABLERO-1; i >= 0; i--){
        for(int j = 0; j < COLS_TABLERO; j++){
            if(tablero->obtenerCasilla(i,j) == 0){
                cout << "-" << "\t";
            }
            else if(tablero->obtenerCasilla(i,j) == 1){
                cout << "X" << "\t";
            }
            else{
                cout << "O" << "\t";
            }
        }
        cout << endl;
    }
    cout << "--------------------------" << endl;
}

//Devuelve true si la direccion que se le pasa esta dentro de los limites del tablero
bool JugadorMaquina::dentroBordes(int i, int j){
    if(i < FILAS_TABLERO && j < COLS_TABLERO && i >= 0 && j >= 0){
        return true;
    }
    return false;
}

//Devuelve true si una ficha no tiene ninguna otra de su tipo alrededor
bool JugadorMaquina::fichaSola(int i, int j, int jug){
    bool sola = true;
    //Devuelve false si no esta sola y true si lo está
    for(int k = 0; k < 8; k++){
        if(dentroBordes(i+Dirs[k].x,j+Dirs[k].y) && (tablero->obtenerCasilla(i+Dirs[k].x,j+Dirs[k].y) == jug)){
            sola = false;
        }
    }
    return sola;
}

//Devuelve el numero de fichas que hay en una direccion
int JugadorMaquina::casillasEnDireccion(Direccion dir, bool matrizDirecciones[][COLS_TABLERO][8], int k, int jug){
    int num = 0;
    int mult = 1;

    while(dentroBordes(dir.x+mult*Dirs[k].x,dir.y+mult*Dirs[k].y) && matrizDirecciones[dir.x][dir.y][k] != false && tablero->obtenerCasilla(dir.x+mult*Dirs[k].x,dir.y+mult*Dirs[k].y) == jug){
        matrizDirecciones[dir.x+mult*Dirs[k].x][dir.y+mult*Dirs[k].y][k] = false;
        if(k < 4){
            matrizDirecciones[dir.x+mult*Dirs[k].x][dir.y+mult*Dirs[k].y][k+4] = false;
        }
        else{
            matrizDirecciones[dir.x+mult*Dirs[k].x][dir.y+mult*Dirs[k].y][k-4] = false;
        }
        mult++;
        num++;
    }
    return num;
}

//Funcion que multiplica la cantidad de cada tipo de grupos por su valor correspondiente
int JugadorMaquina::funcionHeuristica(int G1, int G2, int G3, int G4, int casilla){
    if(casilla == jugador){
        return valor1*G1 + valor2*G2 + valor3*G3 + valor4*G4;
    }
    else{
        return valorOp1*G1 + valorOp2*G2 + valorOp3*G3 + valorOp4*G4;
    }
}

int JugadorMaquina::posibilidadesEnDireccion(Direccion dir, int k, int mult){
    int num = 0;
    int multi = mult+1;
    while(dentroBordes(dir.x+multi*Dirs[k].x,dir.y+multi*Dirs[k].y) && tablero->obtenerCasilla(dir.x+multi*Dirs[k].x,dir.y+multi*Dirs[k].y) == 0){
        num++;
        multi++;
    }
    return num;
}


int JugadorMaquina::contarPuntos(int jug){

    Direccion dir;

    int G1,G2,G3,G4;
    G1 = G2 = G3 = G4 = 0;

    //Matriz de vectores de 8
    bool matrizDirecciones[FILAS_TABLERO][COLS_TABLERO][8];
    for(int i = 0; i < 6; i++){
        for(int j = 0; j < 7; j++){
            for(int k = 0; k < 8; k++){
                matrizDirecciones[i][j][k] = true;
            }
        }
    }

    //Bucle que recorre todo el tablero
    for(int i = 0; i < FILAS_TABLERO; i++){
        for(int j = 0; j < COLS_TABLERO;j++){
            dir = {i,j};
            if(tablero->obtenerCasilla(dir.x,dir.y) == jug){
                if(fichaSola(dir.x,dir.y,jug)){
                G1++;
                //cout << "Ficha en direccion: " << dir.x << ", " << dir.y << "    ficha sola " << endl;
                }
                else{
                    //cout << "Ficha en direccion: " << dir.x << ", " << dir.y << endl;
                    for(int k = 0; k < 4; k++){
                        int dir1 = casillasEnDireccion(dir, matrizDirecciones, k,jug);
                        int dir2 = casillasEnDireccion(dir, matrizDirecciones, k+4,jug);
                        int pos1 = posibilidadesEnDireccion(dir,k,dir1);
                        int pos2 = posibilidadesEnDireccion(dir,k+4,dir2);
                        //cout << "dir " << k << ": " << dir1 << ", "<< dir2 << ", " << pos1 << ", " << pos2<< "    " << endl;
                        if(1 + dir1 + dir2 + pos1 + pos2 >= 4){
                            int fichas = 1 + dir1 + dir2;
                            if(fichas == 2){
                                G2++;
                            }
                            else if(fichas == 3){
                                G3++;
                            }
                            else if(fichas == 4){
                                G4++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    cout << G1 << " " << G2 << " " << G3 << " " << G4 << " " << endl;
    int puntos;
    if(jug == jugador){
        puntos = funcionHeuristica(G1,G2,G3,G4, jugador);
    }
    else{
        puntos = funcionHeuristica(G1,G2,G3,G4, jug);
    }
    cout << "Puntos: " << puntos << endl;
    return puntos;
}

bool JugadorMaquina::nodoHoja(int profundidad){
    //4 en raya, tablero lleno, profundidad alcanzada
    if(tablero->cuatroEnRaya() != 0 || tablero->tableroLleno() || profundidad == NIVEL_DEFECTO){
        return true;
    }
    return false;
}

int JugadorMaquina::minimax(int casilla, int profundidad){


    oponente = 1;
    if(jugador == 1){
        oponente = 2;
    }

    if(nodoHoja(profundidad)){
        //imprimirTablero();
        return contarPuntos(jugador) - contarPuntos(oponente);
    }
    else{
        if(profundidad%2 == 0){
            profundidad++;
            int maximo = INT_MIN;
            cout << "MAX" << endl;
            for(int i = 0; i < COLS_TABLERO; i++){
                int fila = tablero->comprobarColumna(i);
                if(fila != -1){
                    tablero->ponerFicha(i,casilla);
                    int max = minimax(oponente,profundidad);
                    if(max > maximo){
                        maximo = max;
                        if(profundidad == 1){
                            iterador = i;
                        }
                    }
                    tablero->cambiarCasilla(fila, i, 0);
                }
            }
            return maximo;
            cout << "Maximo: " << maximo << " " << iterador << endl;
        }
        else{
            profundidad++;
            int minimo = INT_MAX;
            cout << "MIN" << endl;
            for(int i = 0; i < COLS_TABLERO; i++){
                int fila = tablero->comprobarColumna(i);
                if(fila !=-1){
                    tablero->ponerFicha(i,casilla);
                    int min = minimax(jugador, profundidad);
                    if(min < minimo){
                        minimo = min;
                    }
                    tablero->cambiarCasilla(fila,i, 0);
                }
            }
            return minimo;
            cout << "Minimo: " << minimo << " " << iterador << endl;
        }
    }
    cout << "Final: " << iterador << endl;
}
