PROGRAM_spu := rSPU
LIBRARY_embed64 := rSPU.a

SRC = /opt/ibm/cell-sdk/prototype/src
USR = /opt/ibm/cell-sdk/prototype/sysroot/usr
TC = /opt/cell/toolchain-3.3/lib/gcc/spu/4.1.1/include

INCLUDE := -I.. -I$(SRC)/lib -I$(SRC)/lib/sim/spu -I$(USR)/spu/lib -I$(USR)/spu/include -I$(SRC)/include -I$(SRC)/lib/vector -I$(SRC)/lib/vector/spu -I$(SRC)/lib/gmath -I$(SRC)/lib/gmath/spu -I$(SRC)/lib/sync -I$(SRC)/lib/sync/spu -I$(SRC)/lib/misc -I$(SRC)/lib/misc/spu -I$(TC)/c++ -I$(TC)/c++/spu -I$(TC)/c++/backward -I$(TC) -I/opt/cell/toolchain-3.3/spu/include -I/opt/cell/toolchain-3.3/sysroot/usr/spu/include


IMPORTS = $(SRC)/lib/misc/spu/libmisc.a $(SRC)/lib/gmath/spu/libgmath.a $(SRC)/lib/vector/spu/libvector.a $(SDKLIB_spu)/libsync.a

CPPFLAGS += -g3

include /opt/ibm/cell-sdk/prototype/make.footer
