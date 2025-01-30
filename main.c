#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#define PACKET_SIZE 20
#define OUTPUT_FILE "data.dat"
// #define SCALING_FACTOR 32768.0   



int16_t combine_bytes(uint8_t low, uint8_t high) {
    return (int16_t)((high << 8) | low);
}

// Function to process and print a 20-byte packet
void process_data_packet(uint8_t *buffer, int output_fd) {

    int16_t ax_raw = combine_bytes(buffer[2], buffer[3]);
    int16_t ay_raw = combine_bytes(buffer[4], buffer[5]);
    int16_t az_raw = combine_bytes(buffer[6], buffer[7]);
    
    int16_t wx_raw = combine_bytes(buffer[8], buffer[9]);
    int16_t wy_raw = combine_bytes(buffer[10], buffer[11]);
    int16_t wz_raw = combine_bytes(buffer[12], buffer[13]);

    int16_t roll_raw = combine_bytes(buffer[14], buffer[15]);
    int16_t pitch_raw = combine_bytes(buffer[16], buffer[17]);
    int16_t yaw_raw = combine_bytes(buffer[18], buffer[19]);

    // Convert to double using scale factors
    double ax = ax_raw / (32768.0) * 16;
    double ay = ay_raw / (32768.0) * 16;
    double az = az_raw / (32768.0) * 16;

    double wx = wx_raw / (32768.0) * 2000;
    double wy = wy_raw / (32768.0) * 2000;
    double wz = wz_raw / (32768.0) * 2000;

    double roll = roll_raw / (32768.0) * 180;
    double pitch = pitch_raw / (32768.0) * 180;
    double yaw = yaw_raw / (32768.0) * 180;

    // Print to console
    printf("ax: %lf ay: %lf az: %lf\n", ax, ay, az);
    printf("wx: %lf wy: %lf wz: %lf\n", wx, wy, wz);
    printf("roll: %lf pitch: %lf yaw: %lf\n\n", roll, pitch, yaw);

    // Write binary doubles to the output file
    double data[] = {ax, ay, az, wx, wy, wz, roll, pitch, yaw};
    write(output_fd, data, sizeof(data));
}

// Check for error function
int checkError(int val, const char *msg){
    if (val == -1)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

int main() {
    const char *input_file = "raw.dat";
    
    // Open input file to read
    int input_fd = checkError(open(input_file, O_RDONLY), "Open to read");

    // Open output file, and if it doesn't exist create it, and if it does exist, truncate it
    int output_fd = checkError(open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644),"Open to write");

    uint8_t buffer[PACKET_SIZE];  // Buffer for reading a packet
    ssize_t bytesRead;

    // Read and process packets
    while ((bytesRead = read(input_fd, buffer, PACKET_SIZE)) == PACKET_SIZE) {
        process_data_packet(buffer, output_fd);
    }

    // Close file input and output files
    close(input_fd);
    close(output_fd);

    return EXIT_SUCCESS;
}
