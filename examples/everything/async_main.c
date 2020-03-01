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
 * This file is part of the Async project.
 */

#include <stdarg.h>
#include <async.h>
#include "async/runtimes/monolinux.h"
#include "async_main.h"
#include "publisher.h"

static const char server_crt[] = (
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDETCCAfkCFFVRo6tydPDAjBCbuF2lBCRPAhrUMA0GCSqGSIb3DQEBCwUAMEUx\n"
    "CzAJBgNVBAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRl\n"
    "cm5ldCBXaWRnaXRzIFB0eSBMdGQwHhcNMjAwMTAxMDkxNjIxWhcNNDcwNTE5MDkx\n"
    "NjIxWjBFMQswCQYDVQQGEwJBVTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UE\n"
    "CgwYSW50ZXJuZXQgV2lkZ2l0cyBQdHkgTHRkMIIBIjANBgkqhkiG9w0BAQEFAAOC\n"
    "AQ8AMIIBCgKCAQEAt2cIM+Vkpkhwcdb3cE/dxyRU1nCodWKyO/YLSqDKfZXGmLEp\n"
    "FelzuJFA3ZzoCMdEywn1WbzPzWFBLFm6YP95HQQcWT6PQYTs2NmXfsQpU23ECdaf\n"
    "yf/1QJU9YVeZ/rYXeVReFMxPaAmqdaEe6lRgXXO98q1J9NDArNqETE6gajbVKAEE\n"
    "Sj7+MYs6iBWIyXHh9rzOHXApRi+zcf7nzHaeolw3LwOpV4MyuY010Qcaww4J6btD\n"
    "qEPA0zCVLuwgPlMkeKqpwcR5mgv4wW8F4eaNVdT2aDqIbKcCxWT3h/zTfq7fp3oX\n"
    "ryUUvAhHP4h+IKfX8UTQYKBzPJKfCIMdElwidwIDAQABMA0GCSqGSIb3DQEBCwUA\n"
    "A4IBAQAD6YyMDh/SaOV+XZoDTpn8+dgaIlSFj41yIrr0kTvvtOFoQhTbfAA/kfRy\n"
    "uE42i8XDIWw8Euc/mw1ACJtmXJE3KakwtiRHpphZtfWVUR1vmbvinUEbO8Rmi5wI\n"
    "0q/JBJbitDc1l5/U0bc3VIZ2WOqhkEd4En4YjhAUizicSS5a/eScirNVoZwzXdUY\n"
    "B/JepoMH4SkL9qvOjf+fBCrgrlslhZ5pKRPDJUy7Slqk4EIYuyE8YGvLQnPgV4/7\n"
    "l6PymsbuwVsMOvwz6FEdBVR3dNO3pMT667V/rSilLQGnRFpUcHFILyqg35LONFn3\n"
    "cwT/ciBUfMi/giQyFNpB1RdktfHd\n"
    "-----END CERTIFICATE-----\n");

/* static void log_stdout(void *log_object_p, */
/*                        int level, */
/*                        const char *fmt_p, */
/*                        ...) */
/* { */
/*     (void)log_object_p; */
/*     (void)level; */

/*     va_list vlist; */

/*     va_start(vlist, fmt_p); */
/*     printf("DEBUG "); */
/*     vprintf(fmt_p, vlist); */
/*     printf("\n"); */
/*     va_end(vlist); */
/* } */

int async_main()
{
    struct async_t async;
    struct publisher_t publishers[2];
    struct async_ssl_context_t ssl_context;

    async_ssl_module_init();
    async_init(&async);
    /* async_set_log_object_callbacks(&async, log_stdout, NULL); */
    async_set_runtime(&async, async_runtime_monolinux_create());
    publisher_init(&publishers[0],
                   "tcp",
                   1883,
                   NULL,
                   &async);
    async_ssl_context_init(&ssl_context, async_ssl_protocol_tls_v1_0_t);
    async_ssl_context_load_verify_location(&ssl_context, &server_crt[0]);
    publisher_init(&publishers[1],
                   "ssl",
                   8883,
                   &ssl_context,
                   &async);
    async_run_forever(&async);

    return (0);
}
