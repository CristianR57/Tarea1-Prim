#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <chrono>

using namespace std;


// ============================================================
// NODO FIBONACCI
// ============================================================

struct NodoFibonacci {

    int vertex;
    double key;

    NodoFibonacci* parent;
    NodoFibonacci* child;

    NodoFibonacci* left;
    NodoFibonacci* right;

    int degree;
    bool flag;


    NodoFibonacci(int v, double k) {

        vertex = v;
        key = k;

        parent = nullptr;
        child = nullptr;

        left = this;
        right = this;

        degree = 0;
        flag = false;
    }
};


// ============================================================
// TABLA DE POSICIONES
// ============================================================

vector<NodoFibonacci*> pos;


// ============================================================
// COLA DE FIBONACCI
// ============================================================

class ColaFibonacci {

private:

    NodoFibonacci* minimum;
    int cantidad;


    // --------------------------------------------------------
    // BUFFERS REUTILIZABLES
    //
    // consolidar() y extractMin() se llaman n veces dentro de
    // Prim. Antes, cada llamada a consolidar() reservaba un
    // vector<NodoFibonacci*> A(100, nullptr) nuevo (con un
    // grado maximo "adivinado" a mano), y extractMin() reservaba
    // un vector<NodoFibonacci*> hijos nuevo cada vez. Con
    // n = 2^22, eso son millones de reservas/liberaciones de
    // memoria innecesarias en el camino caliente del algoritmo.
    //
    // tablaGrados, raices e hijos son ahora buffers del propio
    // objeto: se reutilizan entre llamadas (solo se limpian,
    // sin liberar memoria), y tablaGrados crece dinamicamente
    // segun el grado maximo real alcanzado, en vez de un tamano
    // fijo arbitrario.
    // --------------------------------------------------------

    vector<NodoFibonacci*> tablaGrados;
    vector<NodoFibonacci*> raices;
    vector<NodoFibonacci*> hijos;


    // ========================================================
    // AGREGAR RAIZ
    // ========================================================

    void agregarRaiz(NodoFibonacci* nodo) {

        nodo->parent = nullptr;
        nodo->flag = false;


        if (minimum == nullptr) {

            nodo->left = nodo;
            nodo->right = nodo;

            minimum = nodo;

            return;
        }


        nodo->right = minimum->right;
        nodo->left = minimum;

        minimum->right->left = nodo;
        minimum->right = nodo;


        if (nodo->key < minimum->key) {

            minimum = nodo;
        }
    }


public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    ColaFibonacci() {

        minimum = nullptr;
        cantidad = 0;
    }


    // ========================================================
    // INSERT
    // ========================================================

    void insert(int vertex, double key) {

        NodoFibonacci* nuevo =
            new NodoFibonacci(vertex, key);


        agregarRaiz(nuevo);

        cantidad++;


        if (vertex >= (int)pos.size()) {

            pos.resize(vertex + 1, nullptr);
        }


        pos[vertex] = nuevo;
    }


    // ========================================================
    // LINK
    // ========================================================

    NodoFibonacci* link(
        NodoFibonacci* a,
        NodoFibonacci* b
    ) {

        if (b->key < a->key) {

            NodoFibonacci* temp = a;

            a = b;
            b = temp;
        }


        // ----------------------------------------------------
        // Sacamos b de la lista de raices
        // ----------------------------------------------------

        b->left->right = b->right;
        b->right->left = b->left;

        b->left = b;
        b->right = b;


        // ----------------------------------------------------
        // b pasa a ser hijo de a
        // ----------------------------------------------------

        b->parent = a;
        b->flag = false;


        // ----------------------------------------------------
        // Agregamos b a los hijos de a
        // ----------------------------------------------------

        if (a->child == nullptr) {

            a->child = b;
        }

        else {

            b->right = a->child->right;
            b->left = a->child;

            a->child->right->left = b;
            a->child->right = b;
        }


        a->degree++;


        return a;
    }


    // ========================================================
    // FIND MIN
    // ========================================================

    NodoFibonacci* findMin() const {

        return minimum;
    }


    // ========================================================
    // ACTUALIZAR MINIMUM
    // ========================================================

