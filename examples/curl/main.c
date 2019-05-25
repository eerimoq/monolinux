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

#include "curl/curl.h"
#include <unistd.h>
#include "ml/ml.h"

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

    printf("\n>>> HTTP GET %s. >>>\n", url_p);

    curl_p = curl_easy_init();

    if (curl_p) {
        curl_easy_setopt(curl_p, CURLOPT_URL, url_p);
        curl_easy_setopt(curl_p, CURLOPT_WRITEFUNCTION, on_write);

        res = curl_easy_perform(curl_p);

        if (res == CURLE_OK) {
            curl_easy_getinfo(curl_p, CURLINFO_RESPONSE_CODE, &response_code);
            printf("<<< HTTP GET response code %ld. <<<\n", response_code);
        } else {
            printf("<<< HTTP GET curl error %d. <<<\n", res);
        }

        curl_easy_cleanup(curl_p);
    }
}

static int command_http_get(int argc, const char *argv[])
{
    if (argc != 2) {
        printf("http_get <url>\n");

        return (-1);
    }

    http_get(argv[1]);

    return (0);
}

int main()
{
    ml_init();
    ml_shell_init();
    ml_shell_register_command("http_get", "HTTP GET.", command_http_get);
    ml_network_init();
    ml_shell_start();

    ml_network_interface_configure("eth0", "192.168.0.100", "255.255.255.0");
    ml_network_interface_up("eth0");

    while (true) {
        sleep(10);
    }

    return (0);
}
