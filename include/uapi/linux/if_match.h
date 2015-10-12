/*
 * include/uapi/linux/if_match.h -  interface for match action tables
 * Copyright (c) 2014 John Fastabend <john.r.fastabend@intel.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Author: John Fastabend <john.r.fastabend@intel.com>
 */

/* Netlink description:
 *
 * Table definition used to describe running tables. The following
 * describes the netlink message returned from a get tables request.
 * For ADD_FLOW, DELETE_FLOW, and UPDATE Flow only the following
 * attributes need to be provided, NET_MAT_TABLE_ATTR_UID and at least
 * one complete NET_MAT_RULE attribute.
 *
 * [NET_MAT_TABLE_IDENTIFIER_TYPE]
 * [NET_MAT_TABLE_IDENTIFIER]
 * [NET_MAT_TABLE_TABLES]
 *     [NET_MAT_TABLE]
 *       [NET_MAT_TABLE_ATTR_NAME]
 *       [NET_MAT_TABLE_ATTR_UID]
 *       [NET_MAT_TABLE_ATTR_SOURCE]
 *       [NET_MAT_TABLE_ATTR_SIZE]
 *	 [NET_MAT_TABLE_ATTR_MATCHES]
 *	   [NET_MAT_FIELD_REF]
 *	   [NET_MAT_FIELD_REF]
 *	     [...]
 *	   [...]
 *	 [NET_MAT_TABLE_ATTR_ACTIONS]
 *	   [NET_MAT_ACTION]
 *	     [NET_MAT_ACTION_ATTR_NAME]
 *	     [NET_MAT_ACTION_ATTR_UID]
 *	     [NET_MAT_ACTION_ATTR_SIGNATURE]
 *		 [NET_MAT_ACTION_ARG]
 *	         [NET_MAT_ACTION_ARG]
 *	         [...]
 *	   [NET_MAT_ACTION]
 *	     [...]
 *	   [...]
 *     [NET_MAT_TABLE]
 *       [...]
 *
 * Header definitions used to define headers with user friendly
 * names.
 *
 * [NET_MAT_TABLE_HEADERS]
 *   [NET_MAT_HEADER]
 *	[NET_MAT_HEADER_ATTR_NAME]
 *	[NET_MAT_HEADER_ATTR_UID]
 *	[NET_MAT_HEADER_ATTR_FIELDS]
 *	  [NET_MAT_HEADER_ATTR_FIELD]
 *	    [NET_MAT_FIELD_ATTR_NAME]
 *	    [NET_MAT_FIELD_ATTR_UID]
 *	    [NET_MAT_FIELD_ATTR_BITWIDTH]
 *	  [NET_MAT_HEADER_ATTR_FIELD]
 *	    [...]
 *	  [...]
 *   [NET_MAT_HEADER]
 *      [...]
 *   [...]
 *
 * Action definitions supported by tables
 *
 * [NET_MAT_TABLE_ACTIONS]
 *   [NET_MAT_TABLE_ATTR_ACTIONS]
 *	[NET_MAT_ACTION]
 *	  [NET_MAT_ACTION_ATTR_NAME]
 *	  [NET_MAT_ACTION_ATTR_UID]
 *	  [NET_MAT_ACTION_ATTR_SIGNATURE]
 *		 [NET_MAT_ACTION_ARG]
 *		 [NET_MAT_ACTION_ARG]
 *		 [...]
 *	[NET_MAT_ACTION]
 *	  [...]
 *
 */

#ifndef _UAPI_LINUX_IF_MATCH
#define _UAPI_LINUX_IF_MATCH

#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/if.h>

/**
 * @struct net_mat_fields
 * @brief defines a field in a header
 */
struct net_mat_field {
	char name[IFNAMSIZ];
	int uid;
	int bitwidth;
};

enum {
	NET_MAT_FIELD_UNSPEC,
	NET_MAT_FIELD,
	__NET_MAT_FIELD_MAX,
};
#define NET_MAT_FIELD_MAX (__NET_MAT_FIELD_MAX - 1)