    void actualizarMinimum() {

        if (minimum == nullptr) {

            return;
        }


        NodoFibonacci* actual = minimum;
        NodoFibonacci* mejor = minimum;


        do {

            if (actual->key < mejor->key) {

                mejor = actual;
            }

            actual = actual->right;

        } while (actual != minimum);


        minimum = mejor;
    }


    // ========================================================
    // MERGE
    // ========================================================

    void merge(ColaFibonacci& otra) {

        if (otra.minimum == nullptr) {

            return;
        }


        if (minimum == nullptr) {

            minimum = otra.minimum;
            cantidad = otra.cantidad;

            otra.minimum = nullptr;
            otra.cantidad = 0;

            return;
        }


        NodoFibonacci* a = minimum;
        NodoFibonacci* b = minimum->right;

        NodoFibonacci* c = otra.minimum;
        NodoFibonacci* d = otra.minimum->right;


        a->right = c;
        c->left = a;

        d->right = b;
        b->left = d;


        if (otra.minimum->key < minimum->key) {

            minimum = otra.minimum;
        }


        cantidad += otra.cantidad;


        otra.minimum = nullptr;
        otra.cantidad = 0;
    }


    // ========================================================
    // CONSOLIDAR
    // ========================================================

    void consolidar() {

        if (minimum == nullptr) {

            return;
        }


        // ----------------------------------------------------
        // Recolectamos las raices actuales (buffer reutilizado)
        // ----------------------------------------------------

        raices.clear();

        NodoFibonacci* actual = minimum;


        do {

            raices.push_back(actual);

            actual = actual->right;

        } while (actual != minimum);


        // ----------------------------------------------------
        // Consolidamos arboles del mismo grado.
        //
        // tablaGrados crece dinamicamente segun el grado que
        // realmente aparece (nunca mas de O(log n)) y se deja
        // en nullptr al terminar, lista para la proxima llamada.
        // ----------------------------------------------------

        for (NodoFibonacci* x : raices) {

            int d = x->degree;

            while ((int)tablaGrados.size() <= d) {
                tablaGrados.push_back(nullptr);
            }


            while (tablaGrados[d] != nullptr) {

                NodoFibonacci* y = tablaGrados[d];

                tablaGrados[d] = nullptr;


                x = link(x, y);

                d = x->degree;


                while ((int)tablaGrados.size() <= d) {
                    tablaGrados.push_back(nullptr);
                }
            }


            tablaGrados[d] = x;
        }


        // ----------------------------------------------------
        // Reconstruimos la lista de raices y dejamos
        // tablaGrados en nullptr para la proxima consolidacion.
        // ----------------------------------------------------

        minimum = nullptr;


        for (NodoFibonacci*& x : tablaGrados) {

            if (x == nullptr) {

                continue;
            }


            x->left = x;
            x->right = x;


            agregarRaiz(x);

            x = nullptr;
        }
    }


    // ========================================================
    // EXTRACT MIN
    // ========================================================

    NodoFibonacci* extractMin() {

        if (minimum == nullptr) {

            return nullptr;
        }


        NodoFibonacci* z = minimum;


        // ----------------------------------------------------
        // Los hijos de z pasan a ser raices (buffer reutilizado)
        // ----------------------------------------------------

        if (z->child != nullptr) {

            NodoFibonacci* hijoInicial =
                z->child;

            NodoFibonacci* hijo =
                hijoInicial;


            hijos.clear();


            do {

                hijos.push_back(hijo);

                hijo = hijo->right;

            } while (hijo != hijoInicial);


            for (NodoFibonacci* x : hijos) {

                x->parent = nullptr;
                x->flag = false;

                x->left = x;
                x->right = x;

                agregarRaiz(x);
            }


            z->child = nullptr;
        }


        // ----------------------------------------------------
        // Eliminamos z de la lista de raices
        // ----------------------------------------------------

        if (z->right == z) {

            minimum = nullptr;
        }

        else {

            NodoFibonacci* siguiente =
                z->right;


            z->left->right = z->right;
            z->right->left = z->left;


            minimum = siguiente;
        }


        z->left = z;
        z->right = z;
        z->parent = nullptr;


        cantidad--;


        // ----------------------------------------------------
        // El vertice ya no pertenece a Q
        // ----------------------------------------------------

        if (z->vertex < (int)pos.size()) {

            pos[z->vertex] = nullptr;
        }


        // ----------------------------------------------------
        // Consolidamos
        // ----------------------------------------------------

        if (minimum != nullptr) {

            consolidar();
        }


        return z;
    }


