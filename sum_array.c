#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define MAX_SIZE 100000

#define ITERATIONS 80

// Tags
#define SEND_DATA_TAG 2001
#define RETURN_DATA_TAG 2002

// Starting with 1000 numbers
#define VALUES 1000

// Master Array
int master_array[MAX_SIZE];

// Slave Array
int slave_array[MAX_SIZE];

int main(int argc, char *argv[]) {

    double start, time;

    long int total, slave_total;
    MPI_Status status;

    int ierr, processes;
    int process_id, slave_id, values_to_obtain, sender, values_obtained, values_to_send;
    // int start_value, end_value;

    // Init
    ierr = MPI_Init(0, 0);

    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &processes);

    int master = 0;

    // Values per array
    int values = VALUES;

    // Iterations
    for(int i = 0; i < ITERATIONS; i++) {
        char file[200];
        snprintf(file, sizeof(file), "files/sum_array_%02d.txt", i);
        for(int j = 1; j < 8; j++) {

            // Master
            if(process_id == master) {
                start = omp_get_wtime();

                // Filling array with numbers
                for(int k = 0; k < values * j; k++) {
                    master_array[k] = k + 1;
                    slave_array[k] = k + 1;
                }
                values_to_send = values * j;

                // Arrays distribution
                for(slave_id = 1; slave_id < processes; slave_id++) {
                    ierr = MPI_Send(&values_to_send, 1, MPI_INT, slave_id, SEND_DATA_TAG, MPI_COMM_WORLD);
                    ierr = MPI_Send(&slave_array[0], values_to_send, MPI_INT, slave_id, SEND_DATA_TAG, MPI_COMM_WORLD);
                }
                // master array total
                total = 0;

                for(int i = 0; i < values  * j; i++) {
                    total += master_array[i];
                }
                
                printf("Total of master_array: %li\n", total);

                // slave array total
                for(slave_id = 1; slave_id < processes; slave_id++) {
                    
                    ierr = MPI_Recv(&slave_total, 1, MPI_LONG, MPI_ANY_SOURCE, RETURN_DATA_TAG, MPI_COMM_WORLD, &status);
                    sender = status.MPI_SOURCE;
                    printf("Total slave_array: %li Sender: %i\n", slave_total, sender);
                    total += slave_total;
                }
                // Finish time
                time = omp_get_wtime() - start;

                printf("computed value = %li in %3.15f seconds\n", total, time);
                FILE *filePtr = fopen(file, "a");

                if (filePtr) {
                    fprintf(filePtr, "numbers = %d computed value = %ld in %3.15f seconds\n", values * j * processes, total, time);
                    fclose(filePtr);
                    printf("Execution %d completed. Results stored in %s\n", i, file);
                } else {
                    printf("Error opening file %s for writing.\n", file);
                }
                
            } else {
                // Obtaining the num of values from master
                ierr = MPI_Recv(&values_to_obtain, 1, MPI_INT, master, SEND_DATA_TAG, MPI_COMM_WORLD, &status);
                ierr = MPI_Recv(&slave_array, values_to_obtain, MPI_INT, master, SEND_DATA_TAG, MPI_COMM_WORLD, &status);

                values_obtained = values_to_obtain;
                slave_total = 0;
                
                // Getting total value fomr slave_array
                for (int i = 0; i < values_obtained; i++) {

                    slave_total += slave_array[i];
                }

                printf("\n");

                ierr = MPI_Send(&slave_total, 1, MPI_LONG, master, RETURN_DATA_TAG, MPI_COMM_WORLD);
            }
        }
    }
    ierr = MPI_Finalize();
    return 0;

}