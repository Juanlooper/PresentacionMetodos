// LABORATORIO 2 - Sistemas de ecuaciones simultaneas
// Compilar: g++ -std=c++17 laboratorio.cpp -o laboratorio
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;
using Matriz = vector<vector<double>>;

// Imprime cada matriz aumentada despues de una operacion.
void mostrar(const Matriz& a) {
    for (const auto& fila : a) {
        for (size_t j = 0; j < fila.size(); ++j) {
            if (j == fila.size()-1) cout << " | ";
            cout << setw(15) << fila[j];
        }
        cout << '\n';
    }
    cout << '\n';
}
// Residuo infinito: maximo de |Ax-b|, en unidades del problema.
double residuo(const Matriz& a, const vector<double>& x) {
    double r = 0;
    for (size_t i = 0; i < x.size(); ++i) {
        double suma = 0;
        for (size_t j = 0; j < x.size(); ++j) suma += a[i][j]*x[j];
        r = max(r, abs(suma-a[i].back()));
    }
    return r;
}
// @result: imprimir la solucion y verificarla en el sistema original.
void resultado(const Matriz& original, const vector<double>& x) {
    for (size_t i = 0; i < x.size(); ++i)
        cout << "x" << i+1 << " = " << x[i] << '\n';
    cout << "Residuo maximo = " << residuo(original,x) << '\n';
}
// Escala relativa para detectar pivotes numericamente nulos.
double umbral(const Matriz& a) {
    double escala = 0;
    for (const auto& fila : a)
        for (size_t j = 0; j+1 < fila.size(); ++j)
            escala = max(escala,abs(fila[j]));
    return numeric_limits<double>::epsilon()*escala*a.size()*16;
}
// Gauss y Gauss-Jordan comparten el pivoteo y la eliminacion.
void directo(Matriz a, bool jordan) {
    const Matriz original = a;
    int n = a.size();
    double eps = umbral(a);
    mostrar(a);
    for (int k = 0; k < n; ++k) {
        // @pivot: buscar el mayor pivote disponible en la columna.
        int p = k;
        for (int i = k+1; i < n; ++i)
            if (abs(a[i][k]) > abs(a[p][k])) p = i;
        if (abs(a[p][k]) <= eps) {
            cout << "Sin solucion unica a esta precision.\n"; return;
        }
        if (p != k) {
            swap(a[p],a[k]);
            cout << "Intercambiar F" << k+1 << " y F" << p+1 << '\n';
            mostrar(a);
        }
        // @normalize: Gauss-Jordan convierte el pivote en uno.
        if (jordan) {
            double divisor = a[k][k];
            for (int j = k; j <= n; ++j) a[k][j] /= divisor;
            cout << "Normalizar F" << k+1 << " / " << divisor << '\n';
            mostrar(a);
        }
        // @eliminate: Gauss elimina debajo; Jordan arriba y debajo.
        for (int i = jordan ? 0 : k+1; i < n; ++i) {
            if (i == k || a[i][k] == 0) continue;
            double factor = a[i][k]/a[k][k];
            for (int j = k+1; j <= n; ++j) a[i][j] -= factor*a[k][j];
            a[i][k] = 0;
            cout << "F" << i+1 << " -= " << factor << " * F" << k+1 << '\n';
            mostrar(a);
        }
    }
    vector<double> x(n,0);
    // @back: Gauss resuelve desde la ultima fila hasta la primera.
    if (!jordan) {
        for (int i = n-1; i >= 0; --i) {
            double suma = 0;
            for (int j = i+1; j < n; ++j) suma += a[i][j]*x[j];
            x[i] = (a[i][n]-suma)/a[i][i];
            cout << "Sustitucion: x" << i+1 << " = " << x[i] << '\n';
        }
    } else {
        for (int i = 0; i < n; ++i) x[i] = a[i][n];
    }
    resultado(original,x);
}
void seidel(const Matriz& a, double tolerancia, int limite) {
    int n = a.size();
    vector<double> x(n,0); // Aproximacion inicial: todas las variables en cero.
    double eps = umbral(a);
    bool dominante = true;
    for (int i = 0; i < n; ++i) {
        if (abs(a[i][i]) <= eps) {
            cout << "Diagonal nula: reordene las ecuaciones.\n"; return;
        }
        double suma = 0;
        for (int j = 0; j < n; ++j) if (j != i) suma += abs(a[i][j]);
        if (abs(a[i][i]) <= suma) dominante = false;
    }
    if (!dominante) cout << "Sin dominancia estricta: convergencia no garantizada por este criterio.\n";
    mostrar(a);
    for (int k = 1; k <= limite; ++k) {
        vector<double> anterior = x;
        // @iterate: x se actualiza inmediatamente en cada despeje.
        for (int i = 0; i < n; ++i) {
            double suma = 0;
            for (int j = 0; j < n; ++j) if (j != i) suma += a[i][j]*x[j];
            x[i] = (a[i][n]-suma)/a[i][i];
            if (!isfinite(x[i])) {
                cout << "Valores no finitos: detener.\n"; return;
            }
            cout << "Iteracion " << k << ", x" << i+1 << " = " << x[i] << '\n';
        }
        // @error: cambio relativo aproximado, NO error verdadero.
        double error = 0;
        for (int i = 0; i < n; ++i) {
            double e = x[i] == 0 ? (anterior[i] == 0 ? 0 : numeric_limits<double>::infinity())
                                  : abs((x[i]-anterior[i])/x[i])*100;
            error = max(error,e);
        }
        cout << "Cambio maximo (%) = " << error << '\n';
        resultado(a,x);
        if (error <= tolerancia) {
            cout << "Tolerancia alcanzada en " << k << " iteraciones.\n"; return;
        }
    }
    cout << "Limite de iteraciones: tolerancia NO alcanzada.\n";
}
// @input: se solicitan por separado ecuaciones e incognitas.
int main() {
    cout << fixed << setprecision(8);
    int ecuaciones, incognitas, metodo;
    cout << "Cantidad de ecuaciones y cantidad de incognitas: ";
    if (!(cin >> ecuaciones >> incognitas) || ecuaciones <= 0 || ecuaciones != incognitas) {
        cerr << "El sistema debe ser cuadrado y de dimension positiva.\n"; return 1;
    }
    Matriz a(ecuaciones,vector<double>(incognitas+1));
    for (int i = 0; i < ecuaciones; ++i) {
        cout << "Fila " << i+1 << ": coeficientes y termino independiente: ";
        for (double& valor : a[i])
            if (!(cin >> valor) || !isfinite(valor)) { cerr << "Dato invalido.\n"; return 1; }
    }
    cout << "Metodo: 1 Gauss, 2 Gauss-Jordan, 3 Gauss-Seidel: ";
    if (!(cin >> metodo) || metodo < 1 || metodo > 3) { cerr << "Metodo invalido.\n"; return 1; }
    if (metodo == 3) {
        double tolerancia; int limite;
        cout << "Tolerancia porcentual (ej. 5) y limite de iteraciones (ej. 100): ";
        if (!(cin >> tolerancia >> limite) || !isfinite(tolerancia) || tolerancia <= 0 || tolerancia > 100 || limite <= 0) {
            cerr << "Tolerancia o limite invalidos.\n"; return 1;
        }
        seidel(a,tolerancia,limite);
    } else directo(a,metodo == 2);
}

