MODULE TB
	USE ISO_C_BINDING

	! неявное связывание A_INT и a_int
	INTEGER(kind = C_INT), BIND(C)::A_INT

	! явное связывание binding B и b_long
	INTEGER(kind = C_LONG) :: B
	BIND(C, NAME=' b_long ') :: B

END MODULE TB
