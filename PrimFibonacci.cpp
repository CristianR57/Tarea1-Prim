#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>

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


        int maxDegree = 100;


        vector<NodoFibonacci*> A(
            maxDegree,
            nullptr
        );


        vector<NodoFibonacci*> raices;


        NodoFibonacci* actual = minimum;


        do {

            raices.push_back(actual);

            actual = actual->right;

        } while (actual != minimum);


        // ----------------------------------------------------
        // Consolidamos arboles del mismo grado
        // ----------------------------------------------------

        for (NodoFibonacci* x : raices) {

            int d = x->degree;


            while (A[d] != nullptr) {

                NodoFibonacci* y = A[d];

                A[d] = nullptr;


                x = link(x, y);

                d = x->degree;
            }


            A[d] = x;
        }


        // ----------------------------------------------------
        // Reconstruimos la lista de raices
        // ----------------------------------------------------

        minimum = nullptr;


        for (NodoFibonacci* x : A) {

            if (x == nullptr) {

                continue;
            }


            x->left = x;
            x->right = x;


            agregarRaiz(x);
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
        // Los hijos de z pasan a ser raices
        // ----------------------------------------------------

        if (z->child != nullptr) {

            NodoFibonacci* hijoInicial =
                z->child;

            NodoFibonacci* hijo =
                hijoInicial;


            vector<NodoFibonacci*> hijos;


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
    // ========================================================

    void cascadingCut(
        NodoFibonacci* y
    ) {

        if (y == nullptr) {

            return;
        }


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

        cascadingCut(z);
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
        // Se viola la propiedad
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
// PASO 12: PRIM CON COLA DE FIBONACCI
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

    cout << endl;

    cout << "===================================================="
         << endl;

    cout << "          PRIM CON COLA DE FIBONACCI"
         << endl;

    cout << "===================================================="
         << endl;


    // --------------------------------------------------------
    // Archivo
    // --------------------------------------------------------

    string nombreArchivo =
        "grafo_i4_j5.txt";


    cout << endl;

    cout << "Leyendo: "
         << nombreArchivo
         << endl;


    // --------------------------------------------------------
    // Leemos el grafo
    // --------------------------------------------------------

    vector<vector<Arista>> grafo =
        leerGrafo(
            nombreArchivo
        );


    // --------------------------------------------------------
    // Verificamos
    // --------------------------------------------------------

    if (
        grafo.empty()
    ) {

        cerr << "ERROR: El grafo esta vacio."
             << endl;

        return 1;
    }


    cout << "Grafo leido correctamente."
         << endl;


    cout << "Numero de vertices: "
         << grafo.size()
         << endl;


    // --------------------------------------------------------
    // Contamos adyacencias
    // --------------------------------------------------------

    int cantidadAdyacencias =
        0;


    for (
        int u = 0;
        u < (int)grafo.size();
        u++
    ) {

        cantidadAdyacencias +=
            grafo[u].size();
    }


    cout << "Numero de adyacencias almacenadas: "
         << cantidadAdyacencias
         << endl;


    // --------------------------------------------------------
    // Ejecutamos Prim
    // --------------------------------------------------------

    cout << endl;

    cout << "Ejecutando Prim con Cola de Fibonacci..."
         << endl;


    vector<AristaMST> MST =
        primFibonacci(
            grafo,
            0
        );


    // --------------------------------------------------------
    // Resultado
    // --------------------------------------------------------

    cout << endl;

    cout << "Numero de aristas del MST: "
         << MST.size()
         << endl;


    // --------------------------------------------------------
    // TEST
    // --------------------------------------------------------

    if (
        MST.size() ==
        grafo.size() - 1
    ) {

        cout << "Prim Fibonacci: OK"
             << endl;

    }

    else {

        cout << "Prim Fibonacci: ERROR"
             << endl;
    }


    // --------------------------------------------------------
    // Mostramos MST
    // --------------------------------------------------------

    cout << endl;

    cout << "Aristas del MST:"
         << endl;


    double pesoTotal =
        0.0;


    for (
        const AristaMST& arista
        : MST
    ) {

        cout << arista.origen
             << " -- "
             << arista.destino
             << "  peso = "
             << arista.peso
             << endl;


        pesoTotal +=
            arista.peso;
    }


    cout << endl;

    cout << "Peso total del MST: "
         << pesoTotal
         << endl;


    cout << endl;

    cout << "===================================================="
         << endl;


    return 0;
}