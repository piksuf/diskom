#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h> // <-- untuk WEXITSTATUS dan WIFEXITED

volatile bool loading = false;

// Spinner animation
void* spinner(void* arg) {
    const char spin_chars[] = "/-\\|";
    int i = 0;
    while (loading) {
        printf("\r%c Loading...", spin_chars[i % 4]);
        fflush(stdout);
        usleep(100000);
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
        }
        return exitCode;
    } else {
        printf("\n[ERROR] Perintah tidak dieksekusi dengan benar.\n");
        return -1;
    }
}

// Fungsi-fungsi perintah
int perintahSatu() {
    return jalankanPerintah("sudo apt update -y", true, false);
}

int perintahDua() {
    return jalankanPerintah("sudo apt install nmap -y > /dev/null 2>&1", true, true);
}

int perintahTiga() {
    printf("Menjalankan perintah 3...\n");
    return jalankanPerintah("sudo apt install npm -y > /dev/null 2>&1", false, true);
}

int main() {
    if (perintahSatu() != 0) {
    }
    if (perintahDua() != 0) {
    }
    if (perintahTiga() != 0) {
    }

    return 0;
}
