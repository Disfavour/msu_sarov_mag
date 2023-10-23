expression = '''
A[2][i][j] = 2*A[1][i][j] - A[0][i][j]
						+ tau*tau*(a*a*((A[1][i+1][j] - 2*A[1][i][j] + A[1][i-1][j])/(hy*hy)
						+ (A[1][i][j+1] - 2*A[1][i][j] + A[1][i][j-1])/(hx*hx)) + f(a, t, j*hx, i*hy));
'''

operations_num = 0

for symbol in ['+', '-', '*', '/']:
    operations_num += expression.count(symbol)
    print(symbol, expression.count(symbol))

print(operations_num)

