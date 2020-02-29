/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This file is part of the Monolinux project.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <curl/curl.h>
#include <heatshrink_encoder.h>
#include <heatshrink_decoder.h>
#include <lzma.h>
#include <detools.h>
#include "async_main.h"
#include "ml/ml.h"

extern int command_lzma_compress(int argc, const char *argv[]);

static void insert_modules(void)
{
    int res;
    int i;
    const char *modules[] = {
        "/root/jbd2.ko",
        "/root/mbcache.ko",
        "/root/ext4.ko"
    };

    printf("================= insmod begin =================\n");

    for (i = 0; i < membersof(modules); i++) {
        res = ml_insert_module(modules[i], "");

        if (res == 0) {
            printf("Successfully inserted '%s'.\n", modules[i]);
        } else {
            printf("Failed to insert '%s'.\n", modules[i]);
        }
    }

    printf("Loaded modules:\n");
    ml_print_file("/proc/modules");

    printf("=================== insmod end =================\n\n");
}

static size_t on_write(void *buf_p, size_t size, size_t nmemb, void *arg_p)
{
    (void)arg_p;

    fwrite(buf_p, size, nmemb, stdout);

    return (size * nmemb);
}

static void http_get(const char *url_p)
{
    CURL *curl_p;
    long response_code;
    int res;

    printf(">>> HTTP GET %s. >>>\n", url_p);

    curl_p = curl_easy_init();

    if (curl_p) {
        curl_easy_setopt(curl_p, CURLOPT_URL, url_p);
        curl_easy_setopt(curl_p, CURLOPT_WRITEFUNCTION, on_write);

        /* WARNING: Makes the connection insecure! */
        curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl_p, CURLOPT_SSL_VERIFYHOST, 0);

        res = curl_easy_perform(curl_p);

        if (res == CURLE_OK) {
            curl_easy_getinfo(curl_p, CURLINFO_RESPONSE_CODE, &response_code);
            printf("<<< HTTP GET response code %ld. <<<\n", response_code);
        } else {
            printf("<<< HTTP GET CURL error code %d: %s. <<<\n",
                   res,
                   curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl_p);
    }
}

static void http_get_main(void *arg_p)
{
    http_get(arg_p);
    free(arg_p);
}

static int command_http_get(int argc, const char *argv[])
{
    if (argc != 2) {
        printf("http_get <url>\n");

        return (-1);
    }

    ml_spawn(http_get_main, strdup(argv[1]));

    return (0);
}

static void init(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    ml_init();
    ml_shell_init();
    ml_network_init();
    ml_shell_register_command("lzmac",
                              "LZMA compress.",
                              command_lzma_compress);
    ml_shell_register_command("http_get",
                              "HTTP GET.",
                              command_http_get);
    ml_shell_start();

    ml_mount("none", "/proc", "proc", 0);
    ml_mount("none", "/sys", "sysfs", 0);
    ml_mount("none", "/sys/kernel/debug", "debugfs", 0);
    insert_modules();

    ml_mknod("/dev/sda1", S_IFBLK | 0666, makedev(8, 1));
    ml_mknod("/dev/sdb1", S_IFBLK | 0666, makedev(8, 16));
    ml_mknod("/dev/sdc1", S_IFBLK | 0666, makedev(8, 32));
    ml_mknod("/dev/mapper/control", S_IFCHR | 0666, makedev(10, 236));
    ml_mknod("/dev/urandom", S_IFCHR | 0644, makedev(1, 9));

    ml_device_mapper_verity_create(
        "erik",
        "00000000-1111-2222-3333-444444444444",
        "/dev/sdb1",
        8192,
        "/dev/sdc1",
        0,
        "af4f26725d8ce706744b54d313ba47ab3be890b76c592ede8aca52779f4e93c9",
        "7891234871263971625789623497586239875698273465987234658792364598");

    ml_mount("/dev/sda1", "/mnt/disk1", "ext4", 0);
    ml_mount("/dev/mapper/00000000-1111-2222-3333-444444444444",
             "/mnt/disk2",
             "squashfs",
             MS_RDONLY);
}

static void print_banner(void)
{
    int fd;
    char buf[128];
    ssize_t size;

    fd = open("/proc/uptime", O_RDONLY);

    if (fd < 0) {
        perror("error: ");

        return;
    }

    size = read(fd, &buf[0], sizeof(buf) - 1);

    if (size >= 0) {
        buf[size] = '\0';
        strtok(&buf[0], " ");
    } else {
        strcpy(&buf[0], "-");
    }

    close(fd);

    printf("Welcome to Monolinux!\n"
           "\n"
           "Uptime: %s\n"
           "\n",
           &buf[0]);
}

