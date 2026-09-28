#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <utility>
#include <chrono>

using namespace std;


// ============================================================
// NODO DE LA COLA BINOMIAL
// ============================================================

struct NodoBinomial {

    int vertex;
    double key;

    NodoBinomial* parent;
    NodoBinomial* child;
    NodoBinomial* sibling;

    int degree;

    NodoBinomial(int v, double k) {

        vertex = v;
        key = k;

        parent = nullptr;
        child = nullptr;
        sibling = nullptr;

        degree = 0;
    }
};


// ============================================================
// COLA BINOMIAL
// ============================================================

class ColaBinomial {

private:

    NodoBinomial* head;
    NodoBinomial* minimum;

    // pos[v] apunta al nodo que contiene al vertice v
    vector<NodoBinomial*> pos;


    // --------------------------------------------------------
    // LINK
    //
    // Une dos arboles binomiales del mismo grado.
    // --------------------------------------------------------

    NodoBinomial* link(
        NodoBinomial* arbol1,
        NodoBinomial* arbol2
    ) {

        // El arbol con menor clave queda como raiz
        if (arbol2->key < arbol1->key) {
            swap(arbol1, arbol2);
        }

        // arbol2 pasa a ser hijo de arbol1
        arbol2->parent = arbol1;

        arbol2->sibling = arbol1->child;

        arbol1->child = arbol2;

        arbol1->degree++;

        return arbol1;
    }


    // --------------------------------------------------------
    // MERGE LISTAS
    //
    // Combina dos listas de raices (arboles binomiales) y
    // consolida los arboles de igual grado, devolviendo la
    // nueva lista de raices resultante.
    //
    // A diferencia de la version anterior, esta funcion NO
    // depende del vector `pos` ni construye una ColaBinomial
    // temporal: opera unicamente sobre punteros a NodoBinomial.
    //
    // Esto es lo que permite que insert(), extractMin() y
    // merge() sean O(log n) en lugar de O(n): antes, cada una
    // de esas operaciones creaba una ColaBinomial temporal con
    // su propio vector `pos` de tamano n (temporal(pos.size())),
    // lo que costaba O(n) por llamada. Como extractMin() se
    // invoca n veces dentro de Prim, esto convertia el algoritmo
    // completo en O(n^2), inviable para n = 2^22.
    // --------------------------------------------------------

    NodoBinomial* mergeListas(
        NodoBinomial* head1,
        NodoBinomial* head2
    ) {

        vector<NodoBinomial*> grados;

        NodoBinomial* actual = head1;

        while (actual != nullptr) {

            NodoBinomial* siguiente = actual->sibling;

            actual->sibling = nullptr;
            actual->parent = nullptr;

            grados.push_back(actual);

            actual = siguiente;
        }

        actual = head2;

        while (actual != nullptr) {

            NodoBinomial* siguiente = actual->sibling;

            actual->sibling = nullptr;
            actual->parent = nullptr;

            grados.push_back(actual);

            actual = siguiente;
        }


        // ----------------------------------------------------
        // Consolidamos arboles con igual grado
        // ----------------------------------------------------

        vector<NodoBinomial*> tabla;

        for (NodoBinomial* arbol : grados) {

            NodoBinomial* actualArbol = arbol;

            int grado = actualArbol->degree;

            while (true) {

                if (grado >= (int)tabla.size()) {
                    tabla.resize(grado + 1, nullptr);
                }

                // No hay otro arbol de este grado
                if (tabla[grado] == nullptr) {

                    tabla[grado] = actualArbol;

                    break;
                }

                // Ya existe otro arbol del mismo grado
                NodoBinomial* otroArbol = tabla[grado];

                tabla[grado] = nullptr;

                actualArbol = link(
                    actualArbol,
                    otroArbol
                );

                grado = actualArbol->degree;
            }
        }


        // ----------------------------------------------------
        // Reconstruimos la lista de raices
        // ----------------------------------------------------

        NodoBinomial* nuevoHead = nullptr;
        NodoBinomial* ultimo = nullptr;

        for (NodoBinomial* arbol : tabla) {

            if (arbol == nullptr) {
                continue;
            }

            arbol->parent = nullptr;
            arbol->sibling = nullptr;

            if (nuevoHead == nullptr) {

                nuevoHead = arbol;
                ultimo = arbol;

            } else {

                ultimo->sibling = arbol;
                ultimo = arbol;
            }
        }

        return nuevoHead;
    }


    // --------------------------------------------------------
    // ACTUALIZAR MINIMUM
    // --------------------------------------------------------

