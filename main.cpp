/* Lee un tga de 24 bits, hace un proceso básico y escribe un tga out.tga      */
/* out.tga se puede visualizar con photoshop                                   */
/* Para generar un tga, con photoshop coger cualquier imagen, y guardarla como */
/* tga a 24 bits sin compresion, a menor resolucion de 1200x1200               */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <list>
#include <cstdlib>

using namespace std;

#define SALIDA "out.tga"            // el archivo de salida
#define MAXRX 1200                  // el tamaño máximo del array en x
#define MAXRY 1200                  // y en y

unsigned char Img[MAXRX][MAXRY][3]; // aqui se guarda la imagen, tres bytes R, G, B
// Un array es muchisimo más rápido que un array de estructuras
// o una estructura de tres arrays

// array auxiliar para las transformaciones locales
unsigned char ImgAux[MAXRX][MAXRY][3];

unsigned int Histograma[256]; // tenemos un histograma donde cada posicion guarda las repeticiones de ese valor de RGB

int Rx, Ry; // la Resolución de la imagen

void inicializarHistograma() {
    for (int i = 0; i < 256; i++) {
        Histograma[i] = 0;
    }

}

void rellenarHistograma() {

    int x, y, valorR, valorG, valorB, mediaRGB;
    for (y = 0; y < Ry; y++) //aqui leo la imagen
        for (x = 0; x < Rx; x++) {
            // guardamos cada valor de pixel en su correspondiente sitio del histograma
            valorR = (int) Img[x][y][0];
            valorG = (int) Img[x][y][1];
            valorB = (int) Img[x][y][2];
            mediaRGB = (valorR + valorG + valorB) / 3;
            Histograma[mediaRGB] = Histograma[mediaRGB] + 1;
        }
}

bool LeeTga(char na[]) // Lee un tga 24 de bits.
{
    FILE *fe;
    int x, y, i;
    unsigned char c, resto, cociente;
    ifstream ifile(na);
    if (ifile) {
        //Si existe el fichero entonces ejecutamos
        fe = fopen(na, "rb");
        if (fe == NULL) {

            printf("Error al abrir archivo %s\nPulsa...", na);

            getch();
            exit(1);

        }

        for (i = 0; i < 12; i++) //leo 12 bytes de la cabecera que no se usan para nada
            c = fgetc(fe);

        resto = fgetc(fe); //La resolucion se codifica en dos bytes
        cociente = fgetc(fe); //el primero es el resto de dividir por 256
        Rx = cociente * 256 + resto; //y el segundo el cociente. Y lo recompongo

        resto = fgetc(fe); // igual para la Ry
        cociente = fgetc(fe);
        Ry = cociente * 256 + resto;

        c = fgetc(fe);
        c = fgetc(fe); //los ultimos 2 bytes de la cabecera

        for (y = 0; y < Ry; y++) //aqui leo la imagen
            for (x = 0; x < Rx; x++) {
                Img[x][y][2] = fgetc(fe); //lee en orden B, G, R
                Img[x][y][1] = fgetc(fe);
                Img[x][y][0] = fgetc(fe);
            }

        fclose(fe);

        return true;
    }

    return false;
}

void GuardaTga(char* nombreFichero) // Escribe un TGA guardado en memoria, a 24 bits
{
    FILE *fs;
    int x, y;

    fs = fopen(nombreFichero, "wb"); //abro para escritura binaria
    if (fs == NULL) {
        printf("Error al crear archivo %s\nPulsa...", nombreFichero);
        getch();
        exit(1);
    }

    fputc(0, fs); /* 0 */ // guardo valores de cebecera: la mayoria son ceros
    fputc(0, fs); /* 1 */
    fputc(2, fs); /* 2 */
    fputc(0, fs); /* 3 */
    fputc(0, fs); /* 4 */
    fputc(0, fs); /* 5 */
    fputc(0, fs); /* 6 */
    fputc(0, fs); /* 7 */
    fputc(0, fs); /* 8 */
    fputc(0, fs); /* 9 */
    fputc(0, fs); /* 10 */
    fputc(0, fs); /* 11 */
    fputc(Rx % 256, fs); /* 12 */ //La resolucion x en dos bytes
    fputc(Rx / 256, fs); /* 13 */
    fputc(Ry % 256, fs); /* 14 */ //la resolucion y en dos bytes
    fputc(Ry / 256, fs); /* 15 */
    fputc(24, fs); /* 16 */ //indica 24 bits por pixel
    fputc(0, fs); /* 17 */ // acabo la cabecera

    for (y = 0; y < Ry; y++) // y aqui guardo la imagen por filas horizontales
        for (x = 0; x < Rx; x++) {
            fputc((char) Img[x][y][2], fs); //tres bytes por pixel, B, G, R
            fputc((char) Img[x][y][1], fs);
            fputc((char) Img[x][y][0], fs);
        }

    fclose(fs);
}

int normalizar(int numero) {
    if (numero <= 0)
        return 0;
    else if (numero >= 255)
        return 255;
    else
        return numero;
}

void bn() {
    int x, y;
    int r, g, b, newR, newG, newB, med;

    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            r = Img[x][y][0];
            g = Img[x][y][1];
            b = Img[x][y][2];
            med = (r + g + b) / 3;
            Img[x][y][0] = Img[x][y][1] = Img[x][y][2] = med;
        }
}

