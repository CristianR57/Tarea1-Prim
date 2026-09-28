#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <set>
#include <utility>
#include <cmath>
#include <string>

using namespace std;

int main() {

    // ==========================================
    // Parámetros del grafo
    // ==========================================

    int i = 20;
    int j = 23;

    int v = pow(2, i);
    int e = pow(2, j);


    // ==========================================
    // Generar 10 grafos
    // usando semillas 1, 2, ..., 10
    // ==========================================

    for (int semilla = 1; semilla <= 10; semilla++) {

        cout << endl;
        cout << "==========================================" << endl;
        cout << "Generando grafo con semilla: "
             << semilla
             << endl;
        cout << "==========================================" << endl;


        // ==========================================
        // Lista de adyacencia
        // ==========================================

        vector<vector<pair<int, double>>> grafo(v);


        // ==========================================
        // Generador aleatorio
        // ==========================================

        mt19937 generador(semilla);


        // Pesos en (0,1]
        uniform_real_distribution<double> distribucionPeso(
            0.000001,
            1.0
        );


        // ==========================================
        // Para controlar aristas repetidas
        // ==========================================

        set<pair<int, int>> aristas;


        // ==========================================
        // PASO 1:
        // Crear un árbol cobertor
        // ==========================================

        for (int nodo = 1; nodo < v; nodo++) {

            // Elegimos un nodo anterior al azar:
            // 0, 1, ..., nodo-1

            uniform_int_distribution<int> distribucionNodo(
                0,
                nodo - 1
            );

            int padre =
                distribucionNodo(generador);

            double peso =
                distribucionPeso(generador);


            // Como el grafo es no dirigido,
            // agregamos la arista en ambas listas.

            grafo[nodo].push_back(
                {padre, peso}
            );

            grafo[padre].push_back(
                {nodo, peso}
            );


            // Guardamos la arista para evitar
            // duplicados posteriormente.

            aristas.insert(
                {padre, nodo}
            );
        }


        // ==========================================
        // PASO 2:
        // Agregar las aristas restantes
        // ==========================================

        while ((int)aristas.size() < e) {

            uniform_int_distribution<int> distribucionNodo(
                0,
                v - 1
            );

            int u =
                distribucionNodo(generador);

            int w =
                distribucionNodo(generador);


            // No permitimos loops.

            if (u == w) {
                continue;
            }


            // Como el grafo es no dirigido,
            // dejamos siempre el menor primero.

            if (u > w) {
                swap(u, w);
            }


            // Si ya existe, sorteamos otra.

            if (aristas.count({u, w}) > 0) {
                continue;
            }


            // Nueva arista

            double peso =
                distribucionPeso(generador);


            // Lista de adyacencia

            grafo[u].push_back(
                {w, peso}
            );

            grafo[w].push_back(
                {u, peso}
            );


            // Registramos la arista

            aristas.insert(
                {u, w}
            );
        }


        // ==========================================
        // PASO 3:
        // Guardar el grafo en un archivo
        // ==========================================

        string nombreArchivo =
            "grafo_i" + to_string(i) +
            "_j" + to_string(j) +
            "_seed" + to_string(semilla) +
            ".txt";


        ofstream archivo(nombreArchivo);


        if (!archivo.is_open()) {

            cerr << "ERROR: No se pudo crear el archivo "
                 << nombreArchivo
                 << endl;

            return 1;
        }


        // Primera línea:
        // cantidad de nodos y cantidad de aristas

        archivo << v
                << " "
                << e
                << endl;


        // Guardamos la lista de adyacencia.

        for (int nodo = 0;
             nodo < v;
             nodo++) {

            archivo << nodo << ":";


            for (auto arista : grafo[nodo]) {

                int vecino =
                    arista.first;

                double peso =
                    arista.second;


                archivo << " "
                        << vecino
                        << " "
                        << peso;
            }


            archivo << endl;
        }


        archivo.close();


        // ==========================================
        // Información por pantalla
        // ==========================================

        cout << "Grafo generado correctamente."
             << endl;

        cout << "Nodos: "
             << v
             << endl;

        cout << "Aristas: "
             << e
             << endl;

        cout << "Semilla: "
             << semilla
             << endl;

        cout << "Archivo: "
             << nombreArchivo
             << endl;
    }


    cout << endl;
    cout << "==========================================" << endl;
    cout << "Se generaron los 10 grafos correctamente."
         << endl;
    cout << "==========================================" << endl;


    return 0;
}