#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>

#define PACKET_SIZE 8
#define OUTPUT_FILE "data.dat"

#define ACCEL_SCALE 32768.0    // Scaling factor for acceleration
#define GYRO_SCALE 32768.0     // Scaling factor for gyroscope
#define ANGLE_SCALE 32768.0    // Scaling factor for Euler angles

int16_t combine_bytes(uint8_t high, uint8_t low) {
    return (int16_t)((high << 8) | low);
}

// Function to process and print a 20-byte packet
void process_packet(uint8_t *buffer, int output_fd) {
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
    double ax = ax_raw / ACCEL_SCALE;
    double ay = ay_raw / ACCEL_SCALE;
    double az = az_raw / ACCEL_SCALE;

    double wx = wx_raw / GYRO_SCALE;
    double wy = wy_raw / GYRO_SCALE;
    double wz = wz_raw / GYRO_SCALE;

    double roll = roll_raw / ANGLE_SCALE;
    double pitch = pitch_raw / ANGLE_SCALE;
    double yaw = yaw_raw / ANGLE_SCALE;

    // Print to console
    printf("ax:  %lf ay:  %lf az:  %lf\n", ax, ay, az);
    printf("wx:  %lf wy:  %lf wz:  %lf\n", wx, wy, wz);
    printf("roll:  %lf pitch:  %lf yaw:  %lf\n\n", roll, pitch, yaw);

    // Write binary doubles to the output file
    double data[] = {ax, ay, az, wx, wy, wz, roll, pitch, yaw};
    write(output_fd, data, sizeof(data));
}

int main() {
    const char *input_file = "raw.dat";
    
    // Open input file (binary)
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1) {
        perror("Error opening raw.dat");
        return EXIT_FAILURE;
    }

    // Open output file (binary)
    int output_fd = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("Error opening data.dat");
        close(input_fd);
        return EXIT_FAILURE;
    }

    uint8_t buffer[PACKET_SIZE];  // Buffer for reading a packet
    ssize_t bytesRead;

    // Read and process packets
    while ((bytesRead = read(input_fd, buffer, PACKET_SIZE)) == PACKET_SIZE) {
        process_packet(buffer, output_fd);
    }

    if (bytesRead != 0) {
        fprintf(stderr, "Warning: Incomplete data packet at end of file.\n");
    }

    // Close file descriptors
    close(input_fd);
    close(output_fd);

    printf("Processing complete. Data written to %s\n", OUTPUT_FILE);
    return EXIT_SUCCESS;
}

// struct data
// {
//     double ax;
//     double ay;
//     double az;
    
//     double wx;
//     double wy;
//     double wz;

//     double roll;
//     double pitch;
//     double yaw;
// };

// int checkError(int val, const char *msg){
//     if (val == -1)
//     {
//         perror(msg);
//         exit(EXIT_FAILURE);
//     }
//     return val;
// }

// void hex_to_bin(const char *hex, unsigned char *bin, size_t bin_len){
//     for (size_t i = 0; i < bin_len; i++){
//         sscanf(hex+2*i, "%2hhx", &bin[i]);
//     }
// }

// int main( )
// {
//     int fd;
//     int i;
//     struct data valz[20];
//     int count = 20;
//     int bytes = 0;

//     unsigned char buffer[double_size];
//     double value;
//     ssize_t bytesRead;

//     fd = checkError(open("raw.dat",O_RDONLY), "open for read"); 

//     while((bytesRead = read(fd, buffer, double_size))==double_size){
//         memcpy(&value, buffer, double_size);
//         printf("Double %lf]n", value);
//     }
    

    
//     // bytes = checkError(read(fd, valz, sizeof(struct data)*20), "read");

//     // for (i = 0; i< bytes/sizeof(struct data);i++){
//     //     printf("ax: %lf  ay: %lf   az: %lf\n", valz[i].ax, valz[i].ay,valz[i].az);

//     //     printf("wx: %lf  wy: %lf   wz: %lf\n", valz[i].wx, valz[i].wy,valz[i].wz);

//     //     printf("roll: %lf  pitch: %lf   yaw: %lf\n", valz[i].roll, valz[i].pitch,valz[i].yaw);
//     // }

//     close(fd);
//     exit(EXIT_SUCCESS);
// }