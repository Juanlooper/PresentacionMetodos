export const variableMeaning = name => {
  const meanings = {
    n: 'Tamaño del sistema', ecuaciones: 'Número de ecuaciones', incognitas: 'Número de incógnitas',
    k: 'Columna pivote o iteración', i: 'Fila actual', j: 'Columna actual', p: 'Fila del pivote',
    factor: 'Múltiplo que se resta', divisor: 'Pivote antes de dividir', suma: 'Términos acumulados',
    x: 'Soluciones actuales', anterior: 'Soluciones anteriores', error: 'Mayor cambio (%)',
    e: 'Cambio de esta incógnita (%)', tolerancia: 'Cambio máximo permitido (%)',
    residuo: 'Diferencia máxima entre Ax y b', eps: 'Umbral numérico', limite: 'Máximo de iteraciones'
  };
  if (meanings[name]) return meanings[name];
  const cell = name.match(/^a\[(\d+)\]\[(\d+)\]$/);
  if (cell) return `Fila ${Number(cell[1])+1}, columna ${Number(cell[2])+1}`;
  const value = name.match(/^(x|anterior)\[(\d+)\]$/);
  if (value) return `x${Number(value[2])+1}${value[1]==='anterior'?' anterior':' actual'}`;
  return name;
};

export const explanation = {
  input: 'Primero leo el tamaño del sistema y guardo cada ecuación. La última columna es el término independiente b.',
  pivot: 'Busco el coeficiente de mayor valor absoluto en esta columna. Si hace falta, intercambio filas para usarlo como pivote.',
  normalize: 'Divido toda la fila por el pivote. Así el coeficiente principal se convierte en uno.',
  eliminate: 'Calculo un factor y resto ese múltiplo de la fila pivote. Así elimino un coeficiente sin cambiar la solución.',
  back: 'En Gauss resuelvo desde la última ecuación hacia arriba. En Jordan leo las soluciones de la última columna.',
  iterate: 'Despejo una incógnita y guardo su nuevo valor inmediatamente. La siguiente ecuación ya usa ese valor actualizado.',
  error: 'Comparo los valores nuevos con los anteriores y conservo el mayor cambio porcentual. Si cumple la tolerancia, termino.',
  result: 'Sustituyo las soluciones en las ecuaciones originales. El residuo mide cuánto difieren Ax y b.'
};

// Match only inside the selected C function: Gauss and Jordan share statements.
export function locateSourceLine(source, step) {
  const lines = source.split('\n');
  const start = lines.findIndex(line => line.startsWith(`void ${step.scope}(`) || line.startsWith(`int ${step.scope}(`));
  if (start < 0) return -1;
  let end = lines.findIndex((line, index) => index > start && /^(void|int|double) [a-z_*]+\(/.test(line));
  if (end < 0) end = lines.length;
  return lines.findIndex((line, index) => index >= start && index < end && line.includes(step.code));
}
