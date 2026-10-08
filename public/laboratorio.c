/* =====================================================================
LABORATORIO 2 | SISTEMAS DE ECUACIONES | C11
Gauss          : crear ceros debajo del pivote y sustituir hacia atras.
Gauss-Jordan   : crear un 1 en el pivote y ceros arriba y abajo.
Gauss-Seidel   : despejar cada x usando los valores mas recientes.

COMPILAR: gcc -std=c11 -Wall -Wextra -pedantic laboratorio.c -lm -o laboratorio
EJECUTAR EJEMPLO: laboratorio --ejemplo (en PowerShell: .\laboratorio.exe --ejemplo)

DICCIONARIO PARA EXPLICAR
n       = cantidad de ecuaciones e incognitas.
a       = matriz aumentada: coeficientes y ultima columna b.
x       = soluciones; anterior = valores de la iteracion anterior.
i, j    = fila y columna. En C los indices comienzan en cero.
k       = columna pivote en Gauss/Jordan; iteracion en Seidel.
p       = fila elegida como pivote.
factor  = multiplo que se resta para convertir un coeficiente en cero.
suma    = acumulacion de los terminos conocidos.

Ejemplo: 4800 kg de acero, 5810 kg de plastico, 5690 kg de aluminio.
x1, x2, x3 son los kg tomados de cada taller.
Se reserva memoria segun n; no hay un tamano de matriz fijo.
El cambio relativo de Seidel NO es el error verdadero.
===================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <limits.h>

static int n; /* Dimension validada del sistema cuadrado. */
/* 1. APOYO: memoria, presentacion y verificacion. */
/* double ** es una lista de filas; cada fila guarda sus numeros.
   calloc reserva espacio y lo inicia en cero. sizeof mide cada elemento.
   size_t es el tipo usado para expresar un tamano de memoria. */
double **crear(int filas, int columnas) {
    double **a = calloc((size_t)filas, sizeof *a);
    if (!a) {
        fprintf(stderr,"Sin memoria.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < filas; ++i) {
        a[i] = calloc((size_t)columnas, sizeof **a);
        if (!a[i]) {
            for (int j = 0; j < i; ++j) {
                free(a[j]);
            }
            free(a);
            fprintf(stderr,"Sin memoria.\n");
            exit(EXIT_FAILURE);
        }
    }
    return a;
}
void liberar(double **a) {
    /* Primero liberar las filas; despues, la lista que las contiene. */
    for (int i = 0; i < n; ++i) {
        free(a[i]);
    }
    free(a);
}
void mostrar(double **a) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (j == n) {
                printf(" | ");
            }
            printf("%15.8f", a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
double residuo(double **a, const double *x) {
    /* Para cada ecuacion calculamos |suma de a*x - b| y tomamos el mayor. */
    double r = 0;
    for (int i = 0; i < n; ++i) {
        double suma = 0;
        for (int j = 0; j < n; ++j) {
            suma = suma + a[i][j]*x[j];
        }
        r = fmax(r, fabs(suma-a[i][n]));
    }
    return r;
}
/* @result: comprobar la solucion en las ecuaciones originales. */
void resultado(double **original, const double *x) {
    printf("\n--- RESULTADO Y VERIFICACION ---\n");
    for (int i = 0; i < n; ++i) {
        printf("x%d = %.8f\n", i+1, x[i]);
    }
    printf("Residuo maximo = %.8f\n", residuo(original,x));
}
/* Umbral proporcional a la escala: evita dividir por pivotes casi nulos. */
double umbral(double **a) {
    /* DBL_EPSILON describe la precision de double; fabs toma valor absoluto.
       fmax elige el mayor de dos numeros. El 16 deja margen al redondeo.
       Este umbral numerico es distinto de la tolerancia porcentual de Seidel. */
    double escala = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            escala = fmax(escala,fabs(a[i][j]));
        }
    }
    return DBL_EPSILON*escala*n*16;
}
/* 2. GAUSS: bajar haciendo ceros; subir calculando las incognitas. */
void gauss(double **original) {
    double **a = crear(n,n+1);
    double *x = calloc((size_t)n,sizeof *x);
    if (!x) {
        liberar(a);
        fprintf(stderr,"Sin memoria.\n");
        exit(EXIT_FAILURE);
    }
    /* Copiar permite verificar luego con las ecuaciones originales. */
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            a[i][j] = original[i][j];
        }
    }
    double eps = umbral(a);
    mostrar(a);
    for (int k = 0; k < n; ++k) {
        /* @pivot: elegir el mayor valor absoluto disponible. */
        int p = k;
        for (int i = k+1; i < n; ++i) {
            if (fabs(a[i][k]) > fabs(a[p][k])) {
                p = i;
            }
        }
        if (fabs(a[p][k]) <= eps) {
            printf("Sin solucion unica a esta precision.\n");
            liberar(a);
            free(x);
            return;
        }
        if (p != k) {
            double *temporal = a[p];
            a[p] = a[k];
            a[k] = temporal;
            printf("Intercambiar F%d y F%d\n", k+1, p+1);
            mostrar(a);
        }
        /* @eliminate: anular coeficientes sin cambiar las soluciones. */
        for (int i = k+1; i < n; ++i) {
            if (a[i][k] == 0) {
                continue;
            }
            double factor = a[i][k]/a[k][k];
            for (int j = k+1; j <= n; ++j) {
                a[i][j] = a[i][j] - factor*a[k][j];
            }
            a[i][k] = 0;
            printf("F%d -= %.8f * F%d\n", i+1, factor, k+1);
            mostrar(a);
        }
    }
    /* @back: resolver desde la ultima ecuacion hacia la primera. */
    for (int i = n-1; i >= 0; --i) {
        double suma = 0;
        for (int j = i+1; j < n; ++j) {
            suma = suma + a[i][j]*x[j];
        }
        x[i] = (a[i][n]-suma)/a[i][i];
        printf("Sustitucion: x%d = %.8f\n", i+1, x[i]);
    }
    resultado(original,x);
    liberar(a);
    free(x);
}

