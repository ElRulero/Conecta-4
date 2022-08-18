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
int valor4 = 10000; // inf

bool primera;

// //                        A-D    D     B-D      B     B-I     I      A-I    A
//vector<Direccion> Dirs = {{1,1},{0,1},{-1,+1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0}};
vector<Direccion> Dirs = {{0,1},{1,0},{1,1},{1,-1},{0,-1},{-1,0},{-1,-1},{-1,+1}};

vector<Direccion> sola = {{0,-1},{1,0},{0,1},{1,-1},{1,1}};

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

    primera = true;
    NIVEL_DEFECTO = 6;
    minimax(jugador,0,INT_MIN,INT_MAX);
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

//Devuelve true si la direccion que se le pasa esta dentro de los limites del tablero
bool JugadorMaquina::dentroBordes(int i, int j){

    if(i < FILAS_TABLERO && j < COLS_TABLERO && i >= 0 && j >= 0){ // Menor que los valores maximos de las filas y columnas
                                                                   // y mayor o igual a 0
        return true;
    }

    return false;
}

//Devuelve true si una ficha no tiene ninguna otra de su tipo alrededor
bool JugadorMaquina::fichaSola(int i, int j, int jug){

    //Devuelve false si no esta sola y true si lo está
    for(int k = 0; k < 8; k++){

        if(dentroBordes(i+Dirs[k].x,j+Dirs[k].y) && (tablero->obtenerCasilla(i+Dirs[k].x, j+Dirs[k].y) == jug)){
            return false; // Si en alguna de las posiciones detecta una ficha de su mismo color devuelve false;

        }

    }

    // Si no se ha encontrado ninguna ficha del mismo color se devuelve true significando que esta sola
    return true;
}

// Devuelve el numero de fichas del mismo color que hay en una direccion
int JugadorMaquina::casillasEnDireccion(Direccion dir, int k, int jug){

    int multi = 1;
    int puntos = 0;

    // Calculo la casilla siguiente para comprobar su contenido
    int casilla_Siguiente = tablero->obtenerCasilla(dir.x + multi*Dirs[k].x, dir.y + multi*Dirs[k].y);

    // Si la fila es la de abajo del todo (0) no hace falta comprobar la dirección contraria
    // para ver si ya forma parte de otro grupo
    if(dir.x == 0){

        // Este bucle while comprueba hasta un máximo de 3 posiciones en la direccion que indica el parámetro k
        // y solo para si encuentra una ficha del oponente, se sale del tablero o ya ha comprobado las 3 casillas
        while(dentroBordes(dir.x + multi*Dirs[k].x,dir.y + multi*Dirs[k].y) && multi <=3){

            // Si la siguiente casilla es un espacio en blanco aumenta el multiplicador para pasar a la siguiente
            if(casilla_Siguiente == 0){
                multi++;
            }
            else if(casilla_Siguiente == jug){// Si la casilla es del mismo color se incrementa el número de fichas
                                              // que devuelve la función y el multiplicador para comprobar
                                              // la siguiente posición
                puntos++;
                multi++;
            }
            else{ // Si la ficha es del oponente, se sale del bucle
                break;
            }
            // Si no se ha activado el break, se calcula la siguiente posición a comprobar
            casilla_Siguiente = tablero->obtenerCasilla(dir.x + multi*Dirs[k].x, dir.y + multi*Dirs[k].y);

        }

    }
    else{

        // Si no es la fila 0 significa que la ficha puede formar parte de grupos anteriores y por tanto no se debe
        // comprobar la direccion

        // Se comprueba la casilla anterior, ya que si en esta posición hay una ficha del mismo color que la que
        // se está comprobando, quiere decir que la posición actual forma parte de otro grupo en esta dirección
        // y ya se ha tenido en cuenta, por tanto no se debe comprobar esta dirección


        // Si la posicion actual esta dentro de los límites del tablero se calcula
        if(dentroBordes(dir.x - Dirs[k].x,dir.y - Dirs[k].y)){

            //Calculamos posición anterior en la dirección opuesta a la que nos pasan
            int casilla_Anterior = tablero->obtenerCasilla(dir.x - Dirs[k].x, dir.y - Dirs[k].y);

            // Si la posición anterior esta vacía o hay una ficha del oponente, se comrpueba la dirección
            if(casilla_Anterior != jug){

                // Mientras que la siguiente casilla a comprobar este dentro de los límites del tablero y
                // no supere el rango de búsqueda 3, se comprueban las posiciones
                while(dentroBordes(dir.x + multi*Dirs[k].x,dir.y + multi*Dirs[k].y) && multi <=3){

                    if(casilla_Siguiente == 0){// Si la posición esta vacía se aumenta el multiplicador para seguir comprobando
                                   // la siguiente posición
                        multi++;
                    }
                    else if(casilla_Siguiente == jug){ // Si en la posición hay una ficha del mismo color se aumenta el contador y
                                           // se aumenta el multiplicador par segur comprobando direcciones
                        puntos++;
                        multi++;
                    }
                    else{ // Si encontramos una ficha de otro jugador se sale del bucle
                        break;
                    }
                    // Si no se ha activado el break se calcula la siguiente posición a comprobar
                    casilla_Siguiente = tablero->obtenerCasilla(dir.x + multi*Dirs[k].x, dir.y + multi*Dirs[k].y);

                }

            }

        }
        else{

            // Si la posición anterior no está dentro de los bordes del tablero, significa que la ficha no puede
            // ser parte de otro grupo y por tanto simplemente se comprueban las direcciones

            // Mientras que la posicion a comprobar este dentro de los límites del tablero y no se supere el rango de búsqueda:
            while(dentroBordes(dir.x + multi*Dirs[k].x,dir.y + multi*Dirs[k].y) && multi <=3){

                if(casilla_Siguiente == 0){// Si la posición está vacía se aumenta el multiplicador para comprobar la siguiente casilla
                    multi++;
                }
                else if(casilla_Siguiente == jug){// Si en la posición hay una casilla del mismo color se aumenta el contador de
                                      // fichas y el multiplicador para seguir comprobando la dirección
                    puntos++;
                    multi++;
                }
                else{ // Si se encuentra una ficha del oponente, se sale del bucle
                    break;
                }
                // Si no se ha activado el break, calculamos la siguiente posición a comprobar
                casilla_Siguiente = tablero->obtenerCasilla(dir.x + multi*Dirs[k].x, dir.y + multi*Dirs[k].y);

            }

        }

    }

    // Se devuelven el número de fichas encontradas
    return puntos;
}

