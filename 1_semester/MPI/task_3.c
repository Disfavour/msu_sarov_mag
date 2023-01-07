#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define TAG 13423


int main()
{
	MPI_Init(NULL, NULL);

	int size, rank;

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double start_time;

	if (!rank)
	{
		start_time = MPI_Wtime();
	}

	int color = 0;

	if (rank % 2)
	{
		color = 1;
	}

	MPI_Comm New_Comm;

	MPI_Comm_split(MPI_COMM_WORLD, color, rank, &New_Comm);

	int
		new_rank,
		new_size;

	MPI_Comm_size(New_Comm, &new_size);
	MPI_Comm_rank(New_Comm, &new_rank);

	//printf("old %d of %d\tnew %d of %d color %d\n", rank, size, new_rank, new_size, color);

	if (color)
	{
		MPI_Comm cart_comm;
		int dims = 0;

		MPI_Dims_create(new_size, 1, &dims);
		// dims = new_size;

		int periods = 1;

		MPI_Cart_create(New_Comm, 1, &dims, &periods, 1, &cart_comm);

		int
			rank_cart,
			size_cart;

		MPI_Comm_size(cart_comm, &size_cart);
		MPI_Comm_rank(cart_comm, &rank_cart);

		int shift = 1;

		int
			source,
			dest;

		MPI_Cart_shift(cart_comm, 0, shift, &source, &dest);

		int received_rank;
		MPI_Status status;

		MPI_Sendrecv(&rank_cart, 1, MPI_INT, dest, TAG, &received_rank, 1, MPI_INT, source, TAG, cart_comm, &status);

		printf("Cart\tprocess %d/%d recieved %d from process with rank %d\n", rank_cart, size_cart, received_rank, status.MPI_SOURCE);

		MPI_Comm_free(&cart_comm);
	}
	else
	{
		MPI_Comm graph_comm;

		int * index = malloc(new_size * sizeof(int));

		for (int i = 0; i < new_size; ++i)
		{
			index[i] = new_size + i - 1;
		}

		int * edges = malloc(2 * (new_size - 1) * sizeof(int));

		for (int i = 0; i < new_size - 1; ++i)
		{
			edges[i] = i + 1;
			edges[new_size - 1 + i] = 0;
		}

		MPI_Graph_create(New_Comm, new_size, index, edges, 1, &graph_comm);

		int
			rank_graph,
			size_graph;

		MPI_Comm_size(graph_comm, &size_graph);
		MPI_Comm_rank(graph_comm, &rank_graph);

		int neighbors_count;

		MPI_Graph_neighbors_count(graph_comm, rank_graph, &neighbors_count);

		int * neighbors = malloc(neighbors_count * sizeof(int));

		MPI_Graph_neighbors(graph_comm, rank_graph, neighbors_count, neighbors);

		for (int i = 0; i < neighbors_count; ++i)
		{
			MPI_Status status;
			int received_rank;

			MPI_Sendrecv(&rank_graph, 1, MPI_INT, neighbors[i], TAG, &received_rank, 1, MPI_INT, neighbors[i], TAG, graph_comm, &status);

			printf("Graph\tprocess %d/%d recieved %d from process with rank %d\n", rank_graph, size_graph, received_rank, status.MPI_SOURCE);
		}

		MPI_Comm_free(&graph_comm);

		free(neighbors);
		free(index);
		free(edges);
	}

	MPI_Comm_free(&New_Comm);

	if (!rank)
	{
		printf("Elapsed time %.3lf\n", MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	return 0;
}
