DIM c AS DOUBLE
PRINT funciondoble(5)
CALL noretornanada(3,9)
c = 0
CALL porreferencia(c)
PRINT c
DIM a(1 TO 10) AS STRING, b(1 TO 3, 1 TO 4) AS STRING
CALL recibevector(a)
CALL recibematriz(b)

FUNCTION funciondoble (b AS DOUBLE)
DIM a AS DOUBLE
a = 2*b
funciondoble = a
END FUNCTION

SUB noretornanada (a AS STRING, b AS STRING)
PRINT a+b
END SUB

SUB porreferencia (b AS DOUBLE)
b = 7
END SUB

SUB recibevector (v() AS STRING)
INPUT v(1)
END SUB

SUB recibematriz (m(,) AS STRING)
INPUT m(1,1)
END SUB