// Devuelve el numero de fichas sobre el tablero, tanto de un jugador como del otro
int JugadorMaquina::contarFichas(){

    int fichas = 0;

    for(int i = 0; i < FILAS_TABLERO; i++){ // Se comprueban todas las filas

        for(int j = 0; j < COLS_TABLERO; j++){ // Se comprueban todas las columnas

            if(tablero->obtenerCasilla(i,j) != 0){// Si la posición no está vacía se aumenta el contador

                fichas++;

            }

        }

    }

    return fichas;
}

// Devuelve el número de espacios en blanco en la dirección indicada
int JugadorMaquina::posibilidadesEnDireccion(Direccion dir, int k, int mult){

    int multi = mult + 1;
    int puntos = 0;

    // Se calcula la siguiente posición a comprobar
    int siguiente_Casilla = tablero->obtenerCasilla(dir.x + multi*Dirs[k].x,dir.y + multi*Dirs[k].y);

    // Mientras que la posición esté dentro de los límites, la posición a comprobar esté vacía y
    // no se exceda el rango de búsqueda (3)
    while(dentroBordes(dir.x + multi*Dirs[k].x, dir.y + multi*Dirs[k].y) &&  siguiente_Casilla == 0 && multi <= 3){

        puntos++;
        multi++;

        // Se calcula la siguiente posición a comprobar
        siguiente_Casilla = tablero->obtenerCasilla(dir.x + multi*Dirs[k].x,dir.y + multi*Dirs[k].y);

    }

    // Devuleve el número de casillas libres
    return puntos;
}

// Devuelve el número de posibilidades que tiene una ficha solitaria para aumentar su valor dependiendo de
// los posibles grupos que pueda formar
int JugadorMaquina::posibilidadesFichaSola(Direccion dir, int k){

    int num = 0;
    int multi = 1;

    // Se calcula la siguiente posición a comprobar
    int siguiente_Casilla = tablero->obtenerCasilla(dir.x+multi*sola[k].x,dir.y+multi*sola[k].y);

    //Mientras que la posición a comprobar no se salga de los límites del tablero, esté vacía y no exceda el rango de búsqueda:
    while(dentroBordes(dir.x+multi*sola[k].x,dir.y+multi*sola[k].y) && siguiente_Casilla == 0 && num <= 3){

        num++; // Aumentamos contador de espacios vacíos
        multi++; // Aumentamos multiplicador para seguir comprobando posiciones

        // Se calcula la siguiente posición a comprobar
        siguiente_Casilla = tablero->obtenerCasilla(dir.x+multi*sola[k].x,dir.y+multi*sola[k].y);
    }

    // Se devuleven el número de posiciones vacías encontradas
    return num;
}

