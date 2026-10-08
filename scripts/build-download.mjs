import fs from 'node:fs';

// ZIP without external dependencies, so the download is also rebuilt on hosting.
const files=['laboratorio.c','guia-c.html','LEEME-C.txt','compilar-windows.cmd'];
const crc32 = data => {
  let crc=0xffffffff;
  for(const byte of data) {
    crc ^= byte;
    for(let bit=0;bit<8;bit++) crc=(crc>>>1)^((crc&1)?0xedb88320:0);
  }
  return (crc^0xffffffff)>>>0;
};
const local=[],central=[];
let offset=0;
for(const file of files) {
  const name=Buffer.from(file),data=fs.readFileSync(`public/${file}`),crc=crc32(data);
  const header=Buffer.alloc(30);
  header.writeUInt32LE(0x04034b50,0);header.writeUInt16LE(20,4);
  header.writeUInt16LE(0x21,12); // 1980-01-01; reproducible package.
  header.writeUInt32LE(crc,14);header.writeUInt32LE(data.length,18);
  header.writeUInt32LE(data.length,22);header.writeUInt16LE(name.length,26);
  local.push(header,name,data);
  const entry=Buffer.alloc(46);
  entry.writeUInt32LE(0x02014b50,0);entry.writeUInt16LE(20,4);entry.writeUInt16LE(20,6);
  entry.writeUInt16LE(0x21,14);entry.writeUInt32LE(crc,16);
  entry.writeUInt32LE(data.length,20);entry.writeUInt32LE(data.length,24);
  entry.writeUInt16LE(name.length,28);entry.writeUInt32LE(offset,42);
  central.push(entry,name);
  offset+=header.length+name.length+data.length;
}
const directory=Buffer.concat(central),end=Buffer.alloc(22);
end.writeUInt32LE(0x06054b50,0);end.writeUInt16LE(files.length,8);end.writeUInt16LE(files.length,10);
end.writeUInt32LE(directory.length,12);end.writeUInt32LE(offset,16);
fs.writeFileSync('public/laboratorio-c.zip',Buffer.concat([...local,directory,end]));
console.log('Paquete actualizado: C, guía visual, instrucciones y asistente Windows.');
