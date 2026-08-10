include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/wxpseint
CXXFLAGS+=-DFOR_WXPSEINT ${WX_CXXFLAGS}
LDFLAGS+=${WX_LDFLAGS}

ifdef WINDOWS
ICON=${OBJS_DIR}/icon.o
EXTRA_BINS+=../bin/log.exe
else
ICON=
EXTRA_BINS=
endif

ifeq ("${ARCH}","lnx")
EXTRA_BINS+=../bin/wxPSeInt ../bin/pseint
endif

all: ${OBJS_DIR} ../${BIN}/wxPSeInt${EXT} ${EXTRA_BINS}

ifdef WINDOWS
${ICON}: icon.rc ../bin/imgs/icon.ico manifest.xml
	${WINDRES} -I${WX_DIR}/include -i icon.rc -o $@

../bin/log.exe: log-launcher.cpp
	${GPP} -O0 -mwindows -static-libgcc -static-libstdc++ log-launcher.cpp -o $@

endif

ifeq ("${ARCH}","lnx")
../bin/wxPSeInt: launcher.cpp
	${GPP} launcher.cpp -O2 -ldl -o $@

../bin/pseint: launcher.cpp
	${GPP} launcher.cpp -O2 -ldl -o $@
endif

include Makefile.common

clean:
	${RM} ${OBJS} ../${BIN}/wxPSeInt{EXT} ${EXTRA_BINS} ${ICON}

../${BIN}/wxPSeInt${EXT}: ../${BIN} ${OBJS} ${ICON}
	${GPP} ${OBJS} ${LDFLAGS} ${ICON} -o $@

${OBJS_DIR}:
	${MKDIR} ${OBJS_DIR}