    void actualizarMinimum() {

        minimum = nullptr;

        NodoBinomial* actual = head;

        while (actual != nullptr) {

            if (
                minimum == nullptr ||
                actual->key < minimum->key
            ) {

                minimum = actual;
            }

            actual = actual->sibling;
        }
    }


public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    ColaBinomial(int cantidadVertices) {

        head = nullptr;
        minimum = nullptr;

        pos.resize(cantidadVertices, nullptr);
    }


    // ========================================================
    // MERGE
    // ========================================================

    void merge(ColaBinomial& otra) {

        if (otra.head == nullptr) {
            return;
        }

        head = mergeListas(head, otra.head);

        actualizarMinimum();

        // La otra cola queda vacia
        otra.head = nullptr;
        otra.minimum = nullptr;
    }


    // ========================================================
    // INSERT
    // ========================================================

    void insert(int vertex, double key) {

        NodoBinomial* nodo =
            new NodoBinomial(vertex, key);

        head = mergeListas(head, nodo);

        pos[vertex] = nodo;

        actualizarMinimum();
    }


    // ========================================================
    // FIND MIN
    // ========================================================

    NodoBinomial* findMin() {

        return minimum;
    }


    // ========================================================
    // EXTRACT MIN
    // ========================================================

    NodoBinomial* extractMin() {

        if (minimum == nullptr) {
            return nullptr;
        }


        NodoBinomial* minimo = minimum;


        // ----------------------------------------------------
        // Buscamos el minimo en la lista de raices
        // ----------------------------------------------------

        NodoBinomial* anterior = nullptr;
        NodoBinomial* actual = head;

        while (actual != minimo) {

            anterior = actual;
            actual = actual->sibling;
        }


        // ----------------------------------------------------
        // Eliminamos el minimo de la lista de raices
        // ----------------------------------------------------

        if (anterior == nullptr) {

            head = minimo->sibling;

        } else {

            anterior->sibling = minimo->sibling;
        }


        // ----------------------------------------------------
        // Convertimos los hijos del minimo en una lista de
        // raices independiente
        // ----------------------------------------------------

        NodoBinomial* hijo = minimo->child;

        NodoBinomial* nuevoHead = nullptr;
        NodoBinomial* ultimoHijo = nullptr;

        while (hijo != nullptr) {

            NodoBinomial* siguiente = hijo->sibling;

            hijo->parent = nullptr;
            hijo->sibling = nullptr;

            if (nuevoHead == nullptr) {

                nuevoHead = hijo;
                ultimoHijo = hijo;

            } else {

                ultimoHijo->sibling = hijo;
                ultimoHijo = hijo;
            }

            hijo = siguiente;
        }


        // ----------------------------------------------------
        // El vertice extraido ya no esta en Q
        // ----------------------------------------------------

        pos[minimo->vertex] = nullptr;


        // ----------------------------------------------------
        // Fusionamos la lista de raices restante con los
        // hijos del minimo, consolidando arboles de igual
        // grado. mergeListas() no toca `pos`, por lo que este
        // paso es O(log n) y no O(n).
        // ----------------------------------------------------

        head = mergeListas(head, nuevoHead);

        actualizarMinimum();


        // ----------------------------------------------------
        // Desconectamos el nodo extraido
        // ----------------------------------------------------

        minimo->parent = nullptr;
        minimo->child = nullptr;
        minimo->sibling = nullptr;


        return minimo;
    }


    // ========================================================
    // DECREASE KEY
    // ========================================================

    void decreaseKey(
        int vertex,
        double newKey
    ) {

        if (
            vertex < 0 ||
            vertex >= (int)pos.size()
        ) {
            return;
        }


        NodoBinomial* x = pos[vertex];


        if (x == nullptr) {
            return;
        }


        if (newKey > x->key) {
            return;
        }


        x->key = newKey;


        // ----------------------------------------------------
        // Subimos el contenido del nodo.
        //
        // No modificamos la estructura del arbol.
        //
        // Intercambiamos:
        //      vertex
        //      key
        //
        // y actualizamos pos.
        // ----------------------------------------------------

        while (
            x->parent != nullptr &&
            x->key < x->parent->key
        ) {

            NodoBinomial* padre = x->parent;


            // Guardamos el contenido de x
            int vertexX = x->vertex;
            double keyX = x->key;


            // Intercambiamos contenidos
            x->vertex = padre->vertex;
            x->key = padre->key;

            padre->vertex = vertexX;
            padre->key = keyX;


            // Actualizamos pos
            pos[x->vertex] = x;
            pos[padre->vertex] = padre;


            // Continuamos desde el padre
            x = padre;
        }


        // Actualizamos minimo
        if (
            minimum == nullptr ||
            x->key < minimum->key
        ) {

            minimum = x;
        }
    }


