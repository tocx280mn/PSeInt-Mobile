include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/pseint
CXXFLAGS+=-DUSE_ZOCKETS
ifndef MACOS
CXXFLAGS+=-ffloat-store
endif

ifdef WINDOWS
LDFLAGS+=-lwsock32
endif

all: ${OBJS_DIR} ../${BIN}/pseint${EXT}

include Makefile.common

clean:
	${RM} ${OBJS} ../${BIN}/pseint${EXT}

../${BIN}/pseint${EXT}: ../${BIN} ${OBJS}
	${GPP} ${OBJS} ${LDFLAGS} -o $@

${OBJS_DIR}:
	${MKDIR} ${OBJS_DIR}


../${BIN}:
	${MKDIR} ../${BIN}
