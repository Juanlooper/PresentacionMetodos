export const workshops = [[.52,.20,.25,4800],[.30,.50,.20,5810],[.18,.30,.55,5690]];
export const fmt = n => !Number.isFinite(n) ? '∞' : new Intl.NumberFormat('es-CO',{maximumFractionDigits:5}).format(Object.is(n,-0)?0:n);
const matrixText = a => a.map(r=>r.map((v,j)=>(j===r.length-1?' | ':'')+v.toFixed(8).padStart(15)).join('')).join('\n');
export function solve(input, method='gauss', tolerance=5, maxIterations=100) {
 const n=input.length;
 if(!n||input.some(r=>r.length!==n+1||r.some(v=>!Number.isFinite(v)))) throw Error('Introduce un sistema cuadrado con todos sus valores numéricos.');
 if(!['gauss','jordan','seidel'].includes(method)) throw Error('Selecciona un método válido.');
 if(!Number.isInteger(maxIterations)||maxIterations<1) throw Error('El límite de iteraciones debe ser un entero positivo.');
 if(!(tolerance>0&&tolerance<=100)) throw Error('La tolerancia debe ser mayor que 0 y como máximo 100 %.');
 const a=input.map(r=>[...r]), steps=[], x=Array(n).fill(0);
 const scale=Math.max(...a.flatMap(r=>r.slice(0,n).map(Math.abs)));
 const eps=Number.EPSILON*scale*n*16;
 const residual=()=>Math.max(...input.map(r=>Math.abs(r.slice(0,n).reduce((s,v,j)=>s+v*x[j],0)-r[n])));
 // Each snapshot owns its variables, matrix, current source statement and stdout.
 // Rewinding therefore restores the complete execution state, with no future values.
 const record=(title,description,tag,rows=[],extra={})=>steps.push({title,description,tag,rows,matrix:a.map(r=>[...r]),x:[...x],vars:{n,...extra.vars},...extra});
 const outResult=()=>x.map((v,i)=>`x${i+1} = ${v.toFixed(8)}`).join('\n')+`\nResiduo maximo = ${residual().toFixed(8)}`;
 const mark=(title,description,tag,code,vars,rows=[],extra={})=>{
  const scope=tag==='input'?'main':tag==='result'?'resultado':method==='seidel'?'seidel':method==='jordan'?'gauss_jordan':'gauss';
  record(title,description,tag,rows,{code,scope,vars:{n,...vars},...extra});
 };
 mark('Leer el sistema','Se introducen las dimensiones y los coeficientes. Los índices de C empiezan en 0: a[0][0] es la primera celda.','input','double **a = crear(ecuaciones,incognitas+1);',{ecuaciones:n,incognitas:n},[],{stdout:`Cantidad de ecuaciones y cantidad de incognitas: ${n} ${n}\n`+input.map((r,i)=>`Fila ${i+1}: coeficientes y termino independiente: ${r.join(' ')}`).join('\n')+`\nMetodo: ${method==='gauss'?1:method==='jordan'?2:3}`+(method==='seidel'?`\nTolerancia porcentual y limite de iteraciones: ${tolerance} ${maxIterations}`:'')});
 mark('Mostrar la matriz inicial','mostrar(a) escribe en la terminal la misma matriz que aparece a la izquierda.','input','mostrar(a);',{n},[],{stdout:matrixText(a),scope:method==='seidel'?'seidel':method==='jordan'?'gauss_jordan':'gauss'});
 if(method==='seidel') {
  if(a.some((r,i)=>Math.abs(r[i])<=eps)) {mark('Detener: diagonal nula','No se puede dividir por cero. Reordena las ecuaciones.','iterate','if (fabs(a[i][i]) <= eps)',{eps},[],{scope:'seidel',stdout:'Diagonal nula: reordene las ecuaciones.'});return {steps,status:'zero',message:'Hay un cero o valor numéricamente nulo en la diagonal. Reordena las ecuaciones antes de aplicar Gauss-Seidel.'};}
  const dominant=a.every((r,i)=>Math.abs(r[i])>r.slice(0,n).reduce((s,v,j)=>s+(j===i?0:Math.abs(v)),0));
  mark('Inicializar las aproximaciones','Todas las incógnitas comienzan en cero. Se usarán inmediatamente los valores nuevos.','iterate','double *x = calloc((size_t)n,sizeof *x);',{x:[...x],tolerancia:tolerance},[],{scope:'seidel',stdout:dominant?'':'Sin dominancia estricta: convergencia no garantizada por este criterio.'});
  for(let k=1;k<=maxIterations;k++) {
   const old=[...x];
   mark(`Iteración ${k}: guardar el vector anterior`,'Copiamos x antes de actualizarlo para medir el cambio al final de la iteración.','iterate','anterior[j] = x[j];',{k,anterior:[...old],x:[...x]},[],{iteration:k});
   for(let i=0;i<n;i++) {
    let sum=0;
    for(let j=0;j<n;j++)if(j!==i){const term=a[i][j]*x[j];sum+=term;mark(`Acumular la suma para x${i+1}`,`suma += a[${i}][${j}] × x[${j}] = ${fmt(term)}. ${j<i?'Este valor de x ya se actualizó en esta iteración.':'Este valor de x viene de la iteración anterior.'}`,'iterate','suma = suma + a[i][j]*x[j];',{k,i,j,suma:sum,[`a[${i}][${j}]`]:a[i][j],[`x[${j}]`]:x[j]},[i],{cell:[i,j],iteration:k});}
    x[i]=(a[i][n]-sum)/a[i][i];
    if(!Number.isFinite(x[i])) {mark('Detener: valor no finito','La iteración excedió el rango numérico.','iterate','if (!isfinite(x[i]))',{k,i},[i],{stdout:'Valores no finitos: detener.'});return {steps,status:'nonfinite',message:'La iteración produjo valores no finitos; no se alcanzó la tolerancia.',dominant};}
    mark(`Iteración ${k} · despejar x${i+1}`,`x[${i}] = (${fmt(a[i][n])} − ${fmt(sum)}) / ${fmt(a[i][i])} = ${fmt(x[i])}.`,'iterate','x[i] = (a[i][n]-suma)/a[i][i];',{k,i,suma:sum,[`a[${i}][${n}]`]:a[i][n],[`a[${i}][${i}]`]:a[i][i],[`x[${i}]`]:x[i]},[i],{iteration:k});
    mark(`Imprimir x${i+1}`,'printf envía el valor recién calculado a la terminal.','iterate','printf("Iteracion %d',{k,i,[`x[${i}]`]:x[i]},[i],{iteration:k,stdout:`Iteracion ${k}, x${i+1} = ${x[i].toFixed(8)}`});
   }
   let error=0;
   for(let i=0;i<n;i++){const e=x[i]===0?(old[i]===0?0:Infinity):Math.abs((x[i]-old[i])/x[i])*100;error=Math.max(error,e);mark(`Evaluar el cambio de x${i+1}`,`Cambio relativo = ${fmt(e)} %. Conservamos el máximo de las componentes evaluadas.`,'error','if (e > error) {',{k,i,e,error,[`x[${i}]`]:x[i],[`anterior[${i}]`]:old[i]},[],{iteration:k});}
   const res=residual();
   mark(`Iteración ${k} · comprobar el error`,`${fmt(error)} % ${error<=tolerance?'≤':'>'} ${tolerance} %. ${error<=tolerance?'Se cumple la tolerancia.':'Se necesita otra iteración.'} El cambio relativo no es el error verdadero.`,'error','if (error <= tolerancia)',{k,error,tolerancia:tolerance,residuo:res},[],{error,residual:res,iteration:k,stdout:`Cambio maximo (%) = ${error.toFixed(8)}\n${outResult()}`+(error<=tolerance?`\nTolerancia alcanzada en ${k} iteraciones.`:'')});
   if(error<=tolerance) return {steps,x:[...x],status:'converged',iterations:k,error,residual:res,dominant};
  }
  mark('Límite de iteraciones','El algoritmo termina sin cumplir la tolerancia.','error','printf("Limite de iteraciones:',{limite:maxIterations},[],{stdout:'Limite de iteraciones: tolerancia NO alcanzada.'});
  return {steps,x:[...x],status:'limit',iterations:maxIterations,residual:residual(),dominant,message:'Se alcanzó el límite de iteraciones sin cumplir la tolerancia.'};
 }
 for(let k=0;k<n;k++) {
  let p=k;
  mark(`Iniciar la búsqueda del pivote · columna ${k+1}`,`p = k = ${k}. La primera candidata es la fila ${k+1}.`,'pivot','int p = k;',{k,p},[p],{cell:[p,k]});
  for(let i=k+1;i<n;i++) {
   const previousP=p,replace=Math.abs(a[i][k])>Math.abs(a[p][k]);
   if(replace)p=i;
   mark(`Comparar la fila ${i+1} con el pivote`,`|${fmt(a[i][k])}| > |${fmt(a[previousP][k])}|: ${replace?'verdadero; p cambia a '+i:'falso; p permanece en '+p}.`,'pivot','if (fabs(a[i][k]) > fabs(a[p][k])) {',{k,i,p,[`a[${i}][${k}]`]:a[i][k],[`a[${previousP}][${k}]`]:a[previousP][k]},[i,p],{cell:[i,k]});
  }
  if(Math.abs(a[p][k])<=eps) {mark('Detener: pivote nulo','No se obtiene una solución única con esta precisión.','pivot','if (fabs(a[p][k]) <= eps)',{k,p,eps},[p],{stdout:'Sin solucion unica a esta precision.'});return {steps,status:'singular',message:'No se puede obtener una solución única con esta precisión: matriz singular o numéricamente singular.'};}
  if(p!==k) { [a[p],a[k]]=[a[k],a[p]];mark(`Intercambiar F${k+1} ↔ F${p+1}`,'Intercambiamos filas completas; el sistema conserva sus soluciones.','pivot','a[k] = temporal;',{k,p},[k,p],{stdout:`Intercambiar F${k+1} y F${p+1}\n${matrixText(a)}`}); }
  if(method==='jordan') {
   const d=a[k][k];mark('Guardar el divisor',`Guardamos el pivote (${fmt(d)}) antes de modificar la fila.`,'normalize','double divisor = a[k][k];',{k,divisor:d},[k],{cell:[k,k]});
   for(let j=k;j<=n;j++){const before=a[k][j];a[k][j]/=d;mark(`Normalizar a[${k}][${j}]`,`${fmt(before)} / ${fmt(d)} = ${fmt(a[k][j])}.`,'normalize','a[k][j] = a[k][j] / divisor;',{k,j,divisor:d,[`a[${k}][${j}]`]:a[k][j]},[k],{cell:[k,j]});}
   mark(`Mostrar F${k+1} normalizada`,'La fila ya tiene un 1 en el pivote. Se imprime la matriz actualizada.','normalize','printf("Normalizar F%d',{k,divisor:d},[k],{stdout:`Normalizar F${k+1} / ${d.toFixed(8)}\n${matrixText(a)}`});
  }
  for(let i=method==='jordan'?0:k+1;i<n;i++) {
   if(i===k||a[i][k]===0)continue;
   const factor=a[i][k]/a[k][k];
   mark(`Calcular el factor para F${i+1}`,`factor = ${fmt(a[i][k])} / ${fmt(a[k][k])} = ${fmt(factor)}. Este múltiplo permitirá eliminar el coeficiente.`,'eliminate','double factor = a[i][k]/a[k][k];',{k,i,factor,[`a[${i}][${k}]`]:a[i][k],[`a[${k}][${k}]`]:a[k][k]},[k,i],{cell:[i,k]});
   for(let j=k+1;j<=n;j++){const before=a[i][j];a[i][j]-=factor*a[k][j];mark(`Actualizar a[${i}][${j}]`,`${fmt(before)} − ${fmt(factor)} × ${fmt(a[k][j])} = ${fmt(a[i][j])}. ${j===n?'También se actualiza el término independiente.':''}`,'eliminate','a[i][j] = a[i][j] - factor*a[k][j];',{k,i,j,factor,[`a[${i}][${j}]`]:a[i][j],[`a[${k}][${j}]`]:a[k][j]},[k,i],{cell:[i,j]});}
   a[i][k]=0;mark('Fijar el coeficiente eliminado en cero','Al restar el múltiplo adecuado, este coeficiente se anula. Lo fijamos en cero para evitar restos de redondeo.','eliminate','a[i][k] = 0;',{k,i,factor,[`a[${i}][${k}]`]:0},[i],{cell:[i,k]});
   mark(`F${i+1} ← F${i+1} − (${fmt(factor)}) F${k+1}`,'printf describe la operación y mostrar(a) imprime la matriz resultante.','eliminate','printf("F%d -=',{k,i,factor},[k,i],{stdout:`F${i+1} -= ${factor.toFixed(8)} * F${k+1}\n${matrixText(a)}`});
  }
 }
 if(method==='gauss')for(let i=n-1;i>=0;i--){
  let sum=0;
  mark(`Preparar la sustitución de x${i+1}`,'Comenzamos con suma = 0. Las incógnitas de filas inferiores ya están resueltas.','back','double suma = 0;',{i,suma:sum},[i]);
  for(let j=i+1;j<n;j++){sum+=a[i][j]*x[j];mark('Acumular los términos conocidos',`Sumamos a[${i}][${j}] × x[${j}].`,'back','suma = suma + a[i][j]*x[j];',{i,j,suma:sum,[`x[${j}]`]:x[j]},[i],{cell:[i,j]});}
  x[i]=(a[i][n]-sum)/a[i][i];mark(`Sustitución hacia atrás · x${i+1}`,`x[${i}] = (${fmt(a[i][n])} − ${fmt(sum)}) / ${fmt(a[i][i])} = ${fmt(x[i])}.`,'back','x[i] = (a[i][n]-suma)/a[i][i];',{i,suma:sum,[`x[${i}]`]:x[i]},[i]);
  mark(`Imprimir x${i+1}`,'Se escribe el resultado de la sustitución en la terminal.','back','printf("Sustitucion:',{i,[`x[${i}]`]:x[i]},[i],{stdout:`Sustitucion: x${i+1} = ${x[i].toFixed(8)}`});
 }else for(let i=0;i<n;i++){x[i]=a[i][n];mark(`Leer x${i+1} de la última columna`,'La matriz de coeficientes es la identidad; el término independiente de cada fila es su solución.','back','x[i] = a[i][n];',{i,[`x[${i}]`]:x[i]},[i],{cell:[i,n]});}
 mark('Solución del sistema','Se imprimen las incógnitas y se verifica el residuo en el sistema original.','result','printf("Residuo maximo =',{x:[...x],residuo:residual()},[],{residual:residual(),stdout:outResult()});
 return {steps,x,status:'solved',residual:residual()};
}