/* 3. GAUSS-JORDAN: convertir los coeficientes en la identidad. */
void gauss_jordan(double **original) {
    double **a = crear(n,n+1);
    double *x = calloc((size_t)n,sizeof *x);
    if (!x) {
        liberar(a);
        fprintf(stderr,"Sin memoria.\n");
        exit(EXIT_FAILURE);
    }
    /* Copiar permite verificar luego con las ecuaciones originales. */
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            a[i][j] = original[i][j];
        }
    }
    double eps = umbral(a);
    mostrar(a);
    for (int k = 0; k < n; ++k) {
        /* @pivot: elegir el mayor valor absoluto disponible. */
        int p = k;
        for (int i = k+1; i < n; ++i) {
            if (fabs(a[i][k]) > fabs(a[p][k])) {
                p = i;
            }
        }
        if (fabs(a[p][k]) <= eps) {
            printf("Sin solucion unica a esta precision.\n");
            liberar(a);
            free(x);
            return;
        }
        if (p != k) {
            double *temporal = a[p];
            a[p] = a[k];
            a[k] = temporal;
            printf("Intercambiar F%d y F%d\n", k+1, p+1);
            mostrar(a);
        }
        /* @normalize: dividir cada elemento de la fila por el pivote. */
        double divisor = a[k][k];
        for (int j = k; j <= n; ++j) {
            a[k][j] = a[k][j] / divisor;
        }
        printf("Normalizar F%d / %.8f\n", k+1, divisor);
        mostrar(a);
        /* @eliminate: anular coeficientes sin cambiar las soluciones. */
        for (int i = 0; i < n; ++i) {
            if (i == k || a[i][k] == 0) {
                continue;
            }
            double factor = a[i][k]/a[k][k];
            for (int j = k+1; j <= n; ++j) {
                a[i][j] = a[i][j] - factor*a[k][j];
            }
            a[i][k] = 0;
            printf("F%d -= %.8f * F%d\n", i+1, factor, k+1);
            mostrar(a);
        }
    }
    /* @back: la ultima columna ya contiene la solucion. */
    for (int i = 0; i < n; ++i) {
        x[i] = a[i][n];
    }
    resultado(original,x);
    liberar(a);
    free(x);
}

