program task_2
	implicit none

	real, dimension(5) :: vec1, vec2
	integer :: i
	real :: ans

	call fillArray(vec1)
	call fillArray(vec2)

	call scalar_product(vec1, vec2, ans)

	print *, "scalar product =", ans

end program task_2


subroutine scalar_product(vec1, vec2, ans)
	implicit none

	real, dimension(5), intent(in) :: vec1, vec2
	real, intent(out) :: ans
	integer :: i

	ans = 0

	do i = 1, 5
		ans = ans + vec1(i) * vec2(i)
	end do

end subroutine scalar_product


subroutine fillArray(vec)
	implicit none

	real, dimension(5), intent(out) :: vec
	integer :: i

	do i = 1, 5
		vec(i) = i
	end do

end subroutine fillArray