enum {
	NET_MAT_FIELD_ATTR_UNSPEC,
	NET_MAT_FIELD_ATTR_NAME,
	NET_MAT_FIELD_ATTR_UID,
	NET_MAT_FIELD_ATTR_BITWIDTH,
	__NET_MAT_FIELD_ATTR_MAX,
};
#define NET_MAT_FIELD_ATTR_MAX (__NET_MAT_FIELD_ATTR_MAX - 1)

/**
 * @struct net_mat_header
 * @brief defines a match (header/field) an endpoint can use
 *
 * @uid unique identifier for header
 * @field_sz number of fields are in the set
 * @fields the set of fields in the net_mat_header
 */
struct net_mat_header {
	char name[IFNAMSIZ];
	int uid;
	int field_sz;
	struct net_mat_field *fields;
};

enum {
	NET_MAT_HEADER_UNSPEC,
	NET_MAT_HEADER,
	__NET_MAT_HEADER_MAX,
};
#define NET_MAT_HEADER_MAX (__NET_MAT_HEADER_MAX - 1)

enum {
	NET_MAT_HEADER_ATTR_UNSPEC,
	NET_MAT_HEADER_ATTR_NAME,
	NET_MAT_HEADER_ATTR_UID,
	NET_MAT_HEADER_ATTR_FIELDS,
	__NET_MAT_HEADER_ATTR_MAX,
};
#define NET_MAT_HEADER_ATTR_MAX (__NET_MAT_HEADER_ATTR_MAX - 1)

enum {
	NET_MAT_MASK_TYPE_UNSPEC,
	NET_MAT_MASK_TYPE_EXACT,
	NET_MAT_MASK_TYPE_LPM,
};

/**
 * @struct net_mat_field_ref
 * @brief uniquely identify field as header:field tuple
 */
struct net_mat_field_ref {
	int instance;
	int header;
	int field;
	int mask_type;
	int type;
	union {	/* Are these all the required data types */
		__u8 value_u8;
		__u16 value_u16; /* TBD: Misaligned */
		__u32 value_u32;
		__u64 value_u64;
	};
	union {	/* Are these all the required data types */
		__u8 mask_u8;
		__u16 mask_u16;
		__u32 mask_u32;
		__u64 mask_u64;
	};
};

enum {
	NET_MAT_FIELD_REF_UNSPEC,
	NET_MAT_FIELD_REF_NEXT_NODE,
	NET_MAT_FIELD_REF_INSTANCE,
	NET_MAT_FIELD_REF_HEADER,
	NET_MAT_FIELD_REF_FIELD,
	NET_MAT_FIELD_REF_MASK_TYPE,
	NET_MAT_FIELD_REF_TYPE,
	NET_MAT_FIELD_REF_VALUE,
	NET_MAT_FIELD_REF_MASK,
	__NET_MAT_FIELD_REF_MAX,
};
#define NET_MAT_FIELD_REF_MAX (__NET_MAT_FIELD_REF_MAX - 1)

enum {
	NET_MAT_FIELD_REFS_UNSPEC,
	NET_MAT_FIELD_REF,
	__NET_MAT_FIELD_REFS_MAX,
};
#define NET_MAT_FIELD_REFS_MAX (__NET_MAT_FIELD_REFS_MAX - 1)

enum {
	NET_MAT_FIELD_REF_ATTR_TYPE_UNSPEC,
	NET_MAT_FIELD_REF_ATTR_TYPE_U8,
	NET_MAT_FIELD_REF_ATTR_TYPE_U16,
	NET_MAT_FIELD_REF_ATTR_TYPE_U32,
	NET_MAT_FIELD_REF_ATTR_TYPE_U64,
	/* Need more types for ether.addrs, ip.addrs, ... */
};

enum net_mat_action_arg_type {
	NET_MAT_ACTION_ARG_TYPE_UNSPEC,
	NET_MAT_ACTION_ARG_TYPE_NULL,
	NET_MAT_ACTION_ARG_TYPE_U8,
	NET_MAT_ACTION_ARG_TYPE_U16,
	NET_MAT_ACTION_ARG_TYPE_U32,
	NET_MAT_ACTION_ARG_TYPE_U64,
	__NET_MAT_ACTION_ARG_TYPE_VAL_MAX,
};

