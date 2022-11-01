#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

#define PI 3.14159265359

typedef struct Point
{float x, y;} Point;

Point generateRandomPoint();
int isPointHit(Point p);

int main(int argc, char **argv)
{
    srand(time(NULL));
    int size, rank, 
    count = 0;
    long long i, points_to_rank, number_of_points = 10000000;
    Point* p = (Point*)malloc(sizeof(Point)*number_of_points);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //creating own MPI datatype
    //------------------------------
    int          blocklengths[2] = {1,1};
    MPI_Datatype types[2] = {MPI_FLOAT, MPI_FLOAT};
    MPI_Datatype mpi_point_type;
    MPI_Aint     offsets[2];

    offsets[0] = offsetof(Point, x);
    offsets[1] = offsetof(Point, y);

    MPI_Type_create_struct
    (2, blocklengths, offsets, types, &mpi_point_type);
    MPI_Type_commit(&mpi_point_type);
    //------------------------------

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if(rank == 0)
    {
        for(i=0; i<number_of_points; i++)
            p[i] = generateRandomPoint();
        points_to_rank = number_of_points;
    }
    else
    {
        points_to_rank = number_of_points / size;
        points_to_rank *= rank;
    }
    
    MPI_Bcast(p, number_of_points, mpi_point_type, 0, MPI_COMM_WORLD);

    double starttime, endtime, piMC;

    //all processes start simultaneously
    MPI_Barrier(MPI_COMM_WORLD);
    //---------------------------------
    starttime = MPI_Wtime();

    for(i=0; i<points_to_rank; i++)
        count += isPointHit(p[i]);
    piMC = 4.0 * count / points_to_rank;

    endtime   = MPI_Wtime();
    //---------------------------------

    printf("Rank: %d  Points: %llu \tTime: %lf \tpi: %lf \tError: %lf\n",
    rank, points_to_rank, endtime-starttime, piMC, fabs(piMC - PI));

    free(p);
    MPI_Finalize();
}

Point generateRandomPoint()
{
    Point p;
    float temp = (float)rand();
    p.x = (float)rand() / RAND_MAX;
    p.y = (float)rand() / RAND_MAX;
    return p;
}

int isPointHit(Point p)
{return p.x*p.x+p.y*p.y <= 1;}