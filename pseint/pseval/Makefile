include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/pseval
CXXFLAGS+=-DALLOW_MARKDOWN ${WX_CXXFLAGS}
LDFLAGS+=${WX_LDFLAGS} -L../${TEMP}/hoewrap -lhoewrap

ifdef WINDOWS
ICON=${OBJS_DIR}/icon.o
endif

all: ${OBJS_DIR} ../${BIN}/pseval${EXT}

include Makefile.common

clean:
	${RM} ${OBJS} ../${BIN}/pseval${EXT} ${ICON}

../${BIN}/pseval${EXT}: ../${BIN} ${OBJS} ${ICON}
	${GPP} ${OBJS} ${LDFLAGS} ${ICON} -o $@

${OBJS_DIR}:
	${MKDIR} ${OBJS_DIR}

ifdef WINDOWS
${ICON}: ../wxPSeInt/icon.rc ../bin/imgs/icon.ico
	${WINDRES} -I${WX_DIR}/include -i ../wxPSeInt/icon.rc -o $@
endif

../${BIN}:
	${MKDIR} ../${BIN}
