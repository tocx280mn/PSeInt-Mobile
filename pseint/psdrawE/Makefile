include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/psdrawE
CXXFLAGS+=${WX_CXXFLAGS} -D_FOR_EXPORT -I. -Wno-narrowing
LDFLAGS+=${WX_LDFLAGS}

ifdef WINDOWS
ICON=${OBJS_DIR}/icon.o
else
ICON=
endif


all: ${OBJS_DIR} ../${BIN}/psdrawE${EXT}

include Makefile.common

clean:
	${RM} ${OBJS} ../${BIN}/psdrawE${EXT}

../${BIN}/psdrawE${EXT}: ../${BIN} ${OBJS} ${ICON}
	${GPP} ${OBJS} ${LDFLAGS} ${ICON} -o $@

${OBJS_DIR}:
	${MKDIR} ${OBJS_DIR}

ifdef WINDOWS
${ICON}: ../wxPSeInt/icon.rc ../bin/imgs/icon.ico
	${WINDRES} -I${WX_DIR}/include -i ../wxPSeInt/icon.rc -o $@
endif

../${BIN}:
	${MKDIR} ../${BIN}
