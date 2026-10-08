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