struct net_mat_action_arg {
	char name[IFNAMSIZ];
	enum net_mat_action_arg_type type;
	union {
		__u8  value_u8;
		__u16 value_u16;
		__u32 value_u32;
		__u64 value_u64;
	};
};

enum {
	NET_MAT_ACTION_ARG_UNSPEC,
	NET_MAT_ACTION_ARG_NAME,
	NET_MAT_ACTION_ARG_TYPE,
	NET_MAT_ACTION_ARG_VALUE,
	__NET_MAT_ACTION_ARG_MAX,
};
#define NET_MAT_ACTION_ARG_MAX (__NET_MAT_ACTION_ARG_MAX - 1)

enum {
	NET_MAT_ACTION_ARGS_UNSPEC,
	NET_MAT_ACTION_ARGS,
	__NET_MAT_ACTION_ARGS_MAX,
};
#define NET_MAT_ACTION_ARGS_MAX (__NET_MAT_ACTION_ARGS_MAX - 1)

typedef __u32 net_mat_action_ref;

/**
 * @struct net_mat_action
 * @brief a description of a endpoint defined action
 *
 * @name printable name
 * @uid unique action identifier
 * @types NET_MAT_ACTION_TYPE_NULL terminated list of action types
 */
struct net_mat_action {
	char name[IFNAMSIZ];
	net_mat_action_ref uid;
	struct net_mat_action_arg *args;
};

enum {
	NET_MAT_ACTION_UNSPEC,
	NET_MAT_ACTION,
	__NET_MAT_ACTION_MAX,
};
#define NET_MAT_ACTION_MAX (__NET_MAT_ACTION_MAX - 1)

enum {
	NET_MAT_ACTION_ATTR_UNSPEC,
	NET_MAT_ACTION_ATTR_NAME,
	NET_MAT_ACTION_ATTR_UID,
	NET_MAT_ACTION_ATTR_SIGNATURE,
	__NET_MAT_ACTION_ATTR_MAX,
};
#define NET_MAT_ACTION_ATTR_MAX (__NET_MAT_ACTION_ATTR_MAX - 1)

/**
 * @struct net_mat_actions
 * @brief a set of supported action types
 *
 * @net_mat_actions null terminated list of actions
 */
struct net_mat_actions {
	struct net_mat_action **actions;
};

enum {
	NET_MAT_ACTION_SET_UNSPEC,
	NET_MAT_ACTION_SET_ACTIONS,
	__NET_MAT_ACTION_SET_MAX,
};
#define NET_MAT_ACTION_SET_MAX (__NET_MAT_ACTION_SET_MAX - 1)

typedef __u32 net_mat_header_ref;

struct net_mat_jump_table {
	struct net_mat_field_ref field;
	__u32 node; /* <0 is a parser error */
};

#define NET_MAT_JUMP_TABLE_DONE	-1

enum {
	NET_MAT_JUMP_ENTRY_UNSPEC,
	NET_MAT_JUMP_ENTRY,
	__NET_MAT_JUMP_ENTRY_MAX,
};

enum {
	NET_MAT_HEADER_NODE_HDRS_UNSPEC,
	NET_MAT_HEADER_NODE_HDRS_VALUE,
	__NET_MAT_HEADER_NODE_HDRS_MAX,
};
#define NET_MAT_HEADER_NODE_HDRS_MAX (__NET_MAT_HEADER_NODE_HDRS_MAX - 1)

/* net_mat_header_node: node in a header graph of header fields.
 *
 * @uid : unique id of the graph node
 * @flwo_header_ref : identify the hdrs that can handled by this node
 * @net_mat_set : identify if any metadata fields are set by parser
 * @net_mat_jump_table : give a case jump statement
 */
struct net_mat_header_node {
	char name[IFNAMSIZ];
	__u32 uid;
	net_mat_header_ref *hdrs;
	struct net_mat_jump_table *jump;
};

