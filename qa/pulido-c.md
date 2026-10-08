# Código C y descargables: revisión del 8 de octubre de 2026

Base: PresentacionMetodos, master, b97cc2a. Cambios locales sobre la versión del repositorio elegido; se conserva el problema de acero, plástico y aluminio.

- Gauss y Gauss-Jordan se presentan en funciones independientes. Operaciones de eliminación, normalización, sustitución y cambio relativo escritas de forma explícita.
- Se mantienen la memoria dinámica, el pivoteo, la detección de singularidad, las entradas finitas, el caso de cero en el cambio relativo, el límite de iteraciones y el residuo.
- `--ejemplo` ejecuta los tres métodos con el problema principal. La entrada manual sigue disponible.
- La guía y el ZIP se regeneran antes de compilar; el paquete contiene el mismo C que muestra la web.

## Comprobaciones

- C11 compilado con GCC: `-Wall -Wextra -Werror -pedantic`, sin advertencias.
- 16 pruebas de Node: resultados, estado de los pasos y correspondencia de las líneas con la función correcta.
- Comparación del C nativo con los tres métodos de la web. Casos adicionales: pivoteo, sistemas 1×1 y 6×6, dimensiones incompatibles, singularidad, diagonal nula, solución cero, límite sin tolerancia, valores no finitos de Seidel y entradas inválidas.
- ZIP abierto con Python: CRC correcto; cuatro archivos idénticos a los de `public/`.
- Edge sin interfaz visible: tres métodos, avanzar/reiniciar, alternar resumen/matrices, descargas y guía; escritorio 1440 px y móvil 390 px; sin errores de JavaScript ni desplazamiento horizontal del documento.
- Inspección visual de `pulido-escritorio.png`, `pulido-movil.png` y `pulido-guia.png`.

Las capturas históricas se identifican como correspondientes a la versión anterior; la guía actual se genera desde el código vigente. El asistente `.cmd` requiere GCC en PATH; su menú interactivo no se ejecutó durante la revisión. La guía permite imprimir desde el navegador; no se verificó una exportación paginada a PDF. No se publicó en GitHub ni Vercel.