    // ========================================================
    // CUT
    // ========================================================

    void cut(
        NodoFibonacci* x,
        NodoFibonacci* y
    ) {

        if (x == nullptr || y == nullptr) {

            return;
        }


        if (x->right == x) {

            y->child = nullptr;
        }

        else {

            x->left->right = x->right;
            x->right->left = x->left;


            if (y->child == x) {

                y->child = x->right;
            }
        }


        y->degree--;


        x->parent = nullptr;
        x->flag = false;


        x->left = x;
        x->right = x;


        agregarRaiz(x);
    }


    // ========================================================
    // CASCADING CUT
    //
    // El pseudocodigo es recursivo (cascadingCut se llama a si
    // misma sobre el padre), pero esa recursion es de cola: en
    // cada paso solo queda pendiente "seguir subiendo con z".
    // Se implementa aqui como un bucle iterativo, que hace
    // exactamente lo mismo pero sin apilar un frame de funcion
    // por cada corte en cascada, lo cual importa cuando
    // decreaseKey se llama del orden de 2^24 veces.
    // ========================================================

    void cascadingCut(
        NodoFibonacci* y
    ) {

        while (y != nullptr) {

            NodoFibonacci* z =
                y->parent;


            if (z == nullptr) {

                return;
            }


            if (y->flag == false) {

                y->flag = true;

                return;
            }


            cut(y, z);


            y = z;
        }
    }


    // ========================================================
    // DECREASE KEY
    // ========================================================

    void decreaseKey(
        int vertex,
        double nuevaKey
    ) {

        if (
            vertex < 0 ||
            vertex >= (int)pos.size()
        ) {

            cout << "Error: vertice no existe."
                 << endl;

            return;
        }


        NodoFibonacci* x =
            pos[vertex];


        if (x == nullptr) {

            cout << "Error: vertice no esta "
                 << "en la cola."
                 << endl;

            return;
        }


        if (nuevaKey > x->key) {

            cout << "Error: decreaseKey no puede "
                 << "aumentar la key."
                 << endl;

            return;
        }


        // ----------------------------------------------------
        // x.key = nuevaKey
        // ----------------------------------------------------

        x->key = nuevaKey;


        // ----------------------------------------------------
        // Si x es una raiz
        // ----------------------------------------------------

        if (x->parent == nullptr) {

            if (x->key < minimum->key) {

                minimum = x;
            }

            return;
        }


        // ----------------------------------------------------
        // Revisamos al padre
        // ----------------------------------------------------

        NodoFibonacci* y =
            x->parent;


        // ----------------------------------------------------
        // Si no se viola la propiedad del heap
        // ----------------------------------------------------

        if (x->key >= y->key) {

            if (x->key < minimum->key) {

                minimum = x;
            }

            return;
        }


        // ----------------------------------------------------
        // Se viola la propiedad: cut + cascadingCut
        // ----------------------------------------------------

        cut(x, y);

        cascadingCut(y);


        // ----------------------------------------------------
        // Actualizamos el minimo
        // ----------------------------------------------------

        if (x->key < minimum->key) {

            minimum = x;
        }
    }


    // ========================================================
    // HEAPIFY
    // ========================================================

    void heapify(
        const vector<int>& vertices,
        const vector<double>& keys
    ) {

        if (
            vertices.size() !=
            keys.size()
        ) {

            cout << "Error: vertices y keys "
                 << "tienen distinto tamano."
                 << endl;

            return;
        }


        // ----------------------------------------------------
        // Reservamos pos de una sola vez segun el mayor
        // vertice, en vez de dejar que cada insert() la vaya
        // agrandando de a uno. Con n = 2^22 vertices esto
        // evita realocaciones repetidas del vector global.
        // ----------------------------------------------------

        int maxVertice = -1;

        for (int v : vertices) {

            if (v > maxVertice) {
                maxVertice = v;
            }
        }

        if (maxVertice + 1 > (int)pos.size()) {

            pos.resize(maxVertice + 1, nullptr);
        }


        for (
            int i = 0;
            i < (int)vertices.size();
            i++
        ) {

            insert(
                vertices[i],
                keys[i]
            );
        }
    }


