if __name__ == '__main__':
	a = [str() for ind0 in range(10)]
	b = [[str() for ind0 in range(30)] for ind1 in range(20)]
	c = [str() for ind0 in range(40)]
	d = [[str() for ind0 in range(5)] for ind1 in range(50)]
	n = int(input())
	e = [str() for ind0 in range(n)]
	a[0] = input()
	a[9] = a[0]
	print(b[9][9])
