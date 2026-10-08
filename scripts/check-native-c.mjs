import {spawnSync} from 'node:child_process';
import assert from 'node:assert/strict';
import {solve,workshops} from '../src/engine.js';
for (const [method,id] of [['gauss',1],['jordan',2],['seidel',3]]) {
 const input='3 3\n'+workshops.map(r=>r.join(' ')).join('\n')+'\n'+id+'\n'+(id===3?'5 100\n':'');
 const run=spawnSync('./laboratorio-c.exe',{input,encoding:'utf8'});assert.equal(run.status,0);
 const actual=[...run.stdout.matchAll(/^x\d = ([\d.e+-]+)$/gm)].slice(-3).map(m=>Number(m[1]));
 const expected=solve(workshops,method).x;assert.equal(actual.length,3);expected.forEach((x,i)=>assert.ok(Math.abs(x-actual[i])<1e-7));
 console.log(method+': C y web coinciden.');
}
const other=spawnSync('./laboratorio-c.exe',{input:'2 2\n0 2 4\n1 1 3\n1\n',encoding:'utf8'});assert.match(other.stdout,/x1 = 1.00000000/);assert.match(other.stdout,/x2 = 2.00000000/);
const invalid=spawnSync('./laboratorio-c.exe',{input:'2 3\n',encoding:'utf8'});assert.equal(invalid.status,1);
console.log('Sistema 2x2 con pivoteo y dimensiones incompatibles: comprobados.');

const native=input=>spawnSync('./laboratorio-c.exe',{input,encoding:'utf8'});
const example=spawnSync('./laboratorio-c.exe',['--ejemplo'],{encoding:'utf8'});
assert.equal(example.status,0);
for(const title of ['--- 1. GAUSS ---','--- 2. GAUSS-JORDAN ---','--- 3. GAUSS-SEIDEL']) assert.ok(example.stdout.includes(title));
assert.match(example.stdout,/Tolerancia alcanzada en 4 iteraciones/);
for(const method of [1,2]) {
 assert.match(native(`1 1\n2 6\n${method}\n`).stdout,/x1 = 3.00000000/);
 const singular=native(`2 2\n1 2 3\n2 4 6\n${method}\n`);
 assert.match(singular.stdout,/Sin solucion unica/);
 assert.ok(!singular.stdout.includes('RESULTADO Y VERIFICACION'));
 const n=6;
 const identity=Array.from({length:n},(_,i)=>[...Array.from({length:n},(_,j)=>i===j?1:0),i+1].join(' ')).join('\n');
 assert.match(native(`${n} ${n}\n${identity}\n${method}\n`).stdout,/x6 = 6.00000000/);
}
assert.match(native('2 2\n0 1 2\n1 2 3\n3\n5 100\n').stdout,/Diagonal nula/);
assert.match(native('2 2\n1 0 0\n0 1 0\n3\n5 100\n').stdout,/Tolerancia alcanzada en 1 iteraciones/);
assert.match(native('2 2\n1 2 3\n3 1 4\n3\n0.001 5\n').stdout,/tolerancia NO alcanzada/);
assert.match(native('2 2\n1 1e6 1e308\n1e6 1 1\n3\n5 5\n').stdout,/Valores no finitos/);
assert.equal(native('1 1\nnan 1\n1\n').status,1);
assert.equal(native('1 1\n1 1\n4\n').status,1);
assert.equal(native('1 1\n1 1\n3\n0 100\n').status,1);
assert.equal(native('1 1\n1 1\n3\n5 0\n').status,1);
console.log('Ejemplo, 1x1, 6x6, singularidad, diagonal nula, ceros, limite, desbordamiento y entradas invalidas: comprobados.');
