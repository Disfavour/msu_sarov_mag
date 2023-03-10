PROGRAM TEST_B
	USE ISO_C_BINDING
	IMPLICIT NONE

	INTERFACE

		SUBROUTINE nCOPY(STRIN, STROUT, N) BIND(C)

			USE ISO_C_BINDING
			CHARACTER (KIND=C_CHAR), DIMENSION(*) :: STRIN, STROUT

			integer (KIND=C_INT) :: N
			
		END SUBROUTINE nCOPY

	END INTERFACE

	CHARACTER(LEN=10,KIND=C_CHAR)::DIGIT_STRING='123456789'//C_NULL_CHAR

	CHARACTER(KIND=C_CHAR) :: DIGIT_ARR(10) = 'x'

	integer (kind = C_INT) :: N = 7

	PRINT *, 'Before nCOPY: '
	PRINT *, DIGIT_STRING
	PRINT *, DIGIT_ARR

	call nCOPY(DIGIT_STRING, DIGIT_ARR, N)

	PRINT *, 'After call nCOPY(DIGIT_STRING,DIGIT_ARR,', N, ")"
	PRINT *, DIGIT_STRING
	PRINT *, DIGIT_ARR

END PROGRAM TEST_B
