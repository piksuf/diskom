#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

// Fungsi untuk mengabaikan sinyal
void signal_ignore(int sig) {
    (void)sig;
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = signal_ignore;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) < 0 ||
        sigaction(SIGTERM, &sa, NULL) < 0 ||
        sigaction(SIGHUP, &sa, NULL) < 0 ||
        sigaction(SIGQUIT, &sa, NULL) < 0) {
        perror("sigaction failed");
        exit(EXIT_FAILURE);
    }
}

// Variabel untuk menandai status loading
volatile bool loading = false;

// Spinner animation
void* spinner(void* arg) {
    const char spin_chars[] = "/-\\|";
    int i = 0;
    while (loading) {
        printf("\r%c Loading...", spin_chars[i % 4]);
        fflush(stdout);
        usleep(300000);
        i++;
    }
    printf("\r✓ Selesai      \n");
    return NULL;
}

// Bersihkan tampilan perintah dari argumen teknis
void bersihkanPerintah(const char* input, char* output, size_t maxLen) {
    char temp[512];
    strncpy(temp, input, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* token = strtok(temp, " ");
    output[0] = '\0';

    while (token != NULL) {
        if (strcmp(token, "sudo") == 0 ||
            strcmp(token, "-y") == 0 ||
            strcmp(token, ">") == 0 ||
            strcmp(token, "2>&1") == 0 ||
            strstr(token, "/dev/null") != NULL) {
            token = strtok(NULL, " ");
            continue;
        }
        if (strlen(output) + strlen(token) + 2 < maxLen) {
            strcat(output, token);
            strcat(output, " ");
        }
        token = strtok(NULL, " ");
    }

    size_t len = strlen(output);
    if (len > 0 && output[len - 1] == ' ') {
        output[len - 1] = '\0';
    }
}

// Fungsi utama dengan opsi spinner dan tampilkan nama
int jalankanPerintah(const char* cmd, bool tampilkanPerintah, bool pakaiSpinner) {
    if (tampilkanPerintah) {
        char bersih[512];
        bersihkanPerintah(cmd, bersih, sizeof(bersih));
        printf("Menjalankan perintah: %s\n", bersih);
    }

    pthread_t thread_id;

    if (pakaiSpinner) {
        loading = true;
        pthread_create(&thread_id, NULL, spinner, NULL);
    }

    int status = system(cmd);

    if (pakaiSpinner) {
        loading = false;
        pthread_join(thread_id, NULL);
    }

    if (WIFEXITED(status)) {
        int exitCode = WEXITSTATUS(status);
        if (exitCode != 0) {
            printf("\n[ERROR] Perintah gagal dijalankan. Exit code: %d\n", exitCode);
            return -1;
        }
        return 0;
    } else {
        printf("\n[ERROR] Perintah tidak dieksekusi dengan benar.\n");
        return -1;
    }
}

// Fungsi untuk menjalankan perintah pertama
int perintahSatu() {
    int result = jalankanPerintah("sudo apt update -y", true, false);
    if (result == -1) {
        printf("Error: Gagal menjalankan perintah '1'\n");
        return -1;
    }
    return 0;
}

// Fungsi untuk menjalankan perintah kedua
int perintahDua() {
    int result = jalankanPerintah("sudo apt install nmap -y > /dev/null 2>&1", true, true);
    if (result == -1) {
        printf("Error: Gagal menjalankan perintah '2'\n");
        return -1;
    }
    return 0;
}

// Fungsi untuk menjalankan perintah ketiga
int perintahTiga() {
	printf("Menjalankan perintah 3...\n"); //Nama Perintah Yang Di Tampilkan
    int result = jalankanPerintah("sudo apt install npm -y > /dev/null 2>&1", false, true);
    if (result == -1) {
        printf("Error: Gagal menjalankan perintah '3'\n");
        return -1;
    }
    return 0;
}

// Fungsi utama
int main() {
    setup_signal_handlers();
    
    if (perintahSatu() == -1) {
        return -1;
    }

    if (perintahDua() == -1) {
        return -1;
    }

    if (perintahTiga() == -1) {
        return -1;
    }

    return 0;
}
