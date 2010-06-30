/***
 * Monitoring Plugin - check_file
 **
 *
 * check_file - Check a files property.
 * Copyright (C) 2010 Marius Rieder <marius.rieder@durchmesser.ch>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

const char *progname  = "check_file";
const char *progvers  = "0.1";
const char *progcopy  = "2010";
const char *progauth = "Marius Rieder <marius.rieder@durchmesser.ch>";
const char *progusage = "-f <FILE> -w <warning age> -c <critical age>";

#include "mp_common.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

/* Global vars */
char *filename = NULL;
thresholds *age_thresholds = NULL;
thresholds *size_thresholds = NULL;

int main (int argc, char **argv) {

    int         status;
    int         age_status = -1;
    int         size_status = -1;
    char        *output = NULL;
    struct stat file_stat;

    if (process_arguments (argc, argv) == 1)
        exit(STATE_CRITICAL);

    if (filename == 0)
        usage("Filename is mandatory.");

    status = lstat(filename, &file_stat);

    if (status != 0)
        critical("Stat '%s' faild.", filename);

    if (mp_verbose > 0) {
        printf("Stat for %s\n", filename);
        printf(" UID:    %d\n", (int) file_stat.st_uid);
        printf(" GID:    %d\n", (int) file_stat.st_gid);
        printf(" ATIME:  %u\n", (unsigned int) file_stat.st_atime);
        printf(" MTIME:  %u\n", (unsigned int) file_stat.st_mtime);
        printf(" CTIME:  %u\n", (unsigned int) file_stat.st_ctime);
        printf(" Size:   %u\n", (unsigned int)(unsigned int)  file_stat.st_size);
        print_thresholds("Age",age_thresholds);
        print_thresholds("Size", size_thresholds);
    }

    if (age_thresholds != NULL) {
        age_status = get_status((time(0) - file_stat.st_mtime), age_thresholds);

        switch (age_status) {
            case STATE_WARNING:
                output = malloc(sizeof(char) * 12);
                strcat(output, "age warning");
                break;
            case STATE_CRITICAL:
                output = malloc(sizeof(char) * 13);
                strcat(output, "age critical");
                break;
        }
    }

    if (size_thresholds != NULL) {
        size_status = get_status(file_stat.st_size, size_thresholds);

        switch (size_status) {
            case STATE_WARNING:
                if (output != NULL) {
                    output = realloc(output, strlen(output) + sizeof(char) * 15);
                    strcat(output, ", size warning");
                } else {
                    output = malloc(sizeof(char) * 13);
                    strcat(output, "size warning");
                }
                break;
            case STATE_CRITICAL:
                if (output != NULL) {
                    output = realloc(output, strlen(output) + sizeof(char) * 16);
                    strcat(output, ", size critical");
                } else {
                    output = malloc(sizeof(char) * 14);
                    strcat(output, "size critical");
                }
                break;
        }
    }

    status = age_status > size_status ? age_status : size_status;

    switch (status) {
        case STATE_OK:
            ok("%s: Everithing ok.", filename);
        case STATE_WARNING:
            warning("%s: %s", filename, output);
        case STATE_CRITICAL:
            critical("%s: %s", filename, output);
    }



    critical("You should never reach this point.");
}

int process_arguments (int argc, char **argv) {
    int c;
    int option = 0;

    static struct option longopts[] = {
        MP_ARGS_HELP,
        MP_ARGS_VERS,
        MP_ARGS_VERB,
        {"file", required_argument, NULL, (int)'f'},
        MP_ARGS_WARN,
        MP_ARGS_CRIT,
        MP_ARGS_END
    };

    while (1) {
        c = getopt_long (argc, argv, "hVvt:f:w:c:W:C:", longopts, &option);

        if (c == -1 || c == EOF)
            break;

        switch (c) {
            MP_ARGS_CASE_DEF
	    case 'f':
	       filename = optarg;
	       break;
	    MP_ARGS_CASE_WARN_TIME(age_thresholds)
	    MP_ARGS_CASE_CRIT_TIME(age_thresholds)
	    case 'W':
	       if (setWarn(&size_thresholds, optarg, BISI) == ERROR)
		  usage("Illegal -W argument '%s'.", optarg);
	       break;
	    case 'C':
	       if (setCrit(&size_thresholds, optarg, BISI) == ERROR)
		  usage("Illegal -C argument '%s'.", optarg);
        }
    }

    return(OK);
}

void print_help (void) {
    print_revision();
    print_copyright();

    printf("\n");

    printf("This plugin test nothing.");

    printf("\n\n");

    print_usage();

    printf(MP_ARGS_HELP_DEF);
}

/* vim: set ts=4 sw=4 et syn=c.libdns : */
