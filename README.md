# Laboratorio de métodos numéricos

Web educativa en React y Vite. Incluye Gauss con pivoteo parcial y sustitución regresiva, Gauss-Jordan y Gauss-Seidel, matrices paso a paso y código C11 descargable y documentado.

## Iniciar

```sh
npm install
npm run dev
```

Para producción: `npm run build`. Pruebas numéricas: `npm test`.

## C

```sh
gcc -std=c11 -Wall -Wextra -pedantic public/laboratorio.c -lm -o laboratorio
```

El programa solicita cantidades separadas de ecuaciones e incógnitas. Deben ser iguales y positivas. Cada fila se introduce con sus coeficientes y después el término independiente. La web permite entre 1 y 10 incógnitas para mantener la matriz manejable; C usa matrices reservadas dinámicamente con calloc.

Entrada del caso original, método Seidel:

```text
3 3
0.52 0.20 0.25 4800
0.30 0.50 0.20 5810
0.18 0.30 0.55 5690
3
5 100
```

Para Gauss usa método 1, para Jordan 2; esos métodos no solicitan tolerancia.

## Interpretación

Solución: (172500/43, 308000/43, 220400/43) kg. Con inicio cero, Seidel alcanza cambio relativo máximo ≤ 5 % en cuatro iteraciones: (4037.83014330, 7168.36343168, 5113.96644491). Residuo máximo: 11.83597208 kg. El cambio entre iteraciones no es el error verdadero.

La interfaz ejecuta JavaScript equivalente, no compila C en el navegador. Cada paso referencia los comentarios del archivo C real. El botón de impresión incluye el desarrollo seleccionado y el código; los datos de portada deben completarse. Las fuentes web son opcionales: hay tipografías locales de respaldo.

## Ejecución didáctica sincronizada

La terminal inferior reproduce la salida del algoritmo JavaScript equivalente al C descargable. No es un proceso nativo de C ni una consola de comandos del sistema.

Cada paso conserva una instantánea independiente de la matriz, las incógnitas, las variables locales y la salida producida. El código resalta la instrucción correspondiente y muestra valores junto a ella. En los bucles se avanza celda por celda. La salida se acumula únicamente en las operaciones de impresión y se restaura al retroceder.

La entrada del sistema se configura en el formulario. Los controles de la matriz y de la terminal operan sobre el mismo paso de ejecución.

## Ejemplo principal: talleres y vehículos

Se requieren 4800 kg de acero, 5810 kg de plástico y 5690 kg de aluminio. El caso de talleres sustituye al anterior como ejemplo inicial. El botón «Cargar ejemplo principal» restablece los datos y la tolerancia del 5 %.

El código activo y descargable es `public/laboratorio.c`, compilado con C11. Las 166 capturas de todos los pasos están en `public/capturas/`; `index.html` contiene explicaciones de ejecución y programación y permite imprimir un método o los tres. El código completo también está incluido en esa galería. Son capturas de la ejecución didáctica JavaScript, no de un proceso C nativo.

`node scripts/check-native-c.mjs` compara los resultados del ejecutable C compilado con la web para los tres métodos, y comprueba otro sistema y dimensiones incompatibles.
