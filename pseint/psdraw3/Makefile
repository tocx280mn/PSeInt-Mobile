include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/psdraw3
CXXFLAGS+=${WX_CXXFLAGS} ${GL_CXXFLAGS} -D_USE_DF -Wno-narrowing
LDFLAGS+=${WX_LDFLAGS} ${GL_LDFLAGS}

ifdef WINDOWS
ICON=${OBJS_DIR}/icon.o
else
ICON=
endif

all: ${OBJS_DIR} ../${BIN}/psdraw3${EXT}

include Makefile.common

clean:
	${RM} ${OBJS} ../${BIN}/psdraw3${EXT} ${ICON}

../${BIN}/psdraw3${EXT}: ../${BIN} ${OBJS} ${ICON}
	${GPP} ${OBJS} ${LDFLAGS} ${ICON} -o $@

${OBJS_DIR}:
	${MKDIR} ${OBJS_DIR}

ifdef WINDOWS
${ICON}: ../wxPSeInt/icon.rc ../bin/imgs/icon.ico
	${WINDRES} -I${WX_DIR}/include -i ../wxPSeInt/icon.rc -o $@
endif

../${BIN}:
	${MKDIR} ../${BIN}
