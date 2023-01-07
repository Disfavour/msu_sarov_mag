program task_1
	implicit none

	integer :: a, b

	read (*, *) a, b

	write (*, *) "a + b  =", a + b
	print *, "a - b  =", a - b
	print *, "a * b  =", a * b
	print *, "a ** b =", a ** b

	if (b == 0) then
		print *, "a / b  = division by zero"
	else
		print *, "a / b  =", a / b
	endif

end program task_1
