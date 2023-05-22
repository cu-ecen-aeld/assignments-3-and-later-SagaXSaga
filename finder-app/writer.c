#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

int main(int argc , char* argv[]) {

    openlog(argv[0], LOG_PERROR, LOG_USER);   // Initialize syslog with identity "slog" and specify LOG_DEBUG and LOG_ERR as logging priorities, using LOG_USER facility

    if (argc < 2) {             // Check if the number of command line arguments is less than 3
        syslog(LOG_ERR, "need 2 argument");   // Log an error message using syslog
        exit(1);                // Exit the program with an error code of 1
    }

    FILE* file = fopen(argv[1],"w");  // Open the file provided as the first command line argument in write mode and assign the file pointer
    if (file == NULL) {         // Check if the file couldn't be opened
        syslog(LOG_ERR, "Couldn't open file %s, %m\n", argv[1]);    // Log an error message indicating the failure to open the file
        exit(1);                // Exit the program with an error code of 1
    } else {
        fputs(argv[2], file);   // Write the string provided as the second command line argument to the file using fprintf
        syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);   // Log a debug message indicating the string being written to the file
        fclose(file);           // Close the file
    }
}