enum {
	NET_MAT_HEADER_NODE_UNSPEC,
	NET_MAT_HEADER_NODE_NAME,
	NET_MAT_HEADER_NODE_UID,
	NET_MAT_HEADER_NODE_HDRS,
	NET_MAT_HEADER_NODE_JUMP,
	__NET_MAT_HEADER_NODE_MAX,
};
#define NET_MAT_HEADER_NODE_MAX (__NET_MAT_HEADER_NODE_MAX - 1)

enum {
	NET_MAT_HEADER_GRAPH_UNSPEC,
	NET_MAT_HEADER_GRAPH_NODE,
	__NET_MAT_HEADER_GRAPH_MAX,
};
#define NET_MAT_HEADER_GRAPH_MAX (__NET_MAT_HEADER_GRAPH_MAX - 1)

/**
 * @struct net_mat_table
 * @brief define flow table with supported match/actions
 *
 * @uid unique identifier for table
 * @source uid of parent table
 * @size max number of entries for table or -1 for unbounded
 * @type indicate how table is exposed to networking stack
 * @type_arg argument for table bind type
 * @matches null terminated set of supported match types given by match uid
 * @actions null terminated set of supported action types given by action uid
 * @flows set of flows
 */
struct net_mat_table {
	char name[IFNAMSIZ];
	__u32 uid;
	__u32 source;
	__u32 apply_action;
	__u32 size;
	__u32 type;
	union {
		__u32 bpf_map_arg;
	};
	struct net_mat_field_ref *matches;
	net_mat_action_ref *actions;
};

enum {
	NET_MAT_TABLE_UNSPEC,
	NET_MAT_TABLE,
	__NET_MAT_TABLE_MAX,
};
#define NET_MAT_TABLE_MAX (__NET_MAT_TABLE_MAX - 1)

enum {
	NET_MAT_TABLE_TYPE_L2,
	NET_MAT_TABLE_TYPE_L3,
	NET_MAT_TABLE_TYPE_BPFMAP,
};

enum {
	NET_MAT_TABLE_ATTR_UNSPEC,
	NET_MAT_TABLE_ATTR_NAME,
	NET_MAT_TABLE_ATTR_UID,
	NET_MAT_TABLE_ATTR_SOURCE,
	NET_MAT_TABLE_ATTR_APPLY,
	NET_MAT_TABLE_ATTR_SIZE,
	NET_MAT_TABLE_ATTR_TYPE,
	NET_MAT_TABLE_ATTR_TYPE_ARG,
	NET_MAT_TABLE_ATTR_MATCHES,
	NET_MAT_TABLE_ATTR_ACTIONS,
	__NET_MAT_TABLE_ATTR_MAX,
};
#define NET_MAT_TABLE_ATTR_MAX (__NET_MAT_TABLE_ATTR_MAX - 1)


enum {
	NET_MAT_IDNETIFIER_UNSPEC,
	NET_MAT_IDENTIFIER_IFINDEX, /* net_device ifindex */
};

enum {
	NET_MAT_UNSPEC,
	NET_MAT_IDENTIFIER_TYPE,
	NET_MAT_IDENTIFIER,

	NET_MAT_TABLES,
	NET_MAT_HEADERS,
	NET_MAT_ACTIONS,
	NET_MAT_HEADER_GRAPH,

	__NET_MAT_MAX,
	NET_MAT_MAX = (__NET_MAT_MAX - 1),
};

enum {
	NET_MAT_TABLE_CMD_GET_TABLES,
	NET_MAT_TABLE_CMD_GET_HEADERS,
	NET_MAT_TABLE_CMD_GET_ACTIONS,

	NET_MAT_TABLE_CMD_GET_HEADER_GRAPH,
	NET_MAT_TABLE_CMD_GET_TABLE_GRAPH,

	NET_MAT_TABLE_CMD_CREATE_TABLE,
	NET_MAT_TABLE_CMD_DESTROY_TABLE,

	__NET_MAT_CMD_MAX,
	NET_MAT_CMD_MAX = (__NET_MAT_CMD_MAX - 1),
};

#define NET_MAT_GENL_NAME "net_mat_table"
#define NET_MAT_GENL_VERSION 0x1
#endif /* _UAPI_LINUX_IF_MATCH */