    // ========================================================
    // TAMANO
    // ========================================================

    int size() const {

        return cantidad;
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

    ifstream archivo(
        nombreArchivo
    );


    if (!archivo.is_open()) {

        cerr << "ERROR: No se pudo abrir el archivo "
             << nombreArchivo
             << endl;

        return {};
    }


    int n;
    int m;


    // --------------------------------------------------------
    // Primera linea:
    //
    // 16 32
    // --------------------------------------------------------

    archivo >> n >> m;


    vector<vector<Arista>> grafo(n);


    // --------------------------------------------------------
    // Leemos cada vertice
    // --------------------------------------------------------

    for (
        int i = 0;
        i < n;
        i++
    ) {

        int vertice;
        char dosPuntos;


        archivo >> vertice >> dosPuntos;


        // ----------------------------------------------------
        // Leemos el resto de la linea
        // ----------------------------------------------------

        string linea;

        getline(
            archivo,
            linea
        );


        stringstream ss(linea);


        int vecino;
        double peso;


        while (
            ss >> vecino >> peso
        ) {

            Arista arista;

            arista.destino = vecino;
            arista.peso = peso;


            grafo[vertice].push_back(
                arista
            );
        }
    }


    archivo.close();


    return grafo;
}


// ============================================================
// PRIM CON COLA DE FIBONACCI
// ============================================================

vector<AristaMST> primFibonacci(
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
    // Vertices
    // --------------------------------------------------------

    vector<int> vertices(n);


    for (
        int i = 0;
        i < n;
        i++
    ) {

        vertices[i] = i;
    }


    // --------------------------------------------------------
    // Construimos Q
    // --------------------------------------------------------

    ColaFibonacci Q;


    Q.heapify(
        vertices,
        costos
    );


    // --------------------------------------------------------
    // Arbol cobertor minimo
    // --------------------------------------------------------

    vector<AristaMST> MST;

    MST.reserve(n > 0 ? n - 1 : 0);


    // --------------------------------------------------------
    // Mientras Q no este vacia
    // --------------------------------------------------------

    while (
        Q.findMin() != nullptr
    ) {

        // ----------------------------------------------------
        // Extract Min
        // ----------------------------------------------------

        NodoFibonacci* nodo =
            Q.extractMin();


        int u =
            nodo->vertex;


        // ----------------------------------------------------
        // Si u no es la raiz, agregamos la arista
        // correspondiente al MST
        // ----------------------------------------------------

        if (
            parent[u] != -1
        ) {

            AristaMST arista;

            arista.origen =
                parent[u];

            arista.destino =
                u;

            arista.peso =
                costos[u];


            MST.push_back(
                arista
            );
        }


        // ----------------------------------------------------
        // Revisamos los vecinos de u
        // ----------------------------------------------------

        for (
            const Arista& arista
            : grafo[u]
        ) {

            int v =
                arista.destino;

            double peso =
                arista.peso;


            // ------------------------------------------------
            // Si v todavia esta en Q
            //
            // pos[v] != nullptr significa que v sigue
            // dentro de la cola.
            // ------------------------------------------------

            if (
                v >= 0 &&
                v < (int)pos.size() &&
                pos[v] != nullptr
            ) {

                // --------------------------------------------
                // Encontramos una mejor conexion
                // --------------------------------------------

                if (
                    peso < costos[v]
                ) {

                    costos[v] =
                        peso;

                    parent[v] =
                        u;


                    // ----------------------------------------
                    // Decrease Key
                    // ----------------------------------------

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
    int j = 22;

    cout << "============================================" << endl;
    cout << "        PRIM CON COLA DE FIBONACCI          " << endl;
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

        vector<AristaMST> MST = primFibonacci(grafo, 0);

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