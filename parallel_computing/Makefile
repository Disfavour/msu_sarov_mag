params=5 6 4 500 600 400

run: wave_eq_2d_mpi_openmp wave_eq_2d_mpi_openmp_v2 wave_eq_2d_mpi wave_eq_2d wave_eq_2d_openmp
	OMP_NUM_THREADS=1 mpirun --oversubscribe -n 6 ./wave_eq_2d_mpi_openmp 2 3 $(params)
	OMP_NUM_THREADS=1 mpirun --oversubscribe -n 6 ./wave_eq_2d_mpi_openmp_v2 2 3 $(params)
	mpirun --oversubscribe -n 6 ./wave_eq_2d_mpi 2 3 $(params)
	./wave_eq_2d_openmp $(params)
	./wave_eq_2d $(params)

wave_eq_2d_mpi_openmp_v2: wave_eq_2d_mpi_openmp_v2.c
	mpicc wave_eq_2d_mpi_openmp_v2.c -lm -fopenmp -o wave_eq_2d_mpi_openmp_v2

wave_eq_2d_mpi_openmp: wave_eq_2d_mpi_openmp.c
	mpicc wave_eq_2d_mpi_openmp.c -lm -fopenmp -o wave_eq_2d_mpi_openmp

wave_eq_2d_mpi: wave_eq_2d_mpi.c
	mpicc wave_eq_2d_mpi.c -lm -o wave_eq_2d_mpi

wave_eq_2d_openmp: wave_eq_2d_openmp.c
	gcc wave_eq_2d_openmp.c -lm -fopenmp -o wave_eq_2d_openmp

wave_eq_2d: wave_eq_2d.c
	gcc wave_eq_2d.c -lm -o wave_eq_2d

clean:
	rm wave_eq_2d
	rm wave_eq_2d_openmp
	rm wave_eq_2d_mpi
	rm wave_eq_2d_mpi_openmp
