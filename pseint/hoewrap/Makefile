include ../configs/config.${ARCH}
OBJS_DIR=../${TEMP}/hoewrap

all: ${OBJS_DIR} ${OBJS_DIR}/libhoewrap.a

include Makefile.common

clean:
	${RM} ${OBJS} ${LIBS_OBJS}

${OBJS_DIR}/libhoewrap.a: ${OBJS_DIR}/hoewrap.o ${OBJS_DIR}/html.o ${OBJS_DIR}/document.o ${OBJS_DIR}/buffer.o ${OBJS_DIR}/escape.o ${OBJS_DIR}/stack.o ${OBJS_DIR}/autolink.o ${OBJS_DIR}/html_blocks.o
	${AR} cr ${OBJS_DIR}/libhoewrap.a ${OBJS_DIR}/hoewrap.o ${OBJS_DIR}/html.o ${OBJS_DIR}/document.o ${OBJS_DIR}/buffer.o ${OBJS_DIR}/escape.o ${OBJS_DIR}/stack.o ${OBJS_DIR}/autolink.o ${OBJS_DIR}/html_blocks.o 

${OBJS_DIR}:
	${MKDIR} ${OBJS_DIR}