/* 4. GAUSS-SEIDEL: actualizar, medir el cambio y repetir. */
void seidel(double **a, double tolerancia, int limite) {
    double *x = calloc((size_t)n,sizeof *x);
    double *anterior = calloc((size_t)n,sizeof *anterior);
    if (!x || !anterior) {
        free(x);
        free(anterior);
        fprintf(stderr,"Sin memoria.\n");
        exit(EXIT_FAILURE);
    }
    double eps = umbral(a);
    int dominante = 1;
    for (int i = 0; i < n; ++i) {
        if (fabs(a[i][i]) <= eps) {
            printf("Diagonal nula: reordene las ecuaciones.\n");
            free(x);
            free(anterior);
            return;
        }
        double suma = 0;
        for (int j = 0; j < n; ++j) {
            if (j != i) {
                suma = suma + fabs(a[i][j]);
            }
        }
        if (fabs(a[i][i]) <= suma) {
            dominante = 0;
        }
    }
    if (!dominante) {
        printf("Sin dominancia estricta: convergencia no garantizada por este criterio.\n");
    }
    mostrar(a);
    /* @iterate: usar inmediatamente las aproximaciones nuevas. */
    for (int k = 1; k <= limite; ++k) {
        for (int j = 0; j < n; ++j) {
            anterior[j] = x[j];
        }
        for (int i = 0; i < n; ++i) {
            double suma = 0;
            for (int j = 0; j < n; ++j) {
                if (j != i) {
                    suma = suma + a[i][j]*x[j];
                }
            }
            x[i] = (a[i][n]-suma)/a[i][i];
            if (!isfinite(x[i])) {
                printf("Valores no finitos: detener.\n");
                free(x);
                free(anterior);
                return;
            }
            printf("Iteracion %d, x%d = %.8f\n", k, i+1, x[i]);
        }
        /* @error: cambio relativo aproximado, no error verdadero. */
        double error = 0;
        for (int i = 0; i < n; ++i) {
            double e;
            if (x[i] != 0) {
                e = fabs((x[i]-anterior[i])/x[i])*100;
            } else if (anterior[i] == 0) {
                e = 0; /* Cero antes y ahora: no cambio. */
            } else {
                e = INFINITY; /* No podemos dividir por cero. */
            }
            if (e > error) {
                error = e;
            }
        }
        printf("Cambio maximo (%%) = %.8f\n", error);
        resultado(a,x);
        if (error <= tolerancia) {
            printf("Tolerancia alcanzada en %d iteraciones.\n", k);
            free(x);
            free(anterior);
            return;
        }
    }
    printf("Limite de iteraciones: tolerancia NO alcanzada.\n");
    free(x);
    free(anterior);
}
/* @input: solicitar dimensiones por separado y comprobar sistema cuadrado. */
int main(int argc, char *argv[]) {
    int ecuaciones, incognitas, metodo;
    printf("\n============================================================\n");
    printf(" LABORATORIO 2 | GAUSS - GAUSS-JORDAN - GAUSS-SEIDEL\n");
    printf("============================================================\n");
    if (argc == 2 && strcmp(argv[1], "--ejemplo") == 0) {
        n = 3;
        double datos[3][4] = {
            {0.52, 0.20, 0.25, 4800},
            {0.30, 0.50, 0.20, 5810},
            {0.18, 0.30, 0.55, 5690}
        };
        double **ejemplo = crear(n,n+1);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j <= n; ++j) {
                ejemplo[i][j] = datos[i][j];
            }
        }
        printf("Ejemplo: acero, plastico y aluminio. Resultados en kg.\n");
        printf("\n--- 1. GAUSS ---\n");
        gauss(ejemplo);
        printf("\n--- 2. GAUSS-JORDAN ---\n");
        gauss_jordan(ejemplo);
        printf("\n--- 3. GAUSS-SEIDEL | tolerancia 5 %% ---\n");
        seidel(ejemplo,5,100);
        liberar(ejemplo);
        return 0;
    }
    if (argc != 1) {
        fprintf(stderr,"Uso: laboratorio [--ejemplo]\n");
        return 1;
    }
    printf("Modo manual: use punto decimal (por ejemplo, 0.52).\n");
    printf("Cantidad de ecuaciones y cantidad de incognitas: ");
    if (scanf("%d %d", &ecuaciones, &incognitas) != 2 || ecuaciones <= 0 || ecuaciones == INT_MAX || ecuaciones != incognitas) {
        fprintf(stderr,"El sistema debe ser cuadrado y de dimension positiva.\n");
        return 1;
    }
    n = ecuaciones;
    double **a = crear(ecuaciones,incognitas+1);
    for (int i = 0; i < n; ++i) {
        printf("Fila %d: coeficientes y termino independiente: ", i+1);
        for (int j = 0; j <= n; ++j) {
            if (scanf("%lf", &a[i][j]) != 1 || !isfinite(a[i][j])) {
                fprintf(stderr,"Dato invalido.\n");
                liberar(a);
                return 1;
            }
        }
    }
    printf("Metodo: 1 Gauss, 2 Gauss-Jordan, 3 Gauss-Seidel: ");
    if (scanf("%d", &metodo) != 1 || metodo < 1 || metodo > 3) {
        fprintf(stderr,"Metodo invalido.\n");
        liberar(a);
        return 1;
    }
    if (metodo == 3) {
        double tolerancia;
        int limite;
        printf("Tolerancia porcentual (ej. 5) y limite de iteraciones (ej. 100): ");
        if (scanf("%lf %d", &tolerancia, &limite) != 2 || !isfinite(tolerancia)
            || tolerancia <= 0 || tolerancia > 100 || limite <= 0 || limite == INT_MAX) {
            fprintf(stderr,"Tolerancia o limite invalidos.\n");
            liberar(a);
            return 1;
        }
        seidel(a,tolerancia,limite);
    } else if (metodo == 1) {
        gauss(a);
    } else {
        gauss_jordan(a);
    }
    liberar(a);
    return 0;
}