static void disk_test(void)
{
    printf("================ disk test begin ===============\n");
    ml_print_file_systems_space_usage();
    ml_print_file("/mnt/disk1/README");
    printf("================= disk test end ================\n\n");
}

static void heatshrink_test(void)
{
    printf("============= heatshrink test begin ============\n");

    printf("Heatshrink encode and decode.\n");

    (void)heatshrink_encoder_alloc(8, 4);
    (void)heatshrink_decoder_alloc(512, 8, 4);

    printf("============= heatshrink test end ==============\n\n");
}

static void lzma_test(void)
{
    lzma_ret ret;
    lzma_stream stream;

    printf("================ lzma test begin ===============\n");

    memset(&stream, 0, sizeof(stream));

    ret = lzma_alone_decoder(&stream, UINT64_MAX);

    if (ret != LZMA_OK) {
        printf("LZMA decoder init failed.\n");
    } else {
        printf("LZMA decoder init successful.\n");
    }

    printf("================= lzma test end ================\n\n");
}

static void detools_test(void)
{
    int res;
    const char from[] = "/mnt/disk1/detools/v1.txt";
    const char patch[] = "/mnt/disk1/detools/v1-v2.patch";
    const char to[] = "/mnt/disk1/detools/v2.txt";

    printf("============== detools test begin ==============\n");
    printf("Library version: %s\n", DETOOLS_VERSION);
    printf("From:  %s\n", from);
    printf("Patch: %s\n", patch);
    printf("To:    %s\n", to);

    res = detools_apply_patch_filenames(from, patch, to);

    if (res >= 0) {
        printf("Before patching:\n");
        ml_print_file(from);
        printf("After patching:\n");
        ml_print_file(to);
        printf("detools: OK!\n");
    } else {
        res *= -1;
        printf("error: detools: %s (error code %d)\n",
               detools_error_as_string(res),
               res);
    }

    printf("=============== detools test end ===============\n\n");
}

static void rtc_test(void)
{
    struct tm tm;

    printf("================ RTC test begin ================\n");
    printf("RTC sysfs date: ");
    ml_print_file("/sys/class/rtc/rtc0/date");
    printf("RTC sysfs time: ");
    ml_print_file("/sys/class/rtc/rtc0/time");
    ml_mknod("/dev/rtc0", S_IFCHR | 0666, makedev(254, 0));
    ml_rtc_get_time("/dev/rtc0", &tm);
    printf("RTC date and time: %s", asctime(&tm));
    tm.tm_year++;
    ml_rtc_set_time("/dev/rtc0", &tm);
    ml_rtc_get_time("/dev/rtc0", &tm);
    printf("RTC date and time: %s", asctime(&tm));
    printf("================= RTC test end =================\n\n");
}

static void http_test(void)
{
    printf("================ http test begin ===============\n");
    http_get("http://10.0.2.2:8001/");
    http_get("https://10.0.2.2:4443/");
    printf("================= http test end ================\n\n");
}

static void ntp_client_test(void)
{
    int res;

    printf("================ ntp client test begin ===============\n");
    res = ml_ntp_client_sync("0.se.pool.ntp.org");

    if (res != 0) {
        printf("NTP client failed!\n");
    }

    printf("================= ntp client test end ================\n\n");
}

static void log_object_test(void)
{
    struct ml_log_object_t log_object;

    printf("============= log object test begin ============\n");
    ml_log_object_init(&log_object, "foo", ML_LOG_ALL);
    ml_log_object_print(&log_object, ML_LOG_EMERGENCY, "Emergency level!");
    ml_log_object_print(&log_object, ML_LOG_INFO, "Info level!");
    ml_log_object_print(&log_object, ML_LOG_DEBUG, "Debug level!");
    printf("============== log object test end =============\n\n");
}

int main()
{
    init();

    print_banner();
    disk_test();
    heatshrink_test();
    lzma_test();
    detools_test();
    ml_network_interface_up("eth0");
    rtc_test();

# if 1
    ml_network_interface_configure("eth0", "10.0.2.15", "255.255.255.0");
    ml_network_interface_add_route("eth0", "10.0.2.2");
#else
    struct ml_dhcp_client_t dhcp_client;

    ml_dhcp_client_init(&dhcp_client, "eth0", ML_LOG_UPTO(INFO));
    ml_dhcp_client_start(&dhcp_client);
#endif

    http_test();
    ntp_client_test();
    log_object_test();

    return (async_main());
}