    // ========================================================
    // HEAPIFY
    // ========================================================

    void heapify(
        const vector<double>& costos
    ) {

        int n = costos.size();


        if (n != (int)pos.size()) {
            return;
        }


        head = nullptr;
        minimum = nullptr;


        vector<NodoBinomial*> grados;


        // ----------------------------------------------------
        // Creamos todos los arboles B0 y los consolidamos
        // ----------------------------------------------------

        for (int vertex = 0; vertex < n; vertex++) {

            NodoBinomial* actual =
                new NodoBinomial(
                    vertex,
                    costos[vertex]
                );


            pos[vertex] = actual;


            int grado = actual->degree;


            while (true) {

                if (grado >= (int)grados.size()) {
                    grados.resize(grado + 1, nullptr);
                }


                if (grados[grado] == nullptr) {

                    grados[grado] = actual;

                    break;
                }


                NodoBinomial* otro =
                    grados[grado];

                grados[grado] = nullptr;


                actual = link(
                    actual,
                    otro
                );


                grado = actual->degree;
            }
        }


        // ----------------------------------------------------
        // Reconstruimos lista de raices
        // ----------------------------------------------------

        NodoBinomial* ultimo = nullptr;


        for (NodoBinomial* arbol : grados) {

            if (arbol == nullptr) {
                continue;
            }


            arbol->parent = nullptr;
            arbol->sibling = nullptr;


            if (head == nullptr) {

                head = arbol;
                ultimo = arbol;

            } else {

                ultimo->sibling = arbol;
                ultimo = arbol;
            }
        }


        actualizarMinimum();
    }


    // ========================================================
    // GET NODE
    // ========================================================

    NodoBinomial* getNode(int vertex) {

        if (
            vertex < 0 ||
            vertex >= (int)pos.size()
        ) {
            return nullptr;
        }

        return pos[vertex];
    }


    // ========================================================
    // CONTIENE
    // ========================================================

    bool contiene(int vertex) {

        if (
            vertex < 0 ||
            vertex >= (int)pos.size()
        ) {
            return false;
        }

        return pos[vertex] != nullptr;
    }
};


// ============================================================
// ARISTA DEL GRAFO
// ============================================================

struct Arista {

    int destino;
    double peso;
};


// ============================================================
// ARISTA DEL MST
// ============================================================

struct AristaMST {

    int origen;
    int destino;
    double peso;
};


// ============================================================
// LECTURA DEL GRAFO
// ============================================================

vector<vector<Arista>> leerGrafo(
    const string& nombreArchivo
) {

    ifstream archivo(nombreArchivo);


    if (!archivo.is_open()) {

        cerr << "ERROR: No se pudo abrir el archivo "
             << nombreArchivo
             << endl;

        return {};
    }


    int n;
    int m;

    archivo >> n >> m;


    vector<vector<Arista>> grafo(n);


    for (int i = 0; i < n; i++) {

        int vertice;
        char dosPuntos;

        archivo >> vertice >> dosPuntos;

        string linea;

        getline(archivo, linea);

        stringstream ss(linea);

        int vecino;
        double peso;

        while (ss >> vecino >> peso) {

            Arista arista;

            arista.destino = vecino;
            arista.peso = peso;

            grafo[vertice].push_back(arista);
        }
    }


    archivo.close();


    return grafo;
}


// ============================================================
// PRIM CON COLA BINOMIAL
// ============================================================