// Devuelve true si el tablero está completamente vacío y false si hay alguna ficha de cualquier jugador
bool JugadorMaquina::tableroVacio(){

    for(int i = 0; i < FILAS_TABLERO; i++){ // Se comprueban todas las filas

        for(int j = 0; j < COLS_TABLERO; j++){ // Se comprueban todas las columnas

            if(tablero->obtenerCasilla(i,j) != 0){ // Si una posición no está vacía devuelve false

                return false;

            }

        }

    }

    // Si todas las posiciones están vacías devuelve true
    return true;
}

// Devuelve el valor del tablero para el jugador pasado como parámetro
int JugadorMaquina::contarPuntos(int jug){

    Direccion dir;
    int puntos = 0;
    bool finish = false; // Variable que sale de todos los bucles si se encuentra un grupo de 4

    //Bucle que recorre todo el tablero
    for(int i = 0; i < FILAS_TABLERO && finish == false; i++){ // Se comprueban todas las filas a no ser que
                                                               // se encuentre un grupo de 4

        for(int j = 0; j < COLS_TABLERO && finish == false;j++){ // Se comprueban todas las columnas a no ser que
                                                                 // se encuentre un grupo de 4

            dir = {i,j}; // Se actualiza la dirección actual

            if(tablero->obtenerCasilla(dir.x,dir.y) == jug){ // Si en la posición actual hay una ficha del jugador
                                                             // pasado como parámetro

                if(fichaSola(dir.x,dir.y,jug)){ // Si la ficha esta sola y no forma parte de ningun grupo

                    puntos += valor1; // Se suma el valor de una ficha sola

                    for(int n = 0; n < 5; n++){ // Se comprueba cada una de las 5 posibles direcciones
                                                // IZQ || ARR-IZQ || ARR || ARR-DER || DER

                        if(n < 4){ // Si la direccion es IZQ || ARR || DER

                            puntos += posibilidadesFichaSola(dir,n); // Se suman tantos puntos como posiciones libres
                                                                     // se encuentren en la dirección

                            // Las diagonales solo se tienen en cuenta si hay una ficha junto a la posición que se esta
                            // comprobando y hacia el lado que se quiere comprobar la diagonal
                            // Por ejemplo si estamos en la posicion (0,3)(centro), la diagonal ARR-IZQ solo se comprobará
                            // si hay una ficha a la IZQ, es decir en (0,2). Lo mismo se aplica para la diagonal derecha.
                            // si la posición (0,2) estuviera vacía, la diagonal no se tendría en cuenta

                        }
                        else if(n == 3){ // Diagonal ARR-IZQ

                            if(tablero->obtenerCasilla(dir.x,dir.y-1) != 0){ // SI hay una ficha a la izquierda

                                puntos += posibilidadesFichaSola(dir,n); // Se suman tantos puntos como posiciones vacías
                                                                         // haya en esa dirección

                            }

                        }
                        else{ // Diagonal ARR-DER

                            if(tablero->obtenerCasilla(dir.x,dir.y+1) != 0){ // Si hay una ficha a la derecha

                                puntos += posibilidadesFichaSola(dir,n); // Se suman tantos puntos como posiciones vacías
                                                                         // haya en esa dirección

                            }

                        }

                    }

                }
                else{

                    for(int k = 0; k < 4; k++){ // for para comprobar las idrecciones ARR || DER || ARR-IZQ || ARR-DER

                        int dir1 = casillasEnDireccion(dir,k,jug); // Se cuentan las fichas del mismo color en esa direccion
                        int pos1 = posibilidadesEnDireccion(dir,k,dir1); // Se cuentan las casillas en blanco en esa direccion
                        int pos2 = posibilidadesEnDireccion(dir,k+4,0); // Se cuentan las casillas en blanco en la direccion
                                                                        // contraria, ya que son posibilidades para formar
                                                                        // un grupo de 4

                        if(1 + dir1 + pos1 + pos2 >= 4){ // Si la suma de la ficha actual mas
                                                         // las fichas encontradas y las posiciones en blanco
                                                         // son suficientes como para formar un grupo de 4:

                            int fichas = 1 + dir1; // Se calcula la longitud del grupo formado

                            if(fichas == 2){ // Si el grupo es de dos se suma el valor de un grupo de 2 mas las casillas
                                             // posibles para formar un grupo de 4

                                puntos += valor2 + pos1 + pos2;

                            }
                            else if(fichas == 3){ // Si el grupo es de 3 se suma el valor de un grupo de 3 mas las casillas
                                                  // posibles para formar un grupo de 4

                                puntos += valor3 + pos1 + pos2;

                            }
                            else if(fichas == 4){ // Si el grupo es de 4, se suma el valor de un grupo de 4 y se cambia la
                                                  // variable finish a true para que no se cumplan las condiciones de los bucles
                                                  // y se usa un break para salir del bucle

                                puntos += valor4;
                                finish = true;
                                break;

                            }

                        }

                    }

                }

            }

        }

    }

    //Devolvemos las valoración del tablero
    return puntos;

}

