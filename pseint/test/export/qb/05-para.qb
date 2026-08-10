DIM c AS DOUBLE
DIM i AS LONG
DIM j AS LONG
DIM a(1 TO 10) AS DOUBLE
FOR i = 1 TO 10
	a(i) = i*10
NEXT i
FOR aux_index_1 = LBOUND(a,1) TO UBOUND(a,1)
	PRINT a(aux_index_1)
NEXT aux_index_1
DIM b(1 TO 3, 1 TO 6) AS DOUBLE
c = 0
FOR aux_index_1 = LBOUND(b,1) TO UBOUND(b,1)
	FOR aux_index_2 = LBOUND(b,2) TO UBOUND(b,2)
		c = c+1
		b(aux_index_1,aux_index_2) = c
	NEXT aux_index_2
NEXT aux_index_1
FOR i = 3 TO 1 STEP -1
	FOR j = 1 TO 5 STEP 2
		PRINT b(i,j)
	NEXT j
NEXT i
