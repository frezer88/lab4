#include <mpi.h>
#include <iostream>
#include <ctime>

using namespace std;

const int size = 4;

void matrixRandom(int matrix[size][size]) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = i + j;
        }
    }
}

void printMatrix(const char* label, int matrix[size][size]) {
    cout << label << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}

// Функция умножения блоков матриц
void multiplyMatrixBlocks(const int matrixA[size][size], const int matrixB[size][size], int resultMatrixC[size][size], int blockSize, int rank) {

    cout << "Выполняет: " << rank << "\n";


    for (int i = 0; i < blockSize; i++) {
        for (int j = 0; j < size; j++) {
            resultMatrixC[i][j] = 0;
            for (int k = 0; k < size; k++) {
                resultMatrixC[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }

    cout << endl;

}

int main(int argc, char *argv[]) {
    int rank, numProc;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int matrixA[size][size], matrixB[size][size], resultMatrixC[size][size];
    int blockSize = size / numProc;
    int subMatrixA[blockSize][size], subMatrixC[blockSize][size];

    if (rank == 0) {
        srand(time(NULL));
        matrixRandom(matrixA);
        matrixRandom(matrixB);

        printMatrix("Matrix matrixA:", matrixA);
        printMatrix("Matrix matrixB:", matrixB);
    }

    MPI_Scatter(matrixA, blockSize * size, MPI_INT, subMatrixA, blockSize * size, MPI_INT, 0, MPI_COMM_WORLD);// распределения строк матрицы A между процессами
    MPI_Bcast(matrixB, size * size, MPI_INT, 0, MPI_COMM_WORLD);//передается всем процессам матрица B

    multiplyMatrixBlocks(subMatrixA, matrixB, subMatrixC, blockSize, rank);

    MPI_Gather(subMatrixC, blockSize * size, MPI_INT, resultMatrixC, blockSize * size, MPI_INT, 0, MPI_COMM_WORLD);//все собираю в финальную матрицу

    if (rank == 0) {
        printMatrix("Result Matrix resultMatrixC:", resultMatrixC);
    }

    MPI_Finalize();
    return 0;
}

