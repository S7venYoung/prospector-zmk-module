#include "scanner_host_status.h"

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <errno.h>
#include <zephyr/sys/atomic.h>
#include <stdlib.h>
#include <string.h>

#define HOST_LINE_MAX 64

static const struct device *const host_uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
static struct scanner_host_status status;
static atomic_t changed;
static char line[HOST_LINE_MAX];
static size_t line_length;

const struct scanner_host_status *scanner_host_status_get(void) { return &status; }
bool scanner_host_status_take_changed(void) { return atomic_cas(&changed, 1, 0); }

static void reply(const char *text) {
    uart_fifo_fill(host_uart, (const uint8_t *)text, strlen(text));
}

static void process_line(void) {
    if (strcmp(line, "PING") == 0) {
        reply("PROSPECTOR-SCANNER/1\n");
        return;
    }
    if (strncmp(line, "CODEX ", 6) == 0) {
        char *end = NULL;
        unsigned long left = strtoul(line + 6, &end, 10);
        if (end == line + 6 || *end != ' ' || left > 100U) { reply("ERR\n"); return; }
        unsigned long tokens = strtoul(end + 1, &end, 10);
        bool week_available = false;
        unsigned long week_left = 0;
        if (*end == ' ') {
            week_left = strtoul(end + 1, &end, 10);
            if (week_left > 100U) { reply("ERR\n"); return; }
            week_available = true;
        }
        if (*end != '\0') { reply("ERR\n"); return; }
        status.codex_left_percent = (uint8_t)left;
        status.codex_total_tokens = (uint32_t)MIN(tokens, UINT32_MAX);
        status.codex_week_left_percent = (uint8_t)week_left;
        status.codex_week_available = week_available;
        status.codex_available = true;
        atomic_set(&changed, 1);
        reply("OK\n");
        return;
    }
    reply("ERR\n");
}

static void host_uart_callback(const struct device *dev, void *user_data) {
    ARG_UNUSED(user_data);
    while (uart_irq_update(dev) && uart_irq_rx_ready(dev)) {
        uint8_t byte;
        if (uart_fifo_read(dev, &byte, 1) != 1) { break; }
        if (byte == '\r') { continue; }
        if (byte == '\n') {
            line[line_length] = '\0';
            process_line();
            line_length = 0;
        } else if (line_length + 1 < sizeof(line)) {
            line[line_length++] = (char)byte;
        } else {
            line_length = 0;
        }
    }
}

static int scanner_host_status_init(void) {
    if (!device_is_ready(host_uart)) { return -ENODEV; }
    uart_irq_callback_user_data_set(host_uart, host_uart_callback, NULL);
    uart_irq_rx_enable(host_uart);
    return 0;
}

SYS_INIT(scanner_host_status_init, APPLICATION, 90);
