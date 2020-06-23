#include "mbed.h"
#include <stdio.h>
#include <errno.h>

#include "SDBlockDevice.h"

// SDBlockDevice blockDevice(MBED_CONF_SD_SPI_MOSI, MBED_CONF_SD_SPI_MISO, MBED_CONF_SD_SPI_CLK, MBED_CONF_SD_SPI_CS);
// SDBlockDevice blockDevice(PC_3, PC_2, PC_7, PB_9);
// 

SDBlockDevice blockDevice(PA_7, PA_6, PA_5, PA_8);

#include "LittleFileSystem.h"
LittleFileSystem fileSystem("fs");
/*
#include "FATFileSystem.h"
FATFileSystem   fileSystem("fs");
*/


int main() {

//    FATFileSystem   fileSystem("fs");
//    LittleFileSystem   fileSystem("fs");

//    uint8_t block[512] = "Hello World!\r\n";

    Serial *pc;
    pc = new Serial(USBTX, USBRX);
    pc->baud(115200);


    // File system declaration
    printf("--- Mbed OS filesystem example ---\r\n");

    // Try to mount the filesystem
    printf("Mounting the filesystem... ");
    fflush(stdout);

    int err = fileSystem.mount(&blockDevice);
    printf("%s\r\n", (err ? "Fail :(" : "OK"));
    if (err) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        printf("No filesystem found, formatting... ");
        fflush(stdout);
        err = fileSystem.reformat(&blockDevice);
        printf("%s\r\n", (err ? "Fail :(" : "OK"));
        if (err) {
            error("error: %s (%d)\r\n", strerror(-err), err);
        }
    }

    // Open the numbers file
    printf("Opening \"/fs/numbers.txt\"... ");

    fflush(stdout);

    FILE*   f = fopen("/fs/numbers.txt", "r+");
    printf("%s\r\n", (!f ? "Fail :(" : "OK"));
    if (!f) {
        // Create the numbers file if it doesn't exist
        printf("No file found, creating a new file... ");
        fflush(stdout);
        f = fopen("/fs/numbers.txt", "w+");
        printf("%s\r\n", (!f ? "Fail :(" : "OK"));
        if (!f) {
            error("error: %s (%d)\r\n", strerror(errno), -errno);
        }

        for (int i = 0; i < 10; i++) {
            printf("\rWriting numbers (%d/%d)... ", i, 10);
            fflush(stdout);
            err = fprintf(f, "    %d\r\n", i);
            if (err < 0) {
                printf("Fail :(\r\n");
                error("error: %s (%d)\r\n", strerror(errno), -errno);
            }
        }

        printf("\rWriting numbers (%d/%d)... OK\r\n", 10, 10);

        printf("Seeking file... ");
        fflush(stdout);
        err = fseek(f, 0, SEEK_SET);
        printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
        if (err < 0) {
            error("error: %s (%d)\r\n", strerror(errno), -errno);
        }
    }

    // Go through and increment the numbers
    for (int i = 0; i < 10; i++) {
        printf("\rIncrementing numbers (%d/%d)... ", i, 10);
        fflush(stdout);

        // Get current stream position
        long    pos = ftell(f);

        // Parse out the number and increment
        int32_t number;
        fscanf(f, "%ld", &number);
        number += 1;

        // Seek to beginning of number
        fseek(f, pos, SEEK_SET);

        // Store number
        fprintf(f, "    %ld\r\n", number);

        // Flush between write and read on same file
        fflush(f);
    }

    printf("\rIncrementing numbers (%d/%d)... OK\r\n", 10, 10);

    // Close the file which also flushes any cached writes
    printf("Closing \"/fs/numbers.txt\"... ");
    fflush(stdout);
    err = fclose(f);
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\r\n", strerror(errno), -errno);
    }

    // Display the root directory
    printf("Opening the root directory... ");
    fflush(stdout);

    DIR*    d = opendir("/fs/");
    printf("%s\r\n", (!d ? "Fail :(" : "OK"));
    if (!d) {
        error("error: %s (%d)\r\n", strerror(errno), -errno);
    }

    printf("root directory:\r\n");
    while (true) {
        struct dirent*  e = readdir(d);
        if (!e) {
            break;
        }

        printf("    %s\r\n", e->d_name);
    }

    printf("Closing the root directory... ");
    fflush(stdout);
    err = closedir(d);
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\r\n", strerror(errno), -errno);
    }

    // Display the numbers file
    printf("Opening \"/fs/numbers.txt\"... ");
    fflush(stdout);
    f = fopen("/fs/numbers.txt", "r");
    printf("%s\r\n", (!f ? "Fail :(" : "OK"));
    if (!f) {
        error("error: %s (%d)\r\n", strerror(errno), -errno);
    }

    printf("numbers:\r\n");
    while (!feof(f)) {
        int c = fgetc(f);
        printf("%c", c);
    }

    printf("\rClosing \"/fs/numbers.txt\"... ");
    fflush(stdout);
    err = fclose(f);
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\r\n", strerror(errno), -errno);
    }

    // Tidy up
    printf("Unmounting... ");
    fflush(stdout);
    err = fileSystem.unmount();
    printf("%s\r\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\r\n", strerror(-err), err);
    }
/*
    printf("Initializing the block device... ");
    fflush(stdout);

    err = blockDevice.init();
    printf("%s\r\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\r\n", strerror(-err), err);
    }

    printf("Erasing the block device... ");
    fflush(stdout);
    err = blockDevice.erase(0, blockDevice.size());
    printf("%s\r\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\r\n", strerror(-err), err);
    }

    printf("Deinitializing the block device... ");
    fflush(stdout);
    err = blockDevice.deinit();
    printf("%s\r\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\r\n", strerror(-err), err);
    }

    printf("\r\r\n");
    */

    printf("Mbed OS filesystem example done!\r\n");
}

