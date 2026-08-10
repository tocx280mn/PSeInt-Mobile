if __name__ == '__main__':
	a = [float() for ind0 in range(10)]
	for i in range(1,11):
		a[i-1] = i*10
	for aux_index_0 in range(10):
		print(a[aux_index_0])
	b = [[float() for ind0 in range(6)] for ind1 in range(3)]
	c = 0
	for aux_index_0 in range(3):
		for aux_index_1 in range(6):
			c = c+1
			b[aux_index_0][aux_index_1] = c
	for i in range(3,0,-1):
		for j in range(1,6,2):
			print(b[i-1][j-1])
