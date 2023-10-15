check: wave_eq_2d_openmp.out wave_eq_2d_mpi.out
	OMP_NUM_THREADS=16 ./wave_eq_2d_openmp.out 1600 800 800
	mpirun --oversubscribe -n 8 ./wave_eq_2d_mpi.out 1600 800 800

wave_eq_2d_openmp.out: wave_eq_2d_openmp.c
	gcc wave_eq_2d_openmp.c -lm -fopenmp -o wave_eq_2d_openmp.out

wave_eq_2d_mpi.out: wave_eq_2d_mpi.c
	mpicc wave_eq_2d_mpi.c -lm -o wave_eq_2d_mpi.out

clean:
	rm -rf *.out