vector<AristaMST> primBinomial(
    const vector<vector<Arista>>& grafo,
    int raiz
) {

    int n = grafo.size();


    // --------------------------------------------------------
    // Inicializacion
    // --------------------------------------------------------

    vector<double> costos(
        n,
        numeric_limits<double>::infinity()
    );


    vector<int> parent(
        n,
        -1
    );


    costos[raiz] = 0.0;


    // --------------------------------------------------------
    // Construimos Q
    // --------------------------------------------------------

    ColaBinomial Q(n);

    Q.heapify(costos);


    // --------------------------------------------------------
    // Arbol cobertor minimo
    // --------------------------------------------------------

    vector<AristaMST> MST;

    MST.reserve(n > 0 ? n - 1 : 0);


    // --------------------------------------------------------
    // Mientras Q no este vacia
    // --------------------------------------------------------

    while (Q.findMin() != nullptr) {

        NodoBinomial* nodo =
            Q.extractMin();

        int u = nodo->vertex;


        // ----------------------------------------------------
        // Si u no es la raiz, agregamos la arista
        // correspondiente al MST.
        // ----------------------------------------------------

        if (parent[u] != -1) {

            AristaMST arista;

            arista.origen = parent[u];
            arista.destino = u;
            arista.peso = costos[u];

            MST.push_back(arista);
        }


        // ----------------------------------------------------
        // Revisamos los vecinos de u
        // ----------------------------------------------------

        for (const Arista& arista : grafo[u]) {

            int v = arista.destino;
            double peso = arista.peso;

            if (Q.contiene(v)) {

                if (peso < costos[v]) {

                    costos[v] = peso;

                    parent[v] = u;

                    Q.decreaseKey(
                        v,
                        peso
                    );
                }
            }
        }

        delete nodo;
    }


    return MST;
}


// ============================================================
// MAIN
// ============================================================

int main() {

    int i = 20;
    int j = 23;

    cout << "============================================" << endl;
    cout << "        PRIM CON COLA BINOMIAL              " << endl;
    cout << "        Configuracion: i = " << i
         << ", j = " << j << "                 " << endl;
    cout << "============================================" << endl;
    cout << endl;

    double sumaTiempos = 0.0;

    for (int semilla = 1; semilla <= 10; semilla++) {

        string nombreArchivo =
            "grafo_i" + to_string(i) +
            "_j" + to_string(j) +
            "_seed" + to_string(semilla) +
            ".txt";

        cout << "--------------------------------------------" << endl;
        cout << "Semilla: " << semilla << endl;
        cout << "Archivo: " << nombreArchivo << endl;

        // Leer el grafo correspondiente a esta semilla
        vector<vector<Arista>> grafo = leerGrafo(nombreArchivo);

        if (grafo.empty()) {
            cerr << "ERROR: El grafo esta vacio." << endl;
            continue;
        }

        cout << "Grafo leido correctamente." << endl;
        cout << "Numero de vertices: " << grafo.size() << endl;

        // Contar adyacencias almacenadas
        long long cantidadAdyacencias = 0;

        for (int u = 0; u < (int)grafo.size(); u++) {
            cantidadAdyacencias += grafo[u].size();
        }

        cout << "Numero de adyacencias almacenadas: "
             << cantidadAdyacencias << endl;

        cout << "Ejecutando Prim..." << endl;

        // ==========================================
        // MEDICION DEL TIEMPO
        // ==========================================

        auto inicio = std::chrono::steady_clock::now();

        vector<AristaMST> MST = primBinomial(grafo, 0);

        auto fin = std::chrono::steady_clock::now();

        double tiempoMilisegundos =
            std::chrono::duration<double, std::milli>(
                fin - inicio
            ).count();

        double tiempoSegundos =
            std::chrono::duration<double>(
                fin - inicio
            ).count();

        sumaTiempos += tiempoMilisegundos;

        // ==========================================
        // CALCULAR PESO TOTAL DEL MST
        // ==========================================

        double pesoTotal = 0.0;

        for (const AristaMST& arista : MST) {
            pesoTotal += arista.peso;
        }

        // ==========================================
        // MOSTRAR RESULTADOS
        // ==========================================

        cout << "Prim terminado." << endl;
        cout << "Aristas del MST: " << MST.size() << endl;

        if (MST.size() == grafo.size() - 1) {
            cout << "El MST tiene " << grafo.size() - 1
                 << " aristas. CORRECTO." << endl;
        }
        else {
            cout << "ADVERTENCIA: El MST no tiene "
                 << grafo.size() - 1 << " aristas." << endl;
        }

        cout << "Peso total del MST: "
             << pesoTotal << endl;

        cout << "Tiempo de ejecucion: "
             << tiempoMilisegundos << " ms" << endl;

        cout << "Tiempo de ejecucion: "
             << tiempoSegundos << " s" << endl;

        cout << endl;
    }

    cout << "============================================" << endl;
    cout << "      FINALIZARON LAS 10 EJECUCIONES        " << endl;
    cout << "============================================" << endl;

    double promedioMilisegundos = sumaTiempos / 10.0;
    double promedioSegundos = promedioMilisegundos / 1000.0;

    cout << endl;
    cout << "Promedio de tiempo de ejecucion:" << endl;
    cout << "  " << promedioMilisegundos << " ms" << endl;
    cout << "  " << promedioSegundos << " s" << endl;

    return 0;
}