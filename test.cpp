#include <stdio.h>
#include <string>
#include <string.h>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define DEVICE "/dev/demo"

void control_led(int led_number, const std::string& state) {
    int fd = open(DEVICE, O_WRONLY);
    if (fd < 0) {
        perror("Failed to open the device");
        return;
    }

    char buf[10];
    if (state == "on") {
        snprintf(buf, sizeof(buf), "%d 1", led_number);
    } else if (state == "off") {
        snprintf(buf, sizeof(buf), "%d 0", led_number);
    } else {
        snprintf(buf, sizeof(buf), "%d 3", led_number);
    }
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if(bytes_read){
        printf("a/n");
    }
    ioctl(fd, 0, 0);
    printf("Writing to device: %s\n", buf);
    write(fd, buf, strlen(buf) + 1);
    close(fd);
}

int main(int argc, char **argv) {

    std::map<std::string, int> ledMap = {
        {"LED1", 466},
        {"LED2", 397},
        {"LED3", 255},
        {"LED4", 429}
    };

    std::string led_name = argv[1];
    if (ledMap.find(led_name) == ledMap.end()) {

        printf("Invalid LED name\n");
        return -1;
    }

    int led_number = ledMap[led_name];
    std::string state = argv[2];

    control_led(led_number, state);
    return 0;
}