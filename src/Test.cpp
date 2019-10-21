// Author: Wes Kendall
// Copyright 2013 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Program that computes the average of an array of elements in parallel using
// MPI_Reduce.
//
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <time.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: avg num_elements_per_proc\n");
    exit(1);
  }

  int num_elements_per_proc = atoi(argv[1]);

  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Create a random array of elements on all processes.
  srand(time(NULL)*world_rank);   // Seed the random number generator to get different results each time for each processor
  float *rand_nums = NULL;
  rand_nums = create_rand_nums(num_elements_per_proc);

  // Sum the numbers locally
  float localMin = 11111110;
  float localMax = 0;
  int i;
  for (i = 0; i < num_elements_per_proc; i++) {
	  if (localMin > rand_nums[i]) {
		  localMin = rand_nums[i];
	  			}

	  			if (localMax < rand_nums[i]) {
	  				localMax = rand_nums[i];
	  			}
  }

  // Print the random numbers on each process
  printf("Local min for process %d - %f \n",
         world_rank, localMin);
  printf("Local max for process %d - %f \n \n\n\n",
         world_rank, localMax);

  // Reduce all of the local sums into the global sum
  float global_max, global_min;
  MPI_Reduce(&localMin, &global_min, 1, MPI_FLOAT, MPI_MIN, 0,
             MPI_COMM_WORLD);
  MPI_Reduce(&localMax, &global_max, 1, MPI_FLOAT, MPI_MAX, 0,
             MPI_COMM_WORLD);

  // Print the result
  if (world_rank == 0) {
    printf("Total min = %f, max = %f\n", global_min,
           global_max );
  }

  // Clean up
  free(rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
