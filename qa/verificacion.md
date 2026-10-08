# Verificación

- 11 pruebas numéricas aprobadas: solución original con ambos métodos directos, pivoteo, tamaños 1/2/6, matrices singulares, límite de iteraciones, diagonal nula, solución cero y entradas inválidas.
- C++17 compilado con g++ y avisos activados. Los tres métodos se ejecutaron con el sistema original; salidas en gauss.txt, jordan.txt y seidel.txt.
- Navegador: Gauss-Jordan y Seidel producen los resultados esperados. Un sistema identidad 2×2 devuelve (7,9). Se rechazan cantidades incompatibles de ecuaciones e incógnitas.
- Reproducción automática avanza; pausa y avance manual funcionan. El fragmento C++ cambia con el paso.
- Vista móvil de 390 px sin desbordamiento general tras ajustar la decoración. Matrices y código tienen desplazamiento propio.
- Consola del navegador sin errores durante la revisión.
- Captura de escritorio: laboratorio.png.
- La opción de impresión prepara contenido, matrices y fuente; no se generó ni revisó un PDF final. Completar los datos personales de portada antes de entregar.

## Actualización: terminal y seguimiento de variables

- 14 pruebas numéricas y de trazas aprobadas.
- Valores de factor verificados antes de modificar la matriz; cada paso de eliminación modifica una sola celda.
- Las instrucciones de todas las trazas corresponden a texto real del C++.
- Navegador: terminal, código y matriz sincronizados al avanzar y retroceder. La salida final desaparece correctamente al volver al paso anterior.
- Gauss-Jordan muestra divisor y la celda normalizada; Seidel muestra error 3.44485075 %, tolerancia 5 y salida de cuatro iteraciones.
- Código completo desplaza su propio panel a la línea activa. Vista móvil de 390 px sin desbordamiento general.
- Sin errores de consola durante la revisión.
- La terminal es didáctica: ejecuta la implementación JavaScript equivalente, no un binario C++ dentro del navegador.

## Actualización del ejemplo principal y lenguaje C

- Caso principal: ingeniero mecánico, materiales para vehículos, tres talleres; unidades kg, tolerancia 5 %.
- Fuente C11 compilada con gcc -Wall -Wextra -pedantic sin avisos. Resultados nativos y web coinciden para los tres métodos (tolerancia de comparación 1e-7).
- C nativo también probado con un sistema 2x2 que requiere pivoteo y una entrada rectangular inválida.
- 14 pruebas numéricas y de correspondencia entre trazas y C aprobadas.
- 166 capturas reales del navegador: 38 Gauss, 57 Jordan, 71 Seidel. Galería con explicación de ejecución, programación y variables de cada pantalla.
