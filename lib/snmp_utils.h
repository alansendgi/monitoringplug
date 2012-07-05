/***
 * Monitoring Plugin - snmp_utils.h
 **
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

#ifndef _SNMP_UTILS_H_
#define _SNMP_UTILS_H_

#include "config.h"
#include <getopt.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

/* The global snmp vars. */
/** Holds the community for the snmp connection. */
extern char *mp_snmp_community;
/** Holds the snmp version for the connection. */
extern int mp_snmp_version;
/** Holds the security level for the snmp connection. */
extern int mp_snmp_seclevel;
/** Holds the security name for the snmp connection. */
extern char *mp_snmp_secname;
/** Holds the context for the snmp connection. */
extern char *mp_snmp_context;
/** Holds the authentication password for the snmp connection. */
extern char *mp_snmp_authpass;
/** Holds the authentication protocol for the snmp connection. */
extern oid *mp_snmp_authproto;
/** Holds the privacy password for the snmp connection. */
extern char *mp_snmp_privpass;
/** Holds the query timeout. */
extern int mp_snmp_timeout;
/** Holds the query retransmit count. */
extern int mp_snmp_retries;
/** Maps ifOperStatus to text. */
extern char *ifOperStatusText[];

/** SNMP specific short option string. */
#define SNMP_OPTSTR "C:S:L:U:K:A:a:X:T:R:"
/** SNMP specific longopt struct. */
#define SNMP_LONGOPTS {"community", required_argument, NULL, (int)'C'}, \
                      {"snmp", required_argument, NULL, (int)'S'}, \
                      {"secname", required_argument, NULL, (int)'U'}, \
                      {"context", required_argument, NULL, (int)'K'}, \
                      {"authpass", required_argument, NULL, (int)'A'}, \
                      {"authproto", required_argument, NULL, (int)'a'}, \
                      {"privpass", required_argument, NULL, (int)'X'}, \
                      {"snmptimeout", required_argument, NULL, (int)'T'}, \
                      {"snmpretries", required_argument, NULL, (int)'R'}

/**
 * SNMP Query struct
 */
struct mp_snmp_query_cmd {
    /** OID name to query */
    oid oid[MAX_OID_LEN];
    /** OID lenght */
    size_t len;
    /** OID return type */
    u_char type;
    /** Pointer to store value in */
    void **target;
};

/**
 * SNMP Table Query struct
 */
struct mp_snmp_table {
    /** Table row count */
    int row;
    /** Table column count */
    int col;
    /** Table data count */
    netsnmp_variable_list **var;
};

/**
 * Init the Net-SNMP library and return a new session.
 * \return netsnmp_session created
 */
netsnmp_session *mp_snmp_init(void);

/**
 * Cleanup the Net-SNMP library.
 */
void mp_snmp_deinit(void);

/**
 * Run all querys in querycmd and save result to pointer in querycmd struct.
 * \param[in] ss Session to use.
 * \param[in|out] querycmd Query commands
 * \return return a status value like snmp.
 */
int mp_snmp_query(netsnmp_session *ss, const struct mp_snmp_query_cmd *querycmd);

/**
 * Run table query for querycmd and save mp_snmp_table to pointer in querycmd.
 * \param[in] ss Session to use.
 * \param[in|out] querycmd Table query command
 * \return return a status value like snmp.
 */
int mp_snmp_table_query(netsnmp_session *ss, const struct mp_snmp_query_cmd *querycmd, int cols);


/**
 * Get a netsnmp_variable_list out of a mp_snmp_table.
 * \param[in] table Table to fetch value from.
 * \param[in] x X coordinate of value.
 * \param[in] y Y coordinate of value.
 */
netsnmp_variable_list *mp_snmp_table_get(const struct mp_snmp_table table, int x, int y);

/**
 * Handle SNMP related command line options.
 * \param[in] c Command line option to handle.
 */
void getopt_snmp(int c);

/**
 * Print the help for the SNMP related command line options.
 */
void print_help_snmp(void);

/**
 * Print the Net-SNMP library revision.
 */
void print_revision_snmp(void);

#endif /* _SNMP_UTILS_H_ */

/* vim: set ts=4 sw=4 et syn=c : */
