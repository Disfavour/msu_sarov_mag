#include <fstream>
#include <string>
#include <map>
#include <mpi.h>
#include <iostream>

using namespace std;

string update_data(string data, map<char, string>& R)
{
	string buf = "";

	for( unsigned int i=0; i<data.length(); i++ )
		buf += R[data[i]];

	return buf;
}

void run_lsystem(int T, int k)
{
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm comm;

	int dim = 0;
    MPI_Dims_create(size, 1, &dim);

	int periods[2] = {false, false};
	int reorder = true;
    MPI_Cart_create(MPI_COMM_WORLD, 1, &dim, periods, reorder, &comm);
	
    MPI_Comm_rank(comm, &rank);

    int left, right;
    MPI_Cart_shift(comm, 0, 1, &left, &right);

	double* stat_data;
	if (rank == 0)
		stat_data = new double[T/k * (1+size)];

	// аксиома
	string data = rank == size / 2 ? "a" : "";

    // система правил
	map<char, string> R;
	R['a'] = "ab";
	R['b'] = "bc";
	R['c'] = "c";

    // основной цикл
	for (int t = 1; t <= T; t++)
	{
		data = update_data(data,R);

		if (t % k == 0)
		{
			if (rank == 0)
			{
				cout << t << endl;
			}
			// collect data
			double len = data.length();
			MPI_Gather(&len, 1, MPI_DOUBLE, stat_data + (t/k - 1) * (1+size) + 1, 1, MPI_DOUBLE, 0, comm);

			if (rank == 0)
			{
				stat_data[(t/k - 1) * (1+size)] = t;
				double s = 0;
				for (int i = (t/k - 1) * (1+size) + 1; i < (t/k - 1) * (1+size) + 1 + size; i++)
				{
					s += stat_data[i];
				}
				
				for (int i = (t/k - 1) * (1+size) + 1; i < (t/k - 1) * (1+size) + 1 + size; i++)
				{
					stat_data[i] /= s;
				}
			}

			int
				l = data.length(),
				l_left = 0,
				l_right = 0,
				c,
				count;
			MPI_Status status;
			string str_to_send;
			char* receive_buf;
			
			MPI_Sendrecv(&l, 1, MPI_INT, left, 123, &l_right, 1, MPI_INT, right, 123, comm, MPI_STATUS_IGNORE);

			c = (l - l_right) / 2 > 0 ? (l - l_right) / 2 : 0;
			if (c > 0 && rank != size - 1) // dont send from last process to right
			{
				str_to_send = data.substr(l - c);
				data = data.substr(0, l - c);
			}
			
			MPI_Probe(left, MPI_ANY_TAG, comm, &status);
			MPI_Get_count(&status, MPI_CHAR, &count);
			receive_buf = new char[count];

			MPI_Sendrecv(str_to_send.c_str(), str_to_send.length() + 1, MPI_CHAR, right, 123, receive_buf, count, MPI_CHAR, left, 123, comm, MPI_STATUS_IGNORE);

			if (count > 1)
				data = string(receive_buf) + data;

			delete[] receive_buf;

			// to left
			str_to_send = "";
			l = data.length();
			MPI_Sendrecv(&l, 1, MPI_INT, right, 123, &l_left, 1, MPI_INT, left, 123, comm, MPI_STATUS_IGNORE);

			c = (l - l_left) / 2 > 0 ? (l - l_left) / 2 : 0;
			if (c > 0 && rank != 0) // dont send from first process to left
			{
				str_to_send = data.substr(0, c);
				data = data.substr(c);
			}

			MPI_Probe(right, MPI_ANY_TAG, comm, &status);
			MPI_Get_count(&status, MPI_CHAR, &count);
			receive_buf = new char[count];

			MPI_Sendrecv(str_to_send.c_str(), str_to_send.length() + 1, MPI_CHAR, left, 123, receive_buf, count, MPI_CHAR, right, 123, comm, MPI_STATUS_IGNORE);

			if (count > 1)
				data =  data + string(receive_buf);

			delete[] receive_buf;
		}
	}

	if (rank > 0)
	{
		MPI_Send(data.c_str(), data.length() + 1, MPI_CHAR, 0, 123, comm);
	}
	else
	{
		for (int i = 1; i < size; i++)
		{
			int count;
			MPI_Status status;
			MPI_Probe(i, MPI_ANY_TAG, comm, &status);
			MPI_Get_count(&status, MPI_CHAR, &count);

			char* part_of_data = new char[count];

			MPI_Recv(part_of_data, count, MPI_CHAR, i, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);

			data += string(part_of_data);

			delete[] part_of_data;
		}

		// сохранение результатов расчета
		ofstream f("output.dat");
		f << data << endl;
		f.close();

		ofstream f2("stat.dat");
		for (int i = 0; i < T/k; i++)
		{
			for (int j = 0; j < size + 1; j++)
			{
				f2 << stat_data[i*(size + 1) + j] << " ";
			}
			f2 << endl;
		}
		f2.close();

		ofstream f3("data_for_plots1.dat", ios::app);
		for (int i = 0; i < T/k; i++)
		{
			for (int j = 0; j < size + 1; j++)
			{
				f3 << stat_data[i*(size + 1) + j] << " ";
			}
			f3 << endl;
		}
		f3.close();

		delete[] stat_data;
	}
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int T = atoi(argv[1]); // число итераций алгоритма
	int k = atoi(argv[2]); // шаг обмена

	run_lsystem(T, k);

	MPI_Finalize();

	return 0;
}

