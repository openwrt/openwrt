#include <linux/atm.h>
#include <linux/proc_fs.h>

#include "proc.h"
#include "common.h"

struct proc_dir_entry *ppe_proc_dir;

int proc_read_idle_counter(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    len += sprintf(page + off,       "Channel 0\n");
    len += sprintf(page + off + len, "  TX\n");
    len += sprintf(page + off + len,
		"    DREG_AT_CELL0       = %d\n", *DREG_AT_CELL0 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AT_IDLE_CNT0   = %d\n", *DREG_AT_IDLE_CNT0 & 0xFFFF);
    len += sprintf(page + off + len, "  RX\n");
    len += sprintf(page + off + len,
		"    DREG_AR_CELL0       = %d\n", *DREG_AR_CELL0 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AR_IDLE_CNT0   = %d\n", *DREG_AR_IDLE_CNT0 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AR_AIIDLE_CNT0 = %d\n", *DREG_AR_AIIDLE_CNT0 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AR_BE_CNT0     = %d\n", *DREG_AR_BE_CNT0 & 0xFFFF);
    len += sprintf(page + off + len, "Channel 1\n");
    len += sprintf(page + off + len, "  TX\n");
    len += sprintf(page + off + len,
		"    DREG_AT_CELL1       = %d\n", *DREG_AT_CELL1 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AT_IDLE_CNT1   = %d\n", *DREG_AT_IDLE_CNT1 & 0xFFFF);
    len += sprintf(page + off + len, "  RX\n");
    len += sprintf(page + off + len,
		"    DREG_AR_CELL1       = %d\n", *DREG_AR_CELL1 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AR_IDLE_CNT1   = %d\n", *DREG_AR_IDLE_CNT1 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AR_AIIDLE_CNT1 = %d\n", *DREG_AR_AIIDLE_CNT1 & 0xFFFF);
    len += sprintf(page + off + len,
		"    DREG_AR_BE_CNT1     = %d\n", *DREG_AR_BE_CNT1 & 0xFFFF);

    return len;
}

int proc_read_stats(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;

    int i, j;
    struct connection *connection;
    struct port *port;
    int base;

    len += sprintf(page + off, "ATM Stats:\n");

    connection = ppe_dev.connection;
    port = ppe_dev.port;
    for ( i = 0; i < ATM_PORT_NUMBER; i++, port++ )
    {
        base = port->connection_base;
        for ( j = 0; j < port->max_connections; j++, base++ )
            if ( (port->connection_table & (1 << j))
                && connection[base].vcc != NULL )
            {
                if ( connection[base].vcc->stats )
                {
                    struct k_atm_aal_stats *stats = connection[base].vcc->stats;

                    len += sprintf(page + off + len, "  VCC %d.%d.%d (stats)\n", i, connection[base].vcc->vpi, connection[base].vcc->vci);
                    len += sprintf(page + off + len, "    rx      = %d\n", stats->rx.counter);
                    len += sprintf(page + off + len, "    rx_err  = %d\n", stats->rx_err.counter);
                    len += sprintf(page + off + len, "    rx_drop = %d\n", stats->rx_drop.counter);
                    len += sprintf(page + off + len, "    tx      = %d\n", stats->tx.counter);
                    len += sprintf(page + off + len, "    tx_err  = %d\n", stats->tx_err.counter);
                }
                else
                    len += sprintf(page + off + len, "  VCC %d.%d.%d\n", i, connection[base].vcc->vpi, connection[base].vcc->vci);
            }
    }

    return len;
}

void proc_file_create(void)
{
    ppe_proc_dir = proc_mkdir("ppe", NULL);
	create_proc_read_entry("idle_counter", 0, ppe_proc_dir, proc_read_idle_counter, NULL);
    create_proc_read_entry("stats", 0, ppe_proc_dir, proc_read_stats, NULL);
}

void proc_file_delete(void)
{
    remove_proc_entry("idle_counter", ppe_proc_dir);
    remove_proc_entry("stats", ppe_proc_dir);
    remove_proc_entry("ppe", NULL);
}