// Devuelve true si el tablero a comprobar cumple los requisitos para ser un nodo hoja, es decir, que contenga un
// cuatro en ralla, que esté lleno o que se haya alcanzado la profundidad máxima

bool JugadorMaquina::nodoHoja(int profundidad){

    //4 en raya, tablero lleno, profundidad alcanzada
    if(tablero->cuatroEnRaya() != 0 || tablero->tableroLleno() || profundidad == NIVEL_DEFECTO){

        return true;

    }

    return false;
}

int JugadorMaquina::minimax(int casilla, int profundidad, int alfa, int beta){

    oponente = 1;

    // Este if define cual es el jugador sobre el que se debe tirar la siguiente ficha en el minimax
    if(jugador == 1){

        oponente = 2;

    }

    // Como con profundidad 7, en las primeras jugadas hay veces que se supera el tiempo máximo y se usa una jugada parcial
    // se usa profundidad 7 cuando la partida ya está mas avanzada y el tiempo de computo de soluciones es menor
    if(profundidad == 1 && contarFichas() > 15){// 15 && contarFichas() < 30){

        NIVEL_DEFECTO = 7;

    }
    //if(profundidad == 1 && contarFichas() >= 30){
    //    NIVEL_DEFECTO = 8
    //}

    if(!tiempoExcedido){ // Se hace el minimax solo si no se ha superado el tiempo máximo de tirada (1s)

        //Este if contiene una tirada de libro con el que si el tablero esta vacío, es decir le toca a la IA realizar la
        // primera tirada, la primera ficha siempre se tire al centro;
        if (primera && tableroVacio()){ // Si es la primera tirada y si el tablero está vacío

            iterador = 3; // se elige la columna del centro
            //primera = false; // Se cambia la variable primera a false apra no volver a entrar en este bucle

        }
        else{

            // Si el nodo es hoja se calcula la valoración del tablero restando los puntos del jugador a evaluar menos
            // los puntos de tablero del oponente
            if(nodoHoja(profundidad)){
                int puntos = contarPuntos(jugador) - contarPuntos(oponente);

                return puntos;
            }
            else{

                // Si la profundidad es par, quiere decir que el nodo es MAX
                if(profundidad%2 == 0){
                    profundidad++;
                    for(int i = 0; i < COLS_TABLERO; i++){ // Se comprueba una tirada en cada columna

                        int fila = tablero->comprobarColumna(i); // Se comprueba que la columna no esté llena

                        if(fila != -1){ // Si la columna no está llena

                            tablero->ponerFicha(i,casilla); // Se coloca la ficha en la columna correspondiente

                            int max = minimax(oponente,profundidad, alfa, beta); //Llamamos de forma recursiva a la funcion minimax
                                                                                   // para comprobar el nodo hijo MIN aumentando la
                                                                                   // profundidad y usando poda alfa-beta

                            if(max > alfa){ // alfa = max(alfa, max), si el valor actual es mejor que el mejor valor
                                            // hasta el momento, se actualiza

                                alfa = max;
                                if(profundidad == 1){ // Si estamos en el primer nivel de profundidad, se elige la columna
                                                      // actual como columna prometedora para hacer la jugada y se emite
                                                      // la señal de jugada parcial por si acaso se termina el tiempo de calculo
                                    iterador = i;
                                    emit resultadoParcial(iterador);

                                }

                            }

                            // Se elimina la ficha puesta después de comprobar su jugada
                            tablero->cambiarCasilla(fila, i, 0);

                            // Parte de al poda alfa-beta, si alfa es mayor o igual a beta, se devuelve beta
                            if(alfa >= beta){

                                return beta;

                            }

                        }

                    }

                    return alfa;

                }
                else{
                    profundidad++;
                    for(int i = 0; i < COLS_TABLERO; i++){ // Se comprueban todas las columnas

                        int fila = tablero->comprobarColumna(i); // Se comprueba si la columna está vacía

                        if(fila !=-1){ // Si la columna no está vacía

                            tablero->ponerFicha(i,casilla); // Se coloca la ficha
                            beta = min(beta, minimax(jugador, profundidad, alfa, beta)); //Se elige el valor mínimo entre
                                                                                           // el mejor valor hasta el momento
                                                                                           // y el valor actual
                            tablero->cambiarCasilla(fila, i, 0); // Se elimina la ficha puesta

                            if(alfa >= beta){ // Parte de la poda alfa-beta, si beta es menor o igual a alfa se devuelve alfa

                                return alfa;

                            }

                        }

                    }

                    return beta;

                }

            }

        }

    }

}
