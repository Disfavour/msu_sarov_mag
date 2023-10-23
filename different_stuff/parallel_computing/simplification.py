import sympy as sp

# Определите символьные переменные
N, Mx, My, p, t, ts, k = sp.symbols('N Mx My p t ts k')

layer0 = ((My+1)*(Mx+1)*(k*t+2*t)) / p + ts
layer1 = ((My-1)*(Mx-1)*(27*t+2*k*t)) / p + t \
    + ((My-1)*(2*k*t + 2*t)) / p + ((Mx+1)*(2*k*t+2*t)) / p + ts
layers = (N-1)*(ts + ((My-1)*(Mx-1)*(25*t+k*t)) / p + t \
                + ((My-1)*(2*t+2*k*t)) / p + ((Mx+1)*(2*t+2*k*t)) / p + ts)
expr = layer0 + layer1 + layers

# Ваше символьное выражение
#expr = (x**2 - y**2) / (x - y)

#expr2 = x*y/2*x
#print(expr + expr2)

# Сокращение выражения
simplified_expr = sp.simplify(expr)

# Вывод результатов
print("Исходное выражение:", expr)
print("Сокращенное выражение:", simplified_expr)