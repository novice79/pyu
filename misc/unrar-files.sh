#!/bin/sh

flist=(filestr.o recvol.o rs.o scantree.o qopen.o rar.o strlist.o strfn.o pathfn.o smallfn.o 
global.o file.o filefn.o filcreat.o 
  archive.o arcread.o unicode.o system.o crypt.o crc.o rawread.o encname.o 
  resource.o match.o timefn.o rdwrfn.o consio.o options.o errhnd.o rarvm.o secpassword.o 
  rijndael.o getbits.o sha1.o sha256.o blake2s.o hash.o extinfo.o extract.o volume.o 
  list.o find.o unpack.o headers.o threadpool.o rs16.o cmddata.o ui.o)

for i in "${flist[@]}"; do
  f="src/${i%.o}.cpp"
  echo $f
  # [[ -f src/$f ]] && cp -v src/$f src_/
  # f="${i%.o}.hpp"
  # [[ -f src/$f ]] && cp -v src/$f src_/
done