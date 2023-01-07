PROGRAM TEST_B
	USE ISO_C_BINDING
	IMPLICIT NONE

	INTERFACE

		SUBROUTINE my_strcmp(str1, str2) BIND(C)

			USE ISO_C_BINDING
			CHARACTER (KIND=C_CHAR), DIMENSION(*) :: str1, str2

		END SUBROUTINE my_strcmp

	END INTERFACE

	CHARACTER(LEN=10,KIND=C_CHAR)::str1='123456789'//C_NULL_CHAR

	CHARACTER(LEN=10,KIND=C_CHAR)::str2='123456789'//C_NULL_CHAR

	CHARACTER(LEN=11,KIND=C_CHAR)::str3='1234567899'//C_NULL_CHAR

	CHARACTER(LEN=10,KIND=C_CHAR)::str4='123456787'//C_NULL_CHAR

	print *, "Strings:"

	print *, "str1 =", str1
	print *, "str2 =", str2
	print *, "str3 =", str3
	print *, "str4 =", str4

	print *, "call my_strcmp(str1, str2)"
	call my_strcmp(str1, str2)

	print *, "call my_strcmp(str1, str3)"
	call my_strcmp(str1, str3)

	print *, "call my_strcmp(str1, str4)"
	call my_strcmp(str1, str4)

END PROGRAM TEST_B
