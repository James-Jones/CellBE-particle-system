PROGRAM_ppu64 := PPU

SRC = /opt/ibm/cell-sdk/prototype/src
USR = /opt/ibm/cell-sdk/prototype/sysroot/usr
TC = /opt/cell/toolchain-3.3/lib/gcc/ppu/4.1.1/include

INCLUDE := -I.. -I$(SRC)/lib -I$(USR)/include -I$(USR)/lib -I$(SRC)/include/ppu -I$(SRC)/lib/vector/ppu -I$(SRC)/lib/sync/ppu -I$(TC)/c++ -I$(TC)/c++/ppu -I$(TC)/c++/backward -I$(TC) -I/opt/cell/toolchain-3.3/sysroot/usr/include -maltivec

IMPORTS := ../spu/SPU.a ../spub/SPUb.a -lspe2 -lc

CPPFLAGS += -g3

include /opt/ibm/cell-sdk/prototype/make.footer
