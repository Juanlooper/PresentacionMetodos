import test from 'node:test';
import assert from 'node:assert/strict';
import {solve,workshops} from '../src/engine.js';
for(const m of ['gauss','jordan']){
 test(m+' solves workshops and preserves input',()=>{const before=JSON.stringify(workshops),r=solve(workshops,m);assert.equal(r.status,'solved');assert.ok(r.residual<1e-9);r.x.forEach((x,i)=>assert.ok(Math.abs(x-[172500/43,308000/43,220400/43][i])<1e-8));assert.equal(JSON.stringify(workshops),before)});
 test(m+' pivots and supports variable size',()=>{const r=solve([[0,2,4],[1,1,3]],m);assert.deepEqual(r.x,[1,2]);assert.ok(r.steps.some(s=>s.title.includes('↔')));assert.deepEqual(solve([[2,6]],m).x,[3]);const a=Array.from({length:6},(_,i)=>Array.from({length:7},(_,j)=>j===6?i+1:i===j?1:0));assert.deepEqual(solve(a,m).x,[1,2,3,4,5,6]);});
 test(m+' singular is not success',()=>assert.equal(solve([[1,2,3],[2,4,6]],m).status,'singular'));
}
test('Seidel 5 percent in four iterations',()=>{const r=solve(workshops,'seidel',5);assert.equal(r.status,'converged');assert.equal(r.iterations,4);assert.ok(r.error<=5);assert.ok(r.residual>1);assert.equal(r.dominant,false)});
test('Seidel limit is not convergence',()=>assert.equal(solve([[1,2,3],[3,1,4]],'seidel',.001,5).status,'limit'));
test('Zero diagonal handled',()=>assert.equal(solve([[0,1,2],[1,2,3]],'seidel').status,'zero'));
test('Zero solution does not divide by zero',()=>{const r=solve([[1,0,0],[0,1,0]],'seidel');assert.equal(r.status,'converged');assert.equal(r.error,0)});
test('Reject malformed inputs',()=>{assert.throws(()=>solve([[1,2,3]]));assert.throws(()=>solve([[NaN,2]]));assert.throws(()=>solve(workshops,'seidel',0))});


test('Elimination exposes factor before changing cells, then changes one cell at a time',()=>{
 const r=solve(workshops),factorIndex=r.steps.findIndex(s=>s.code==='double factor = a[i][k]/a[k][k];');
 const f=r.steps[factorIndex];assert.deepEqual(f.matrix,workshops);assert.equal(f.vars.i,1);assert.equal(f.vars.k,0);assert.equal(f.vars.factor,.3/.52);
 const next=r.steps[factorIndex+1];assert.deepEqual(next.cell,[1,1]);assert.equal(next.vars.j,1);assert.ok(Math.abs(next.matrix[1][1]-(.5-.3/.52*.2))<1e-12);assert.equal(next.matrix[1][2],.2);assert.equal(next.stdout,undefined);
 assert.deepEqual(f.matrix,workshops,'Earlier snapshot must not mutate');
});
test('Every executed statement maps to actual C source',async()=>{
 const fs=await import('node:fs/promises');const code=await fs.readFile(new URL('../public/laboratorio.c',import.meta.url),'utf8');
 for(const method of ['gauss','jordan','seidel'])for(const s of solve(workshops,method).steps){assert.ok(s.code&&code.includes(s.code),s.code);assert.equal(s.vars.n,3);}
});
test('Seidel reveals new and old variable values and ends with real numeric output',()=>{
 const r=solve(workshops,'seidel');const firstX=r.steps.find(s=>s.code==='x[i] = (a[i][n]-suma)/a[i][i];');
 assert.equal(firstX.vars['x[0]'],4800/.52);assert.equal(firstX.x[1],0);
 const sum=r.steps.find(s=>s.tag==='iterate'&&s.vars.i===1&&s.vars.j===0);assert.equal(sum.vars['x[0]'],firstX.x[0]);
 assert.match(r.steps.at(-1).stdout,/Tolerancia alcanzada en 4 iteraciones/);assert.match(r.steps.at(-1).stdout,/11.83597208/);
});
