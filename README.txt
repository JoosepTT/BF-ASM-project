Projekt: BF kompilaator koos dünaamilise mälualaga
Autor: Joosep Tiger Tilgar

Kirjeldus:

Otsustasin teha edasiarenduse assembleri praktikumide käigus loodud BF translaatorist.
Sisuliselt on tegemist kompilaatoriga, mis genereerib BF sisendist masinkoodi, mis omakorda kasutab dünaamilist mäluala.


Käivitusprotsess:

1. Veenduda, et on paigaldatud gcc, nasm ja make.

2. Käivitamiseks anda järgnevad käsud:
   make clean
   make

   - Sellekäigus kompileerib make bf_compiler-i, mis on programm, mis loeb sisse .bf faili ning loob selle põhjal .asm masinkoodi.

Kasutamine:

1. Kirjuta oma BF programm faili, nt hello.bf, mis on hetkel loodud näidisena.

2. Käivita järgmine käsk, et luua asmi fail:
   ./bf_compiler hello.bf hello.asm

3. Kompileeri genereeritud programm käivitatavaks failiks:
   nasm -f elf32 hello.asm -o hello.o
   gcc -m32 -o hello hello.o mem.o

4. Käivita kompileeritud programm:
   ./hello

Tähelepanekud:
- Programm öötab 32-bit Linux keskkonnas, kuna kasutusel on NASM-kompilaator.
- Erinevate sisendite korral võib väljundis esineda vigu, kuna kõik erijuhud pole testitud.