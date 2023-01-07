#include <math.h>
#include <stdlib.h>
#include <stdio.h>


double f1(double x);
double f2(double x);
double f3(double x);
double root(double (*f)(double), double (*g)(double), double a, double b, double eps);
double rectangle_method(double (*f)(double), double a, double b, int parts);
double integral(double (*f)(double), double a, double b, double eps);


int main()
{	
	double eps = 1e-6;

	double
		left  = root(f1, f3, -3, -2, eps),
		bot   = root(f2, f3, 0, 1, eps),
		right = root(f1, f2, 1, 2, eps);

	printf("f1 = f3 in x = %lf\n", left);
	printf("f2 = f3 in x = %lf\n", bot);
	printf("f1 = f2 in x = %lf\n", right);

	double
		integral_top = integral(f1, left, right, eps),
		integral_left_bot = integral(f3, left, bot, eps),
		integral_right_bot = integral(f2, bot, right, eps);

	printf("Integral f1 from %lf to %lf is %lf\n", left, right, integral_top);
	printf("Integral f2 from %lf to %lf is %lf\n", left, bot, integral_left_bot);
	printf("Integral f3 from %lf to %lf is %lf\n", bot, right, integral_right_bot);

	double answer = integral_top - integral_left_bot - integral_right_bot;

	printf("Answer = %lf\n", answer);

	return 0;
}


double f1(double x)
{
	return pow(2, x) + 1;
}


double f2(double x)
{
	return pow(x, 5);
}


double f3(double x)
{
	return (1 - x) / 3;
}


// Координата x точки пересечения функций f и g на отрезке [a, b]
double root(double (*f)(double), double (*g)(double), double a, double b, double eps)
{
	if ((f(a) - g(a)) * (f(b) - g(b)) >= 0)
	{
		printf("bisection requirements do not met\n");
		exit(0);
	}

	double
		x = a,
		len = b - a;

	while (b - a > eps)
	{
		len /= 2;

		x = a + len;

		if ((f(a) - g(a)) * (f(x) - g(x)) < 0)
		{
			b = x;
		}
		else
		{
			a = x;
		}
	}

	return x;
}


double rectangle_method(double (*f)(double), double a, double b, int parts)
{
	double
		S = 0,
		len = b - a,
		delta = len / parts;

	for (double x = a + delta / 2; x < b; x += delta)
	{
		S += f(x);
	}

	S *= delta;

	return S;
}


double integral(double (*f)(double), double a, double b, double eps)
{
	int n = 5;

	double Sn[2];

	double p = 1. / 3;

	for (int i = 0; i < 2; i++)
	{
		n *= 2;
		Sn[i] = rectangle_method(f, a, b, n);
	}

	while (p * fabs(Sn[0] - Sn[1]) > eps)
	{
		n *= 2;

		Sn[0] = Sn[1];
		Sn[1] = rectangle_method(f, a, b, n);
	}

	return Sn[1];
}