void bnPreciso() {
    int x, y;
    int r, g, b, med;

    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            r = Img[x][y][0]*0.21;
            g = Img[x][y][1]*0.72;
            b = Img[x][y][2]*0.07;
            med = r + g + b;
            Img[x][y][0] = Img[x][y][1] = Img[x][y][2] = med;
        }
}

/* CORRECTO */
void incrementarBrilloCte(int constante) {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] + constante);
            Img[x][y][1] = normalizar(Img[x][y][1] + constante);
            Img[x][y][2] = normalizar(Img[x][y][2] + constante);
        }
}

/* CORRECTO */
void decrementarBrilloCte(int constante) {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] - constante);
            Img[x][y][1] = normalizar(Img[x][y][1] - constante);
            Img[x][y][2] = normalizar(Img[x][y][2] - constante);
        }
}

/* CORRECTO */
void incrementarBrilloFact(float factor) {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] * factor);
            Img[x][y][1] = normalizar(Img[x][y][1] * factor);
            Img[x][y][2] = normalizar(Img[x][y][2] * factor);
        }
}

/* CORRECTO */
void decrementarBrilloFact(float factor) {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] / factor);
            Img[x][y][1] = normalizar(Img[x][y][1] / factor);
            Img[x][y][2] = normalizar(Img[x][y][2] / factor);
        }
}

/* CORRECTO */
void negativo() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = 255 - Img[x][y][0];
            Img[x][y][1] = 255 - Img[x][y][1];
            Img[x][y][2] = 255 - Img[x][y][2];
        }
}

/* CORRECTO */
void ajusteLineal() {

    inicializarHistograma();
    rellenarHistograma();
    int maximo, minimo;
    int x, y;
    // calculamos los percentiles en funcion del numero de pixels que tengamos
    float resolucion = Rx * Ry;
    int percentilCinco;
    int percentilNoventaYCinco;
    percentilCinco = resolucion * 0.05;
    percentilNoventaYCinco = resolucion * 0.95;

    // una vez tengamos los percentiles, tenemos que coger del histograma el minimo y el maximo
    // cuando lleguemos al primer valor que deje el 5% de los pixeles a la izquierda, tendremos el minimo
    float sumaTotal;
    sumaTotal = 0;
    // Minimo
    for (int i = 0; i < 256; i++) {
        sumaTotal += Histograma[i];
        if (sumaTotal >= percentilCinco) {
            minimo = i;
            break;
        }
    }
    sumaTotal = 0;
    // Maximo
    for (int i = 0; i < 256; i++) {
        sumaTotal += Histograma[i];
        if (sumaTotal >= percentilNoventaYCinco) {
            maximo = i;
            break;
        }
    }
    cout << "Aplicando maximo de: " << maximo << endl;
    cout << "Aplicando minimo de: " << minimo << endl;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            //f(v)= ((v-m)·255)/(M-m)
            //normalizando los valores
            Img[x][y][0] = normalizar(((Img[x][y][0] - minimo)*255) / (maximo - minimo));
            Img[x][y][1] = normalizar(((Img[x][y][1] - minimo)*255) / (maximo - minimo));
            Img[x][y][2] = normalizar(((Img[x][y][2] - minimo)*255) / (maximo - minimo));
        }
}

/* CORRECTO */
void gama(double valorGama) {
    int x, y;
    int r, g, b;
    float auxR, auxG, auxB;
    float baseR, baseG, baseB, exponente;
    exponente = 1 / valorGama;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            // convertimos los valores que tenemos en el array en float, sino al dividir entre 255 da 0
            auxR = Img[x][y][0];
            auxG = Img[x][y][1];
            auxB = Img[x][y][2];

            baseR = (auxR / 255);
            baseG = (auxG / 255);
            baseB = (auxB / 255);

            // con pow hacemos la potencia del primer numero al segundo
            // tenemos que pasarle los valores como reales
            r = 255 * pow(baseR, exponente);
            g = 255 * pow(baseG, exponente);
            b = 255 * pow(baseB, exponente);

            Img[x][y][0] = r;
            Img[x][y][1] = g;
            Img[x][y][2] = b;
        }
}

/* CORRECTO */
void umbralizacion(int umbral) {
    int x, y;
    int i; // i = R+G+B/3 calculamos la luminosidad media
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            i = (Img[x][y][0] + Img[x][y][1] + Img[x][y][2]) / 3;
            // un umbral bajo producira mas blancos y un umbral alto mas negros
            if (i > umbral) {
                // blanco
                Img[x][y][0] = 255;
                Img[x][y][1] = 255;
                Img[x][y][2] = 255;
            } else {
                // negro 
                Img[x][y][0] = 0;
                Img[x][y][1] = 0;
                Img[x][y][2] = 0;
            }
        }
}

/* CORRECTO */
void viraje(int r, int g, int b) {
    int x, y;
    int i; // i = R+G+B/3 calculamos la luminosidad media
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            i = (Img[x][y][0] + Img[x][y][1] + Img[x][y][2]) / 3;
            if (i < 128) {
                Img[x][y][0] = r * i / 128;
                Img[x][y][1] = g * i / 128;
                Img[x][y][2] = b * i / 128;
            } else {
                Img[x][y][0] = normalizar(r + (255 - r)*(i - 128) / 128);
                Img[x][y][1] = normalizar(g + (255 - g)*(i - 128) / 128);
                Img[x][y][2] = normalizar(b + (255 - b)*(i - 128) / 128);
            }
        }
}

