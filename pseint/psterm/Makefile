include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/psterm
CXXFLAGS+=${WX_CXXFLAGS}
LDFLAGS+=${WX_LDFLAGS}

ifdef WINDOWS
ICON=${OBJS_DIR}\icon.o
else
ICON=
endif

all: ${OBJS_DIR} ../${BIN}/psterm${EXT}

include Makefile.common

clean:
	rm -rf ${OBJS} ../${BIN}/psterm${EXT} ${ICON}

../${BIN}/psterm${EXT}: ../${BIN} ${OBJS} ${ICON}
	${GPP} ${OBJS} ${LDFLAGS} ${ICON} -o $@

${OBJS_DIR}:
	mkdir -p ${OBJS_DIR}

${ICON}: ../wxPSeInt/icon.rc ../bin/imgs/icon.ico
	${WINDRES} -I${WX_DIR}/include -i ../wxPSeInt/icon.rc -o $@

../${BIN}:
	${MKDIR} ../${BIN}
