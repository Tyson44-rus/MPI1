#include <iostream>
#include <time.h>
#include "mpi.h"
#include <stdio.h>
#include "Circle.h"
#include <Windows.h>

using namespace std;

void searchMinMax(Circle* mas, int n, int& maxX, int& maxY, int& minX, int& minY, int world_rank) {

	int world_rank2 = world_rank;
	world_rank2++;

	for (int i = world_rank * n / 4; i < world_rank2 * n / 4; i++) {
		mas[i].maxX = mas[i].point1.x + mas[i].R;
		mas[i].minX = mas[i].point1.x - mas[i].R;
		mas[i].maxY = mas[i].point1.y + mas[i].R;
		mas[i].minY = mas[i].point1.y - mas[i].R;
	}

	minX = mas[world_rank * n / 4].minX;
	minY = mas[world_rank * n / 4].minY;
	maxX = mas[world_rank * n / 4].maxX;
	maxY = mas[world_rank * n / 4].maxY;

	for (int i = world_rank * n / 4; i < world_rank2 * n / 4; i++) {
		if (mas[i].maxX > maxX)
			maxX = mas[i].maxX;

		if (mas[i].maxY > maxY)
			maxY = mas[i].maxY;

		if (mas[i].minX < minX)
			minX = mas[i].minX;

		if (mas[i].minY < minY)
			minY = mas[i].minY;
	}
}

int main(int argc, char* argv[]) {
	setlocale(0, "");
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	srand(time(NULL));

	MPI_Init(NULL, NULL);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	int n = atoi(argv[1]);

	Circle* mas = new Circle[n];

	/*if (!world_rank)*/
	for (int i = 0; i < n; i++) {
		new(&mas[i]) Circle;
		mas[i].point1.x = rand() % 100;
		mas[i].point1.y = rand() % 100;
		mas[i].point2.x = rand() % 100;
		mas[i].point2.y = rand() % 100;
		mas[i].R = sqrt(pow(mas[i].point2.x - mas[i].point1.x, 2) +
			pow(mas[i].point2.y - mas[i].point1.y, 2));
	}
		/*}
	else
		for (int i = 0; i < n; i++) {
			new(&mas[i]) Circle;
			mas[i].point1.x = 0;
			mas[i].point1.y = 0;
			mas[i].point2.x = 0;
			mas[i].point2.y = 0;
			mas[i].R = 0;
		}*/

	MPI_Status status;

	/*for (int i = 0; i < n; i++)
		if (!world_rank) {
			MPI_Send(&mas[i], n, MPI_INT, 1, 1, MPI_COMM_WORLD);
			MPI_Send(&mas[i], n, MPI_INT, 2, 1, MPI_COMM_WORLD);
			MPI_Send(&mas[i], n, MPI_INT, 3, 1, MPI_COMM_WORLD);
		}
		else
			MPI_Recv(&mas[i], n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);*/

	double timer = MPI_Wtime();

	int maxX, maxY, minX, minY;

	searchMinMax(mas, n, maxX, maxY, minX, minY, world_rank);

	cout << '\n' << "Time running: " << (double)MPI_Wtime() - timer << " Thread: " << world_rank << endl;

	if (!world_rank) {

		const int N = 4;

		int bufMinX[N];
		int bufMinY[N];
		int bufMaxX[N];
		int bufMaxY[N];

		MPI_Status status;

		bufMinX[0] = minX;
		bufMinY[0] = minY;
		bufMaxX[0] = maxX;
		bufMaxY[0] = maxY;

		MPI_Recv(&bufMinX[1], 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMinY[1], 1, MPI_INT, 1, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMaxX[1], 1, MPI_INT, 1, 3, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMaxY[1], 1, MPI_INT, 1, 4, MPI_COMM_WORLD, &status);

		MPI_Recv(&bufMinX[2], 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMinY[2], 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMaxX[2], 1, MPI_INT, 2, 3, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMaxY[2], 1, MPI_INT, 2, 4, MPI_COMM_WORLD, &status);

		MPI_Recv(&bufMinX[3], 1, MPI_INT, 3, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMinY[3], 1, MPI_INT, 3, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMaxX[3], 1, MPI_INT, 3, 3, MPI_COMM_WORLD, &status);
		MPI_Recv(&bufMaxY[3], 1, MPI_INT, 3, 4, MPI_COMM_WORLD, &status);

		minX = 0, minY = 0, maxX = 0, maxY = 0;

		for (int i = 0; i < N; i++) {
			if (bufMinX[i] < minX)
				minX = bufMinX[i];

			if (bufMinY[i] < minY)
				minY = bufMinY[i];

			if (bufMaxX[i] > maxX)
				maxX = bufMaxX[i];

			if (bufMaxY[i] > maxY)
				maxY = bufMaxY[i];
		}

		cout << endl;
		cout << "minX" << '\t' << "minY" << '\n';
		cout << minX << '\t' << minY << '\n';

		cout << "maxX" << '\t' << "maxY" << '\n';
		cout << maxX << '\t' << maxY << '\n';
	}
	else {
		MPI_Send(&minX, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&minY, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&maxX, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
		MPI_Send(&maxY, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	delete[] mas;
	return 0;
}