/* CORRECTO */
void balanceBlancosSuma(int r, int g, int b) {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] + r);
            Img[x][y][1] = normalizar(Img[x][y][1] + g);
            Img[x][y][2] = normalizar(Img[x][y][2] + b);
        }
}

/* CORRECTO */
void balanceBlancosProducto(float r, float g, float b) {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] * r);
            Img[x][y][1] = normalizar(Img[x][y][1] * g);
            Img[x][y][2] = normalizar(Img[x][y][2] * b);
        }
}

/* CORRECTO */
void suavizadoA4() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar((Img[x][y][0] + Img[x - 1][y][0] + Img[x][y - 1][0] + Img[x - 1][y - 1][0]) / 4);
            ImgAux[x][y][1] = normalizar((Img[x][y][1] + Img[x - 1][y][1] + Img[x][y - 1][1] + Img[x - 1][y - 1][1]) / 4);
            ImgAux[x][y][2] = normalizar((Img[x][y][2] + Img[x - 1][y][2] + Img[x][y - 1][2] + Img[x - 1][y - 1][2]) / 4);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void suavizadoA9() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar((Img[x - 1][y - 1][0] + Img[x - 1][y][0] + Img[x - 1][y + 1][0] + Img[x][y - 1][0] + Img[x][y][0] + Img[x][y + 1][0] + Img[x + 1][y - 1][0] + Img[x + 1][y][0] + Img[x + 1][y + 1][0]) / 9);
            ImgAux[x][y][1] = normalizar((Img[x - 1][y - 1][1] + Img[x - 1][y][1] + Img[x - 1][y + 1][1] + Img[x][y - 1][1] + Img[x][y][1] + Img[x][y + 1][1] + Img[x + 1][y - 1][1] + Img[x + 1][y][1] + Img[x + 1][y + 1][1]) / 9);
            ImgAux[x][y][2] = normalizar((Img[x - 1][y - 1][2] + Img[x - 1][y][2] + Img[x - 1][y + 1][2] + Img[x][y - 1][2] + Img[x][y][2] + Img[x][y + 1][2] + Img[x + 1][y - 1][2] + Img[x + 1][y][2] + Img[x + 1][y + 1][2]) / 9);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void restarValorIzdo() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar(Img[x][y][0] - Img[x - 1][y][0]);
            ImgAux[x][y][1] = normalizar(Img[x][y][1] - Img[x - 1][y][1]);
            ImgAux[x][y][2] = normalizar(Img[x][y][2] - Img[x - 1][y][2]);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void restarValorSup() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar(Img[x][y][0] - Img[x][y - 1][0]);
            ImgAux[x][y][1] = normalizar(Img[x][y][1] - Img[x][y - 1][1]);
            ImgAux[x][y][2] = normalizar(Img[x][y][2] - Img[x][y - 1][2]);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void suavizadoGaussiano9() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar((Img[x - 1][y - 1][0] + (2 * Img[x - 1][y][0]) + Img[x - 1][y + 1][0] + (2 * Img[x][y - 1][0]) + (4 * Img[x][y][0]) + (2 * Img[x][y + 1][0]) + Img[x + 1][y - 1][0] + (2 * Img[x + 1][y][0]) + Img[x + 1][y + 1][0]) / 16);
            ImgAux[x][y][1] = normalizar((Img[x - 1][y - 1][1] + (2 * Img[x - 1][y][1]) + Img[x - 1][y + 1][1] + (2 * Img[x][y - 1][1]) + (4 * Img[x][y][1]) + (2 * Img[x][y + 1][1]) + Img[x + 1][y - 1][1] + (2 * Img[x + 1][y][1]) + Img[x + 1][y + 1][1]) / 16);
            ImgAux[x][y][2] = normalizar((Img[x - 1][y - 1][2] + (2 * Img[x - 1][y][2]) + Img[x - 1][y + 1][2] + (2 * Img[x][y - 1][2]) + (4 * Img[x][y][2]) + (2 * Img[x][y + 1][2]) + Img[x + 1][y - 1][2] + (2 * Img[x + 1][y][2]) + Img[x + 1][y + 1][2]) / 16);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void restarValorDiag() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar(Img[x][y][0] - Img[x - 1][y - 1][0]);
            ImgAux[x][y][1] = normalizar(Img[x][y][1] - Img[x - 1][y - 1][1]);
            ImgAux[x][y][2] = normalizar(Img[x][y][2] - Img[x - 1][y - 1][2]);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void bajorrelieve() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = normalizar(Img[x][y][0] - Img[x - 1][y - 1][0] + 128);
            ImgAux[x][y][1] = normalizar(Img[x][y][1] - Img[x - 1][y - 1][1] + 128);
            ImgAux[x][y][2] = normalizar(Img[x][y][2] - Img[x - 1][y - 1][2] + 128);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

int maximo2(int D1, int D2) {
    if (D1 > D2)
        return D1;
    else
        return D2;
}

int minimo2(int D1, int D2) {
    if (D1 > D2)
        return D2;
    else
        return D1;
}

int maximo4(int D1, int D2, int D3, int D4) {
    return (maximo2(maximo2(maximo2(D1, D2), D3), D4));

}

int minimo4(int D1, int D2, int D3, int D4) {
    return (minimo2(minimo2(minimo2(D1, D2), D3), D4));
}

int maximo9(int num1, int num2, int num3, int num4, int num5, int num6, int num7, int num8, int num9) {
    return maximo4(maximo4(num1, num2, num3, num4), num5, maximo2(num6, num7), maximo2(num8, num9));
}

