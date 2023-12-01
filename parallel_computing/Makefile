params=5 6 4 500 600 400
omp=4
mpi=4
flags=-Wall

run: wave_eq_2d_mpi_openmp wave_eq_2d_mpi_openmp_isend wave_eq_2d_mpi_openmp_send wave_eq_2d_mpi_openmp_sendrecv
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp $(params)
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp_isend $(params)
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp_send $(params)
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp_sendrecv $(params)

run_all: wave_eq_2d_mpi_openmp wave_eq_2d_mpi_openmp_isend wave_eq_2d_mpi_openmp_send wave_eq_2d_mpi_openmp_sendrecv wave_eq_2d_mpi wave_eq_2d_openmp wave_eq_2d
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp $(params)
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp_isend $(params)
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp_send $(params)
	OMP_NUM_THREADS=$(omp) mpirun --bind-to none --oversubscribe -n $(mpi) ./wave_eq_2d_mpi_openmp_sendrecv $(params)
	mpirun --oversubscribe -n 6 ./wave_eq_2d_mpi 2 3 $(params)
	OMP_NUM_THREADS=16 ./wave_eq_2d_openmp $(params)
	./wave_eq_2d $(params)

wave_eq_2d_mpi_openmp: wave_eq_2d_mpi_openmp.c
	mpicc $< -lm -fopenmp $(flags) -o $@

wave_eq_2d_mpi_openmp_isend: wave_eq_2d_mpi_openmp_isend.c
	mpicc $< -lm -fopenmp $(flags) -o $@

wave_eq_2d_mpi_openmp_send: wave_eq_2d_mpi_openmp_send.c
	mpicc $< -lm -fopenmp $(flags) -o $@

wave_eq_2d_mpi_openmp_sendrecv: wave_eq_2d_mpi_openmp_sendrecv.c
	mpicc $< -lm -fopenmp $(flags) -o $@

wave_eq_2d_mpi: wave_eq_2d_mpi.c
	mpicc wave_eq_2d_mpi.c -lm -o wave_eq_2d_mpi

wave_eq_2d_openmp: wave_eq_2d_openmp.c
	gcc wave_eq_2d_openmp.c -lm -fopenmp -o wave_eq_2d_openmp

wave_eq_2d: wave_eq_2d.c
	gcc wave_eq_2d.c -lm -o wave_eq_2d

clean: 
	rm wave_eq_2d_mpi_openmp
	rm wave_eq_2d_mpi_openmp_isend
	rm wave_eq_2d_mpi_openmp_send
	rm wave_eq_2d_mpi_openmp_sendrecv

clean_all: wave_eq_2d_mpi_openmp wave_eq_2d_mpi_openmp_isend wave_eq_2d_mpi_openmp_send wave_eq_2d_mpi_openmp_sendrecv wave_eq_2d_mpi wave_eq_2d_openmp wave_eq_2d
	rm wave_eq_2d_mpi_openmp
	rm wave_eq_2d_mpi_openmp_isend
	rm wave_eq_2d_mpi_openmp_send
	rm wave_eq_2d_mpi_openmp_sendrecv
	rm wave_eq_2d_mpi
	rm wave_eq_2d_openmp
	rm wave_eq_2d
