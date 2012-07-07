/***
 * Monitoring Plugin - check_qnap_disks.c
 **
 *
 * check_qnap_disks - Check the disks of a QNAP NAS by snmp.
 *
 * Copyright (C) 2012 Marius Rieder <marius.rieder@durchmesser.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * $Id$
 */

const char *progname  = "check_qnap_disks";
const char *progdesc  = "Check the disks of a QNAP NAS by snmp.";
const char *progvers  = "0.1";
const char *progcopy  = "2011";
const char *progauth  = "Marius Rieder <marius.rieder@durchmesser.ch>";
const char *progusage = "--hostname <HOSTNAME>";

/* MP Includes */
#include "mp_common.h"
#include "snmp_utils.h"
/* Default Includes */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Library Includes */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>


/* Global Vars */
const char  *hostname = NULL;
int         port = 0;

int main (int argc, char **argv) {
    /* Local Vars */
    int         i;
    char        *output = NULL;
    int         status = STATE_OK;
    struct mp_snmp_table    table_state;
    netsnmp_session         *ss;
    netsnmp_variable_list   *vars, *vars2;

    /* Set signal handling and alarm */
    if (signal(SIGALRM, timeout_alarm_handler) == SIG_ERR)
        critical("Setup SIGALRM trap faild!");

    /* Process check arguments */
    if (process_arguments(argc, argv) != OK)
        unknown("Parsing arguments faild!");

    /* Start plugin timeout */
    alarm(mp_timeout);

    // PLUGIN CODE
    ss = mp_snmp_init();

    /* OIDs to query */
    struct mp_snmp_query_cmd snmpcmd_table = {{1,3,6,1,4,1,24681,1,2,11}, 10, 0, (void *)&table_state};

    status = mp_snmp_table_query(ss, &snmpcmd_table, 7);
    if (status != STAT_SUCCESS) {
        char *string;
        snmp_error(ss, NULL, NULL, &string);
        printf("Error fetching table: %s", string);
    }

    mp_snmp_deinit();

    status = STATE_OK;

    for (i = 0; i<table_state.row; i++) {
        vars = mp_snmp_table_get(table_state, 3, i);

        if (*vars->val.integer == 0) {
            continue;
        }

        vars2 = mp_snmp_table_get(table_state, 1, i);
        char *t;

        switch ((int)*vars->val.integer) {
            case -5:
                t = (char *)malloc(9 + vars2->val_len);
                sprintf(t, "%s missing", vars2->val.string);
                break;
            case -6:
                t = (char *)malloc(9 + vars2->val_len);
                sprintf(t, "%s invalid", vars2->val.string);
                break;
            case -9:
                t = (char *)malloc(11 + vars2->val_len);
                sprintf(t, "%s r/w-error", vars2->val.string);
                break;
            case -4:
            default:
                t = (char *)malloc(9 + vars2->val_len);
                sprintf(t, "%s unknown", vars2->val.string);
                break;
        }

        mp_strcat_comma(&output, t);
        free(t);
        status = STATE_CRITICAL;
    }
    /* Output and return */
    if (status == STATE_OK)
        ok("QNAP: All Disks \"GOOD\"");
    critical("QNAP: %s", output);
}

int process_arguments (int argc, char **argv) {
    int c;
    int option = 0;

    static struct option longopts[] = {
            MP_LONGOPTS_DEFAULT,
            MP_LONGOPTS_HOST,
            MP_LONGOPTS_PORT,
            SNMP_LONGOPTS,
            MP_LONGOPTS_END
    };

    if (argc < 3) {
       print_help();
       exit(STATE_OK);
    }


    while (1) {
        c = mp_getopt(argc, argv, MP_OPTSTR_DEFAULT"H:p:o:O:"SNMP_OPTSTR, longopts, &option);

        if (c == -1 || c == EOF)
            break;

        getopt_snmp(c);

        switch (c) {
            /* Default opts */
            /* Hostname opt */
            case 'H':
                getopt_host(optarg, &hostname);
                break;
            /* Port opt */
            case 'P':
                getopt_port(optarg, &port);
                break;
        }
    }

    return(OK);
}

void print_help (void) {
    print_revision();
    print_revision_snmp();
    print_copyright();

    printf("\n");

    printf("Check description: %s", progdesc);

    printf("\n\n");

    print_usage();
    print_help_default();
    print_help_snmp();
}

/* vim: set ts=4 sw=4 et syn=c : */