int minimo9(int num1, int num2, int num3, int num4, int num5, int num6, int num7, int num8, int num9) {
    return minimo4(minimo4(num1, num2, num3, num4), num5, minimo2(num6, num7), minimo2(num8, num9));
}

int mediana9(int num1, int num2, int num3, int num4, int num5, int num6, int num7, int num8, int num9) {
    list<int> * listaNumeros;
    listaNumeros = new list<int>();
    listaNumeros->push_back(num1);
    listaNumeros->push_back(num2);
    listaNumeros->push_back(num3);
    listaNumeros->push_back(num4);
    listaNumeros->push_back(num5);
    listaNumeros->push_back(num6);
    listaNumeros->push_back(num7);
    listaNumeros->push_back(num8);
    listaNumeros->push_back(num9);
    listaNumeros->sort();

    listaNumeros->pop_back();
    listaNumeros->pop_back();
    listaNumeros->pop_back();
    listaNumeros->pop_back();

    return listaNumeros->back();
}

/* CORRECTO */
void maximoGradienteSimple() {
    int x, y;
    int D1, D2, D3, D4;

    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            // CANAL R
            D1 = Img[x][y][0] - Img[x - 1][y][0];
            D2 = Img[x][y][0] - Img[x][y - 1][0];
            D3 = Img[x][y][0] - Img[x - 1][y - 1][0];
            D4 = Img[x][y][0] - Img[x + 1][y + 1][0];
            ImgAux[x][y][0] = normalizar(maximo4(D1, D2, D3, D4));

            // CANAL G
            D1 = Img[x][y][1] - Img[x - 1][y][1];
            D2 = Img[x][y][1] - Img[x][y - 1][1];
            D3 = Img[x][y][1] - Img[x - 1][y - 1][1];
            D4 = Img[x][y][1] - Img[x + 1][y + 1][1];
            ImgAux[x][y][1] = normalizar(maximo4(D1, D2, D3, D4));

            // CANAL B 
            D1 = Img[x][y][2] - Img[x - 1][y][2];
            D2 = Img[x][y][2] - Img[x][y - 1][2];
            D3 = Img[x][y][2] - Img[x - 1][y - 1][2];
            D4 = Img[x][y][2] - Img[x + 1][y + 1][2];
            ImgAux[x][y][2] = normalizar(maximo4(D1, D2, D3, D4));
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void maximoGradiente() {
    int x, y;
    int D1, D2, D3, D4;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            // CANAL R
            D1 = -Img[x - 1][y + 1][0] - Img[x][y + 1][0] - Img[x + 1][y + 1][0] + Img[x - 1][y - 1][0] + Img[x][y - 1][0] + Img[x + 1][y - 1][0];
            D2 = -Img[x - 1][y + 1][0] - Img[x][y + 1][0] - Img[x - 1][y][0] + Img[x + 1][y][0] + Img[x][y - 1][0] + Img[x + 1][y - 1][0];
            D3 = -Img[x - 1][y + 1][0] + Img[x + 1][y + 1][0] - Img[x - 1][y][0] + Img[x + 1][y][0] - Img[x - 1][y - 1][0] + Img[x + 1][y - 1][0];
            D4 = Img[x][y + 1][0] + Img[x + 1][y + 1][0] - Img[x - 1][y][0] + Img[x + 1][y][0] - Img[x - 1][y - 1][0] - Img[x][y - 1][0];
            ImgAux[x][y][0] = normalizar(maximo4(D1, D2, D3, D4));

            // CANAL G
            D1 = -Img[x - 1][y + 1][1] - Img[x][y + 1][1] - Img[x + 1][y + 1][1] + Img[x - 1][y - 1][1] + Img[x][y - 1][1] + Img[x + 1][y - 1][1];
            D2 = -Img[x - 1][y + 1][1] - Img[x][y + 1][1] - Img[x - 1][y][1] + Img[x + 1][y][1] + Img[x][y - 1][1] + Img[x + 1][y - 1][1];
            D3 = -Img[x - 1][y + 1][1] + Img[x + 1][y + 1][1] - Img[x - 1][y][1] + Img[x + 1][y][1] - Img[x - 1][y - 1][1] + Img[x + 1][y - 1][1];
            D4 = Img[x][y + 1][1] + Img[x + 1][y + 1][1] - Img[x - 1][y][1] + Img[x + 1][y][1] - Img[x - 1][y - 1][1] - Img[x][y - 1][1];
            ImgAux[x][y][1] = normalizar(maximo4(D1, D2, D3, D4));

            // CANAL B 
            D1 = -Img[x - 1][y + 1][2] - Img[x][y + 1][2] - Img[x + 1][y + 1][2] + Img[x - 1][y - 1][2] + Img[x][y - 1][2] + Img[x + 1][y - 1][2];
            D2 = -Img[x - 1][y + 1][2] - Img[x][y + 1][2] - Img[x - 1][y][2] + Img[x + 1][y][2] + Img[x][y - 1][2] + Img[x + 1][y - 1][2];
            D3 = -Img[x - 1][y + 1][2] + Img[x + 1][y + 1][2] - Img[x - 1][y][2] + Img[x + 1][y][2] - Img[x - 1][y - 1][2] + Img[x + 1][y - 1][2];
            D4 = Img[x][y + 1][2] + Img[x + 1][y + 1][2] - Img[x - 1][y][2] + Img[x + 1][y][2] - Img[x - 1][y - 1][2] - Img[x][y - 1][2];
            ImgAux[x][y][2] = normalizar(maximo4(D1, D2, D3, D4));
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void laplace() {
    int x, y, valorPixel;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            //Canal R
            valorPixel = ((8 * Img[x][y][0]) - Img[x - 1][y - 1][0] - Img[x][y - 1][0] - Img[x + 1][y + 1][0] - Img[x - 1][y][0] - Img[x + 1][y][0] - Img[x + 1][y - 1][0] - Img[x + 1][y][0] - Img[x + 1][y + 1][0]);
            ImgAux[x][y][0] = normalizar(valorPixel);

            // CANAL G
            valorPixel = ((8 * Img[x][y][1]) - Img[x - 1][y - 1][1] - Img[x][y - 1][1] - Img[x + 1][y + 1][1] - Img[x - 1][y][1] - Img[x + 1][y][1] - Img[x + 1][y - 1][1] - Img[x + 1][y][1] - Img[x + 1][y + 1][1]);
            ImgAux[x][y][1] = normalizar(valorPixel);

            // CANAL B
            valorPixel = ((8 * Img[x][y][2]) - Img[x - 1][y - 1][2] - Img[x][y - 1][2] - Img[x + 1][y + 1][2] - Img[x - 1][y][2] - Img[x + 1][y][2] - Img[x + 1][y - 1][2] - Img[x + 1][y][2] - Img[x + 1][y + 1][2]);
            ImgAux[x][y][2] = normalizar(valorPixel);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void perfilado() {

    // simplemente hacemos laplace y sumamos el valor
    int x, y, valorPixel;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            //Canal R
            valorPixel = ((8 * Img[x][y][0]) - Img[x - 1][y - 1][0] - Img[x][y - 1][0] - Img[x + 1][y + 1][0] - Img[x - 1][y][0] - Img[x + 1][y][0] - Img[x + 1][y - 1][0] - Img[x + 1][y][0] - Img[x + 1][y + 1][0]);
            ImgAux[x][y][0] = normalizar(valorPixel);

            // CANAL G
            valorPixel = ((8 * Img[x][y][1]) - Img[x - 1][y - 1][1] - Img[x][y - 1][1] - Img[x + 1][y + 1][1] - Img[x - 1][y][1] - Img[x + 1][y][1] - Img[x + 1][y - 1][1] - Img[x + 1][y][1] - Img[x + 1][y + 1][1]);
            ImgAux[x][y][1] = normalizar(valorPixel);

            // CANAL B
            valorPixel = ((8 * Img[x][y][2]) - Img[x - 1][y - 1][2] - Img[x][y - 1][2] - Img[x + 1][y + 1][2] - Img[x - 1][y][2] - Img[x + 1][y][2] - Img[x + 1][y - 1][2] - Img[x + 1][y][2] - Img[x + 1][y + 1][2]);
            ImgAux[x][y][2] = normalizar(valorPixel);
        }

    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = normalizar(Img[x][y][0] + ImgAux[x][y][0]);
            Img[x][y][1] = normalizar(Img[x][y][1] + ImgAux[x][y][1]);
            Img[x][y][2] = normalizar(Img[x][y][2] + ImgAux[x][y][2]);
        }
}

/* CORRECTO */
void maximo3x3() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = maximo9(Img[x - 1][y - 1][0], Img[x - 1][y][0], Img[x - 1][y + 1][0], Img[x][y - 1][0], Img[x][y][0], Img[x][y + 1][0], Img[x + 1][y - 1][0], Img[x + 1][y][0], Img[x + 1][y + 1][0]);
            ImgAux[x][y][1] = maximo9(Img[x - 1][y - 1][1], Img[x - 1][y][1], Img[x - 1][y + 1][1], Img[x][y - 1][1], Img[x][y][1], Img[x][y + 1][1], Img[x + 1][y - 1][1], Img[x + 1][y][1], Img[x + 1][y + 1][1]);
            ImgAux[x][y][2] = maximo9(Img[x - 1][y - 1][2], Img[x - 1][y][2], Img[x - 1][y + 1][2], Img[x][y - 1][2], Img[x][y][2], Img[x][y + 1][2], Img[x + 1][y - 1][2], Img[x + 1][y][2], Img[x + 1][y + 1][2]);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void minimo3x3() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = minimo9(Img[x - 1][y - 1][0], Img[x - 1][y][0], Img[x - 1][y + 1][0], Img[x][y - 1][0], Img[x][y][0], Img[x][y + 1][0], Img[x + 1][y - 1][0], Img[x + 1][y][0], Img[x + 1][y + 1][0]);
            ImgAux[x][y][1] = minimo9(Img[x - 1][y - 1][1], Img[x - 1][y][1], Img[x - 1][y + 1][1], Img[x][y - 1][1], Img[x][y][1], Img[x][y + 1][1], Img[x + 1][y - 1][1], Img[x + 1][y][1], Img[x + 1][y + 1][1]);
            ImgAux[x][y][2] = minimo9(Img[x - 1][y - 1][2], Img[x - 1][y][2], Img[x - 1][y + 1][2], Img[x][y - 1][2], Img[x][y][2], Img[x][y + 1][2], Img[x + 1][y - 1][2], Img[x + 1][y][2], Img[x + 1][y + 1][2]);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void mediana3x3() {
    int x, y;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            ImgAux[x][y][0] = mediana9(Img[x - 1][y - 1][0], Img[x - 1][y][0], Img[x - 1][y + 1][0], Img[x][y - 1][0], Img[x][y][0], Img[x][y + 1][0], Img[x + 1][y - 1][0], Img[x + 1][y][0], Img[x + 1][y + 1][0]);
            ImgAux[x][y][1] = mediana9(Img[x - 1][y - 1][1], Img[x - 1][y][1], Img[x - 1][y + 1][1], Img[x][y - 1][1], Img[x][y][1], Img[x][y + 1][1], Img[x + 1][y - 1][1], Img[x + 1][y][1], Img[x + 1][y + 1][1]);
            ImgAux[x][y][2] = mediana9(Img[x - 1][y - 1][2], Img[x - 1][y][2], Img[x - 1][y + 1][2], Img[x][y - 1][2], Img[x][y][2], Img[x][y + 1][2], Img[x + 1][y - 1][2], Img[x + 1][y][2], Img[x + 1][y + 1][2]);
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void difuminadoAleatorio(int radio) {
    int radioAleatorio;
    int x, y;
    int posicionX, posicionY;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            radioAleatorio = rand() % (2 * radio);
            posicionX = x + radioAleatorio - radio;
            posicionY = y + radioAleatorio - radio;
            // comprobar si se sale de los limites
            if (posicionX >= Rx || posicionY >= Ry || posicionX < 0 || posicionY < 0) {
                ImgAux[x][y][0] = Img[x][y][0];
                ImgAux[x][y][1] = Img[x][y][1];
                ImgAux[x][y][2] = Img[x][y][2];
            } else {
                ImgAux[x][y][0] = Img[posicionX][posicionY][0];
                ImgAux[x][y][1] = Img[posicionX][posicionY][1];
                ImgAux[x][y][2] = Img[posicionX][posicionY][2];
            }
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

/* CORRECTO */
void pixeladoN(int tamBloque) {
    int x, y;
    int posicionX, posicionY;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            posicionX = (x / tamBloque) * tamBloque;
            posicionY = (y / tamBloque) * tamBloque;
            // comprobar si se sale de los limites
            if (posicionX >= Rx || posicionY >= Ry || posicionX < 0 || posicionY < 0) {
                ImgAux[x][y][0] = Img[x][y][0];
                ImgAux[x][y][1] = Img[x][y][1];
                ImgAux[x][y][2] = Img[x][y][2];
            } else {
                ImgAux[x][y][0] = Img[posicionX][posicionY][0];
                ImgAux[x][y][1] = Img[posicionX][posicionY][1];
                ImgAux[x][y][2] = Img[posicionX][posicionY][2];
            }
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

void cristalCuadriculado(int tamCuadricula) {
    int x, y;
    int posicionX, posicionY;
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            posicionX = x - (x % tamCuadricula) + (y % tamCuadricula);
            posicionY = y - (y % tamCuadricula) + (x % tamCuadricula);
            // comprobar si se sale de los limites
            if (posicionX >= Rx || posicionY >= Ry || posicionX < 0 || posicionY < 0) {
                ImgAux[x][y][0] = Img[x][y][0];
                ImgAux[x][y][1] = Img[x][y][1];
                ImgAux[x][y][2] = Img[x][y][2];
            } else {
                ImgAux[x][y][0] = Img[posicionX][posicionY][0];
                ImgAux[x][y][1] = Img[posicionX][posicionY][1];
                ImgAux[x][y][2] = Img[posicionX][posicionY][2];
            }
        }
    // asignamos la imagen cambiada a la imagen que se guarda
    for (y = 0; y < Ry; y++)
        for (x = 0; x < Rx; x++) {
            Img[x][y][0] = ImgAux[x][y][0];
            Img[x][y][1] = ImgAux[x][y][1];
            Img[x][y][2] = ImgAux[x][y][2];
        }
}

int main() {
    string opcion;
    char na[100];
    int tipoTransformacion;
    char nombreFichero[100];
    // valor de la constante que sumamos o restamos, si la hubiera
    int constante;
    // valores que se piden para las transformaciones
    float factor;
    int r, g, b;
    float rF, gF, bF;
    double valorGama;
    int umbral;
    int radio;
    int tamBloque;
    int tamCuadricula;
    while (opcion != "5") {
        cout << "-------------MENU PRINCIPAL-------------\n";
        cout << "1. Aplicar transformaciones globales a un fichero\n";
        cout << "2. Aplicar transformaciones locales a un fichero\n";
        cout << "3. Aplicar transformaciones geometricas a un fichero\n";
        cout << "4. Aplicar transformaciones combinadas\n";
        cout << "5. Salir\n";
        cout << "----------------------------------------\n";
        cout << "Introduce una opcion: ";
        /* En funcion de la opcion elegida, debemos de elegir la imagen */
        cin >> opcion;
        if (opcion == "1") {
            printf("Nombre de archivo : ");
            cin >> na;
            if (LeeTga(na)) {
                printf("Resolucion de %d x %d\n", Rx, Ry);
                cout << "Tipos de transformaciones\n";
                cout << "       1. Convertir a blanco y negro\n";
                cout << "       2. Incrementar brillo con constante\n";
                cout << "       3. Decrementar brillo con constante\n";
                cout << "       4. Incrementar brillo en un factor\n";
                cout << "       5. Decrementar brillo en un factor\n";
                cout << "       6. Convertir a negativo\n";
                cout << "       7. Aplicar ajuste lineal\n";
                cout << "       8. Aplicar gama\n";
                cout << "       9. Aplicar umbral\n";
                cout << "       10. Aplicar viraje a un color\n";
                cout << "       11. Aplicar balance de blancos con suma\n";
                cout << "       12. Aplicar balance de blancos con producto\n";
                cout << "Elige un tipo de transformacion: ";
                cin >> tipoTransformacion;
                switch (tipoTransformacion) {
                    case 1:
                        cout << "Convirtiendo a blanco y negro...\n";
                        bnPreciso();
                        strcpy(nombreFichero, "BN-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 2:
                        cout << "Introduce el valor de la constante: ";
                        cin >> constante;
                        cout << "Aplicando incremento de brillo...";
                        incrementarBrilloCte(constante);
                        strcpy(nombreFichero, "IncrBrilloCte-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 3:
                        cout << "Introduce el valor de la constante: ";
                        cin >> constante;
                        cout << "Aplicando decremento de brillo...";
                        decrementarBrilloCte(constante);
                        strcpy(nombreFichero, "DecrBrilloCte-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 4:
                        cout << "Introduce el valor del factor: ";
                        cin >> factor;
                        cout << "Aplicando incremento de brillo...";
                        incrementarBrilloFact(factor);
                        strcpy(nombreFichero, "IncrBrilloFact-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 5:
                        cout << "Introduce el valor del factor: ";
                        cin >> factor;
                        cout << "Aplicando decremento de brillo...";
                        decrementarBrilloFact(factor);
                        strcpy(nombreFichero, "DecrBrilloFact-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 6:
                        cout << "Convirtiendo a negativo...\n";
                        negativo();
                        strcpy(nombreFichero, "Negativo-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 7:
                        cout << "Aplicando ajuste lineal...\n";
                        ajusteLineal();
                        strcpy(nombreFichero, "AjusteLineal-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 8:
                        cout << "Introduce el valor de gama a aplicar: ";
                        cin >> valorGama;
                        cout << "Aplicando gama...\n";
                        gama(valorGama);
                        strcpy(nombreFichero, "Gama-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 9:
                        cout << "Introduce el umbral a aplicar: ";
                        cin >> umbral;
                        if (umbral > 255 || umbral < 0)
                            cout << "Umbral incorrecto, debe de estar entre 0-255. Volviendo a la pantalla principal...\n";
                        else {
                            cout << "Aplicando umbralizacion...\n";
                            umbralizacion(umbral);
                            strcpy(nombreFichero, "Umbralizacion-");
                            strcat(nombreFichero, na);
                            GuardaTga(nombreFichero);
                            cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        }
                        break;
                    case 10:
                        cout << "Introduce el viraje R a aplicar: ";
                        cin >> r;
                        cout << "Introduce el viraje G a aplicar: ";
                        cin >> g;
                        cout << "Introduce el viraje B a aplicar: ";
                        cin >> b;
                        if (r > 255 || r < 0 || g > 255 || g < 0 || b > 255 || b < 0) {
                            cout << "Los valores RGB deben de situarse entre 0-255. Volviendo al menu principal...\n";
                        } else {
                            cout << "Aplicando viraje...\n";
                            viraje(r, g, b);
                            strcpy(nombreFichero, "Viraje-");
                            strcat(nombreFichero, na);
                            GuardaTga(nombreFichero);
                            cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        }
                        break;
                    case 11:
                        cout << "Introduce la constante que se suma a R: ";
                        cin >> r;
                        cout << "Introduce la constante que se suma a G: ";
                        cin >> g;
                        cout << "Introduce la constante que se suma a B: ";
                        cin >> b;
                        cout << "Aplicando el balance de blancos con suma...\n";
                        balanceBlancosSuma(r, g, b);
                        strcpy(nombreFichero, "BBSuma-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 12:
                        cout << "Introduce la constante que se multiplica a R: ";
                        cin >> rF;
                        cout << "Introduce la constante que se multiplica a G: ";
                        cin >> gF;
                        cout << "Introduce la constante que se multiplica a B: ";
                        cin >> bF;
                        cout << "Aplicando el balance de blancos con producto...\n";
                        balanceBlancosProducto(rF, gF, bF);
                        strcpy(nombreFichero, "BBProducto-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    default:
                        cout << "La opcion elegida es incorrecta\n";
                }
            } else {
                cout << "El fichero no existe\n";
            }
        } else if (opcion == "2") {
            printf("Nombre de archivo : ");
            cin >> na;
            if (LeeTga(na)) {
                printf("Resolucion de %d x %d\n", Rx, Ry);
                cout << "Tipos de transformaciones\n";
                cout << "       1. Suavizado media de 4\n";
                cout << "       2. Suavizado media de 9\n";
                cout << "       3. Suavizado gaussiano de 9\n";
                cout << "       4. Bordes restando pixel izquierdo\n";
                cout << "       5. Bordes restando pixel superior\n";
                cout << "       6. Bordes restando pixel diagonal\n";
                cout << "       7. Bordes bajorrelieve\n";
                cout << "       8. Maximo gradiente sencillo\n";
                cout << "       9. Maximo gradiente avanzado\n";
                cout << "       10. Laplaciana\n";
                cout << "       11. Perfilado\n";
                cout << "       12. Maximo 3x3\n";
                cout << "       13. Minimo 3x3\n";
                cout << "       14. Mediana 3x3\n";
                cout << "Elige un tipo de transformacion: ";
                cin >> tipoTransformacion;
                switch (tipoTransformacion) {
                    case 1:
                        cout << "Aplicando suavizado media de 4...\n";
                        suavizadoA4();
                        strcpy(nombreFichero, "SuavizadoA4-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 2:
                        cout << "Aplicando suavizado media de 9...\n";
                        suavizadoA9();
                        strcpy(nombreFichero, "SuavizadoA9-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;

                    case 3:
                        cout << "Aplicando un suavizado gaussiano de 9...\n";
                        suavizadoGaussiano9();
                        strcpy(nombreFichero, "Gaussiano-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 4:
                        cout << "Aplicando la resta del pixel izquierdo...\n";
                        restarValorIzdo();
                        strcpy(nombreFichero, "RestaIzda-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 5:
                        cout << "Aplicando la resta del pixel superior...\n";
                        restarValorSup();
                        strcpy(nombreFichero, "RestaSup-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 6:
                        cout << "Aplicando la resta del pixel diagonal...\n";
                        restarValorDiag();
                        strcpy(nombreFichero, "RestaDiag-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 7:
                        cout << "Aplicando bajorrelieve...\n";
                        bajorrelieve();
                        strcpy(nombreFichero, "Bajorrelieve-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 8:
                        cout << "Aplicando maximo gradiente sencillo...\n";
                        maximoGradienteSimple();
                        strcpy(nombreFichero, "MaximoGradienteSimple-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 9:
                        cout << "Aplicando maximo gradiente avanzado...\n";
                        maximoGradiente();
                        strcpy(nombreFichero, "MaximoGradienteAv-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 10:
                        cout << "Aplicando mascara laplaciana...\n";
                        laplace();
                        strcpy(nombreFichero, "Laplaciana-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 11:
                        cout << "Aplicando perfilado...\n";
                        perfilado();
                        strcpy(nombreFichero, "Perfilado-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 12:
                        cout << "Aplicando maximo...\n";
                        maximo3x3();
                        strcpy(nombreFichero, "Maximo-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 13:
                        cout << "Aplicando minimo...\n";
                        minimo3x3();
                        strcpy(nombreFichero, "Minimo-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 14:
                        cout << "Aplicando mediana...\n";
                        mediana3x3();
                        strcpy(nombreFichero, "Mediana-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    default:
                        cout << "La opcion elegida es incorrecta\n";
                }
            } else {
                cout << "El fichero no existe\n";
            }

        } else if (opcion == "3") {
            printf("Nombre de archivo : ");
            cin >> na;
            if (LeeTga(na)) {
                printf("Resolucion de %d x %d\n", Rx, Ry);
                cout << "Tipos de transformaciones\n";
                cout << "       1. Difuminado aleatorio de radio r\n";
                cout << "       2. Pixelado de bloques n\n";
                cout << "       3. Cristal cuadriculado\n";
                cout << "Elige un tipo de transformacion: ";
                cin >> tipoTransformacion;
                switch (tipoTransformacion) {
                    case 1:
                        cout << "Aplicando difuminado aleatorio de radio r...\n";
                        cout << "Introduce el valor del radio: ";
                        cin >> radio;
                        difuminadoAleatorio(radio);
                        strcpy(nombreFichero, "DifuminadoRadio-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 2:
                        cout << "Aplicando pixelado de bloques de n...\n";
                        cout << "Introduce el tamano del bloque: ";
                        cin >> tamBloque;
                        pixeladoN(tamBloque);
                        strcpy(nombreFichero, "PixeladoN-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    case 3:
                        cout << "Aplicando cristal cuadriculado...\n";
                        cout << "Introduce el tamano de la cuadricula: ";
                        cin >> tamCuadricula;
                        cristalCuadriculado(tamCuadricula);
                        strcpy(nombreFichero, "CristalCuadriculado-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    default:
                        cout << "La opcion elegida es incorrecta\n";
                }
            } else {
                cout << "El fichero no existe\n";
            }
        } else if (opcion == "4") {
            printf("Nombre de archivo : ");
            cin >> na;
            if (LeeTga(na)) {
                printf("Resolucion de %d x %d\n", Rx, Ry);
                cout << "Tipos de transformaciones\n";
                cout << "       1. BN + Deteccion de bordes + Suavizado \n";
                cout << "Elige un tipo de transformacion: ";
                cin >> tipoTransformacion;
                switch (tipoTransformacion) {
                    case 1:
                        cout << "Aplicando transformacion...\n";
                        bnPreciso();
                        maximoGradiente();
                        suavizadoA4();
                        strcpy(nombreFichero, "Mezcla1-");
                        strcat(nombreFichero, na);
                        GuardaTga(nombreFichero);
                        cout << "Conversion finalizada, guardado como " << nombreFichero << "\n";
                        break;
                    default:
                        cout << "La opcion elegida es incorrecta\n";
                }
            } else {
                cout << "El fichero no existe\n";
            }
        } else if (opcion == "5") {
            cout << "Hasta la proxima";
        } else {
            cout << "Opcion Incorrecta\n";
        }
        if (opcion != "5") {
            cout << "Quiere realizar otra transformacion? [S/n]: ";
            char respuesta;
            cin >> respuesta;
            if (respuesta == 'n' || respuesta == 'N') {
                cout << "Hasta la proxima";
                opcion = "5";
            } else
                cout << endl;
        }
    }
}