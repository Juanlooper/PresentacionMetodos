/* LABORATORIO 2 - Lenguaje C (C11).
   Ejemplo principal: acero, plastico y aluminio para vehiculos.
   Compilar: gcc -std=c11 -Wall -Wextra laboratorio.c -lm -o laboratorio
   Cada fila contiene n coeficientes y el termino independiente.
   La memoria se reserva segun las dimensiones introducidas. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

static int n; /* Dimension validada del sistema cuadrado. */
double **crear(int filas, int columnas) {
    double **a = calloc((size_t)filas, sizeof *a);
    if (!a) { fprintf(stderr,"Sin memoria.\n"); exit(EXIT_FAILURE); }
    for (int i = 0; i < filas; ++i) {
        a[i] = calloc((size_t)columnas, sizeof **a);
        if (!a[i]) { fprintf(stderr,"Sin memoria.\n"); exit(EXIT_FAILURE); }
    }
    return a;
}
void liberar(double **a) {
    for (int i = 0; i < n; ++i) free(a[i]);
    free(a);
}
void mostrar(double **a) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (j == n) printf(" | ");
            printf("%15.8f", a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
double residuo(double **a, const double *x) {
    double r = 0;
    for (int i = 0; i < n; ++i) {
        double suma = 0;
        for (int j = 0; j < n; ++j) suma += a[i][j]*x[j];
        r = fmax(r, fabs(suma-a[i][n]));
    }
    return r;
}
/* @result: comprobar la solucion en las ecuaciones originales. */
void resultado(double **original, const double *x) {
    for (int i = 0; i < n; ++i) printf("x%d = %.8f\n", i+1, x[i]);
    printf("Residuo maximo = %.8f\n", residuo(original,x));
}
double umbral(double **a) {
    double escala = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) escala = fmax(escala,fabs(a[i][j]));
    return DBL_EPSILON*escala*n*16;
}
/* jordan=0: Gauss. jordan=1: Gauss-Jordan. No modifica el original. */
void directo(double **original, int jordan) {
    double **a = crear(n,n+1);
    double *x = calloc((size_t)n,sizeof *x);
    if (!x) { fprintf(stderr,"Sin memoria.\n"); exit(EXIT_FAILURE); }
    for (int i = 0; i < n; ++i)
        for (int j = 0; j <= n; ++j) a[i][j] = original[i][j];
    double eps = umbral(a);
    mostrar(a);
    for (int k = 0; k < n; ++k) {
        /* @pivot: elegir el mayor valor absoluto disponible. */
        int p = k;
        for (int i = k+1; i < n; ++i)
            if (fabs(a[i][k]) > fabs(a[p][k])) p = i;
        if (fabs(a[p][k]) <= eps) {
            printf("Sin solucion unica a esta precision.\n");
            liberar(a); free(x); return;
        }
        if (p != k) {
            double *temporal = a[p]; a[p] = a[k]; a[k] = temporal;
            printf("Intercambiar F%d y F%d\n", k+1, p+1);
            mostrar(a);
        }
        /* @normalize: dividir cada elemento de la fila por el pivote. */
        if (jordan) {
            double divisor = a[k][k];
            for (int j = k; j <= n; ++j) a[k][j] /= divisor;
            printf("Normalizar F%d / %.8f\n", k+1, divisor);
            mostrar(a);
        }
        /* @eliminate: anular coeficientes sin cambiar las soluciones. */
        for (int i = jordan ? 0 : k+1; i < n; ++i) {
            if (i == k || a[i][k] == 0) continue;
            double factor = a[i][k]/a[k][k];
            for (int j = k+1; j <= n; ++j) a[i][j] -= factor*a[k][j];
            a[i][k] = 0;
            printf("F%d -= %.8f * F%d\n", i+1, factor, k+1);
            mostrar(a);
        }
    }
    /* @back: sustituir desde la ultima fila o leer la identidad. */
    if (!jordan) {
        for (int i = n-1; i >= 0; --i) {
            double suma = 0;
            for (int j = i+1; j < n; ++j) suma += a[i][j]*x[j];
            x[i] = (a[i][n]-suma)/a[i][i];
            printf("Sustitucion: x%d = %.8f\n", i+1, x[i]);
        }
    } else {
        for (int i = 0; i < n; ++i) x[i] = a[i][n];
    }
    resultado(original,x);
    liberar(a); free(x);
}
void seidel(double **a, double tolerancia, int limite) {
    double *x = calloc((size_t)n,sizeof *x);
    double *anterior = calloc((size_t)n,sizeof *anterior);
    if (!x || !anterior) { fprintf(stderr,"Sin memoria.\n"); exit(EXIT_FAILURE); }
    double eps = umbral(a);
    int dominante = 1;
    for (int i = 0; i < n; ++i) {
        if (fabs(a[i][i]) <= eps) {
            printf("Diagonal nula: reordene las ecuaciones.\n");
            free(x); free(anterior); return;
        }
        double suma = 0;
        for (int j = 0; j < n; ++j) if (j != i) suma += fabs(a[i][j]);
        if (fabs(a[i][i]) <= suma) dominante = 0;
    }
    if (!dominante) printf("Sin dominancia estricta: convergencia no garantizada por este criterio.\n");
    mostrar(a);
    /* @iterate: usar inmediatamente las aproximaciones nuevas. */
    for (int k = 1; k <= limite; ++k) {
        for (int j = 0; j < n; ++j) anterior[j] = x[j];
        for (int i = 0; i < n; ++i) {
            double suma = 0;
            for (int j = 0; j < n; ++j) if (j != i) suma += a[i][j]*x[j];
            x[i] = (a[i][n]-suma)/a[i][i];
            if (!isfinite(x[i])) {
                printf("Valores no finitos: detener.\n");
                free(x); free(anterior); return;
            }
            printf("Iteracion %d, x%d = %.8f\n", k, i+1, x[i]);
        }
        /* @error: cambio relativo aproximado, no error verdadero. */
        double error = 0;
        for (int i = 0; i < n; ++i) {
            double e = x[i] == 0 ? (anterior[i] == 0 ? 0 : INFINITY)
                                  : fabs((x[i]-anterior[i])/x[i])*100;
            error = fmax(error,e);
        }
        printf("Cambio maximo (%%) = %.8f\n", error);
        resultado(a,x);
        if (error <= tolerancia) {
            printf("Tolerancia alcanzada en %d iteraciones.\n", k);
            free(x); free(anterior); return;
        }
    }
    printf("Limite de iteraciones: tolerancia NO alcanzada.\n");
    free(x); free(anterior);
}
/* @input: solicitar dimensiones por separado y comprobar sistema cuadrado. */
int main(void) {
    int ecuaciones, incognitas, metodo;
    printf("Cantidad de ecuaciones y cantidad de incognitas: ");
    if (scanf("%d %d", &ecuaciones, &incognitas) != 2 || ecuaciones <= 0 || ecuaciones != incognitas) {
        fprintf(stderr,"El sistema debe ser cuadrado y de dimension positiva.\n"); return 1;
    }
    n = ecuaciones;
    double **a = crear(ecuaciones,incognitas+1);
    for (int i = 0; i < n; ++i) {
        printf("Fila %d: coeficientes y termino independiente: ", i+1);
        for (int j = 0; j <= n; ++j)
            if (scanf("%lf", &a[i][j]) != 1 || !isfinite(a[i][j])) {
                fprintf(stderr,"Dato invalido.\n"); liberar(a); return 1;
            }
    }
    printf("Metodo: 1 Gauss, 2 Gauss-Jordan, 3 Gauss-Seidel: ");
    if (scanf("%d", &metodo) != 1 || metodo < 1 || metodo > 3) {
        fprintf(stderr,"Metodo invalido.\n"); liberar(a); return 1;
    }
    if (metodo == 3) {
        double tolerancia; int limite;
        printf("Tolerancia porcentual (ej. 5) y limite de iteraciones (ej. 100): ");
        if (scanf("%lf %d", &tolerancia, &limite) != 2 || !isfinite(tolerancia) || tolerancia <= 0 || tolerancia > 100 || limite <= 0) {
            fprintf(stderr,"Tolerancia o limite invalidos.\n"); liberar(a); return 1;
        }
        seidel(a,tolerancia,limite);
    } else directo(a,metodo == 2);
    liberar(a);
    return 0;
}
