program task_3
	implicit none

	real, dimension(5) :: vec1, vec2, vec3
	real :: ans

	call fillArray(vec1)
	call fillArray(vec2)

	call sum_vec(vec1, vec2, vec3)

	print *, vec1
	print *, vec2
	print *, vec3

end program task_3


subroutine sum_vec(vec1, vec2, vec3)
	implicit none

	real, dimension(5), intent(in) :: vec1, vec2
	real, dimension(5), intent(out) :: vec3
	integer :: i

	do i = 1, 5
		vec3(i) = vec1(i) + vec2(i)
	end do

end subroutine sum_vec


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
