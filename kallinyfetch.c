#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/statvfs.h>

#define COLOR_RESET "\033[0m"
#define COLOR_BLUE "\033[1;34m"
#define COLOR_CYAN "\033[1;36m"
#define COLOR_GREEN "\033[1;32m"

// ASCII
void print_ascii_art() {
    printf(COLOR_CYAN);
    printf("    ██╗  ██╗ █████╗ ██╗     ██╗     ██╗███╗   ██╗██╗   ██╗\n");
    printf("    ██║ ██╔╝██╔══██╗██║     ██║     ██║████╗  ██║╚██╗ ██╔╝\n");
    printf("    █████╔╝ ███████║██║     ██║     ██║██╔██╗ ██║ ╚████╔╝ \n");
    printf("    ██╔═██╗ ██╔══██║██║     ██║     ██║██║╚██╗██║  ╚██╔╝  \n");
    printf("    ██║  ██╗██║  ██║███████╗███████╗██║██║ ╚████║   ██║   \n");
    printf("    ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝╚═╝╚═╝  ╚═══╝   ╚═╝   \n");
    printf(COLOR_RESET);
}

// username
void get_username() {
    char *username = getenv("USER");
    printf(COLOR_BLUE "User:\033[0m %s\n", username ? username : "Unknown");
}

// hostname
void get_hostname() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf(COLOR_BLUE "Hostname:\033[0m %s\n", hostname);
    } else {
        printf(COLOR_BLUE "Hostname:\033[0m Unknown\n");
    }
}

// information about operating system and etc.
void get_os_info() {
    FILE *file = fopen("/etc/os-release", "r");
    if (!file) {
        printf(COLOR_BLUE "OS:\033[0m Unknown\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "PRETTY_NAME=", 12) == 0) {
            char *os_name = strchr(line, '=') + 1;
            os_name[strcspn(os_name, "\n")] = '\0';
            if (os_name[0] == '"') {
                os_name++;
                os_name[strlen(os_name) - 1] = '\0';
            }
            printf(COLOR_BLUE "OS:\033[0m %s\n", os_name);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf(COLOR_BLUE "OS:\033[0m Unknown\n");
}

// kernel version
void get_kernel_version() {
    struct utsname kernel_info;
    if (uname(&kernel_info) == 0) {
        printf(COLOR_BLUE "Kernel:\033[0m %s\n", kernel_info.release);
    } else {
        printf(COLOR_BLUE "Kernel:\033[0m Unknown\n");
    }
}

// information about cpu
void get_cpu_info() {
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (!file) {
        printf(COLOR_BLUE "CPU:\033[0m Unknown\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "model name", 10) == 0) {
            char *cpu_name = strchr(line, ':') + 2;
            cpu_name[strcspn(cpu_name, "\n")] = '\0';
            printf(COLOR_BLUE "CPU:\033[0m %s\n", cpu_name);
            fclose(file);
            return;
        }
    }

    fclose(file);
    printf(COLOR_BLUE "CPU:\033[0m Unknown\n");
}

// information about memory
void get_memory_info() {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        printf(COLOR_BLUE "Memory:\033[0m Unknown\n");
        return;
    }

    char line[256];
    unsigned long total_memory = 0, available_memory = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %lu kB", &total_memory) == 1) {
            total_memory /= 1024;
        }
        if (sscanf(line, "MemAvailable: %lu kB", &available_memory) == 1) {
            available_memory /= 1024;
            break;
        }
    }

    fclose(file);

    if (total_memory > 0 && available_memory > 0) {
        printf(COLOR_BLUE "Memory:\033[0m %lu MB / %lu MB\n", available_memory, total_memory);
    } else {
        printf(COLOR_BLUE "Memory:\033[0m Unknown\n");
    }
}

// available disk memory
void get_disk_info() {
    struct statvfs stat;
    if (statvfs("/", &stat) == 0) {
        unsigned long total = stat.f_blocks * stat.f_frsize / (1024 * 1024);
        unsigned long available = stat.f_bavail * stat.f_frsize / (1024 * 1024);
        printf(COLOR_BLUE "Disk:\033[0m %lu MB / %lu MB\n", available, total);
    } else {
        printf(COLOR_BLUE "Disk:\033[0m Unknown\n");
    }
}

// videocard info
void get_gpu_info() {
    FILE *file = fopen("/proc/driver/nvidia/version", "r");
    if (file) {
        char line[256];
        fgets(line, sizeof(line), file);
        printf(COLOR_BLUE "GPU:\033[0m %s", line);
        fclose(file);
        return;
    }

    // alternative method lspci
    file = popen("lspci | grep -i vga | cut -d ':' -f3", "r");
    if (file) {
        char gpu_info[256];
        if (fgets(gpu_info, sizeof(gpu_info), file)) {
            printf(COLOR_BLUE "GPU:\033[0m %s", gpu_info);
        } else {
            printf(COLOR_BLUE "GPU:\033[0m Unknown\n");
        }
        pclose(file);
    }
}

int main() {
    print_ascii_art();
    get_username();
    get_hostname();
    get_os_info();
    get_kernel_version();
    get_cpu_info();
    get_memory_info();
    get_disk_info();
    get_gpu_info();
    return 0;
}
