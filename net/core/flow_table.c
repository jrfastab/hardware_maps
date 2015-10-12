/*
 * net/core/flow_table.h - Flow table interface for packet processing pipeline
 * Copyright (c) 2014 John Fastabend <john.r.fastabend@intel.com>
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

#include <uapi/linux/if_match.h>
#include <uapi/linux/if_match_hdrs.h>
#include <uapi/linux/if_match_acts.h>
#include <linux/if_bridge.h>
#include <linux/types.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include <net/rtnetlink.h>
#include <linux/module.h>

static struct genl_family net_mat_nl_family = {
	.id		= GENL_ID_GENERATE,
	.name		= NET_MAT_GENL_NAME,
	.version	= NET_MAT_GENL_VERSION,
	.maxattr	= NET_MAT_MAX,
	.netnsok	= true,
};

static const
struct nla_policy net_mat_matches_policy[NET_MAT_FIELD_REF_MAX + 1] = {
	[NET_MAT_FIELD_REF] = { .len = sizeof(struct net_mat_field_ref) },
};

static int net_mat_put_act_args(struct sk_buff *skb,
				struct net_mat_action_arg *args)
{
	struct nlattr *arg;
	int i, err;

	for (i = 0; args[i].type; i++) {
		arg = nla_nest_start(skb, NET_MAT_ACTION_ARGS);

		if (!arg)
			return -ENOMEM;

		if (args[i].name &&
		    nla_put_string(skb, NET_MAT_ACTION_ARG_NAME, args[i].name))
			goto errout;

		if (nla_put_u32(skb, NET_MAT_ACTION_ARG_TYPE,
				args[i].type))
			goto errout;

		switch (args[i].type) {
		case NET_MAT_ACTION_ARG_TYPE_U8:
			err = nla_put_u8(skb,
					 NET_MAT_ACTION_ARG_VALUE,
					 args[i].value_u8);
			break;
		case NET_MAT_ACTION_ARG_TYPE_U16:
			err = nla_put_u16(skb,
					  NET_MAT_ACTION_ARG_VALUE,
					  args[i].value_u16);
			break;
		case NET_MAT_ACTION_ARG_TYPE_U32:
			err = nla_put_u32(skb,
					  NET_MAT_ACTION_ARG_VALUE,
					  args[i].value_u32);
			break;
		case NET_MAT_ACTION_ARG_TYPE_U64:
			err = nla_put_u64(skb,
					  NET_MAT_ACTION_ARG_VALUE,
					  args[i].value_u64);
			break;
		case NET_MAT_ACTION_ARG_TYPE_NULL:
		default:
			err = 0;
			break;
		}

		if (err)
			goto errout;

		nla_nest_end(skb, arg);
	}
	return 0;

errout:
	nla_nest_cancel(skb, arg);
	return -EMSGSIZE;
}

static const
struct nla_policy net_mat_action_policy[NET_MAT_ACTION_ATTR_MAX + 1] = {
	[NET_MAT_ACTION_ATTR_NAME]	 = {.type = NLA_STRING,
					    .len = IFNAMSIZ - 1 },
	[NET_MAT_ACTION_ATTR_UID]	 = {.type = NLA_U32 },
	[NET_MAT_ACTION_ATTR_SIGNATURE] = {.type = NLA_NESTED },
};

static int net_mat_put_action(struct sk_buff *skb, struct net_mat_action *a)
{
	struct net_mat_action_arg *this;
	struct nlattr *nest;
	int err, args = 0;

	if (a->name && nla_put_string(skb, NET_MAT_ACTION_ATTR_NAME, a->name))
		return -EMSGSIZE;

	if (nla_put_u32(skb, NET_MAT_ACTION_ATTR_UID, a->uid))
		return -EMSGSIZE;

	if (!a->args)
		return 0;

	for (this = &a->args[0]; strlen(this->name) > 0; this++)
		args++;

	if (args) {
		nest = nla_nest_start(skb, NET_MAT_ACTION_ATTR_SIGNATURE);
		if (!nest)
			goto nest_put_failure;

		err = net_mat_put_act_args(skb, a->args);
		if (err) {
			nla_nest_cancel(skb, nest);
			return err;
		}
		nla_nest_end(skb, nest);
	}

	return 0;
nest_put_failure:
	return -EMSGSIZE;
}

static int net_mat_put_actions(struct sk_buff *skb,
			       struct net_mat_action **acts)
{
	struct nlattr *actions;
	int err, i;

	actions = nla_nest_start(skb, NET_MAT_ACTIONS);
	if (!actions)
		return -EMSGSIZE;

	for (i = 0; acts[i]; i++) {
		struct nlattr *action = nla_nest_start(skb, NET_MAT_ACTION);

		if (!action)
			goto action_put_failure;

		err = net_mat_put_action(skb, acts[i]);
		if (err)
			goto action_put_failure;
		nla_nest_end(skb, action);
	}
	nla_nest_end(skb, actions);

	return 0;
action_put_failure:
	nla_nest_cancel(skb, actions);
	return -EMSGSIZE;
}

static int net_mat_put_field_ref_top(struct sk_buff *skb,
				     struct net_mat_field_ref *ref)
{
	if (nla_put_u32(skb, NET_MAT_FIELD_REF_INSTANCE, ref->instance) ||
	    nla_put_u32(skb, NET_MAT_FIELD_REF_HEADER, ref->header) ||
	    nla_put_u32(skb, NET_MAT_FIELD_REF_FIELD, ref->field) ||
	    nla_put_u32(skb, NET_MAT_FIELD_REF_MASK_TYPE, ref->mask_type) ||
	    nla_put_u32(skb, NET_MAT_FIELD_REF_TYPE, ref->type))
		return -EMSGSIZE;

	return 0;
}

static int net_mat_put_field_ref_vals(struct sk_buff *skb,
				      struct net_mat_field_ref *ref)
{
	if (!ref->type)
		return 0;

	switch (ref->type) {
	case NET_MAT_FIELD_REF_ATTR_TYPE_U8:
		if (nla_put_u8(skb, NET_MAT_FIELD_REF_VALUE, ref->value_u8) ||
		    nla_put_u8(skb, NET_MAT_FIELD_REF_MASK, ref->mask_u8))
			return -EMSGSIZE;
		break;
	case NET_MAT_FIELD_REF_ATTR_TYPE_U16:
		if (nla_put_u16(skb, NET_MAT_FIELD_REF_VALUE, ref->value_u16) ||
		    nla_put_u16(skb, NET_MAT_FIELD_REF_MASK, ref->mask_u16))
			return -EMSGSIZE;
		break;
	case NET_MAT_FIELD_REF_ATTR_TYPE_U32:
		if (nla_put_u32(skb, NET_MAT_FIELD_REF_VALUE, ref->value_u32) ||
		    nla_put_u32(skb, NET_MAT_FIELD_REF_MASK, ref->mask_u32))
			return -EMSGSIZE;
		break;
	case NET_MAT_FIELD_REF_ATTR_TYPE_U64:
		if (nla_put_u64(skb, NET_MAT_FIELD_REF_VALUE, ref->value_u64) ||
		    nla_put_u64(skb, NET_MAT_FIELD_REF_MASK, ref->mask_u64))
			return -EMSGSIZE;
		break;
	default:
		break;
	}

	return 0;
}

int net_mat_put_fields(struct sk_buff *skb, const struct net_mat_header *h)
{
	struct net_mat_field *f;
	int count = h->field_sz;
	struct nlattr *field;

	for (f = h->fields; count; count--, f++) {
		field = nla_nest_start(skb, NET_MAT_FIELD);
		if (!field)
			goto field_put_failure;

		if (nla_put_string(skb, NET_MAT_FIELD_ATTR_NAME, f->name) ||
		    nla_put_u32(skb, NET_MAT_FIELD_ATTR_UID, f->uid) ||
		    nla_put_u32(skb, NET_MAT_FIELD_ATTR_BITWIDTH, f->bitwidth))
			goto out;

		nla_nest_end(skb, field);
	}

	return 0;
out:
	nla_nest_cancel(skb, field);
field_put_failure:
	return -EMSGSIZE;
}

int net_mat_put_headers(struct sk_buff *skb,
			struct net_mat_header **headers)
{
	struct nlattr *nest, *hdr, *fields;
	struct net_mat_header *h;
	int i, err;

	nest = nla_nest_start(skb, NET_MAT_HEADERS);
	if (!nest)
		return -EMSGSIZE;

	for (i = 0; headers[i]; i++) {
		err = -EMSGSIZE;
		h = headers[i];

		hdr = nla_nest_start(skb, NET_MAT_HEADER);
		if (!hdr)
			goto hdr_put_failure;

		if (nla_put_string(skb, NET_MAT_HEADER_ATTR_NAME, h->name) ||
		    nla_put_u32(skb, NET_MAT_HEADER_ATTR_UID, h->uid))
			goto attr_put_failure;

		fields = nla_nest_start(skb, NET_MAT_HEADER_ATTR_FIELDS);
		if (!fields)
			goto attr_put_failure;

		err = net_mat_put_fields(skb, h);
		if (err)
			goto fields_put_failure;

		nla_nest_end(skb, fields);

		nla_nest_end(skb, hdr);
	}
	nla_nest_end(skb, nest);

	return 0;
fields_put_failure:
	nla_nest_cancel(skb, fields);
attr_put_failure:
	nla_nest_cancel(skb, hdr);
hdr_put_failure:
	nla_nest_cancel(skb, nest);
	return err;
}

struct sk_buff *net_mat_build_headers_msg(struct net_mat_header **h,
					  struct net_device *dev,
					  u32 portid, int seq, u8 cmd)
{
	struct genlmsghdr *hdr;
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return ERR_PTR(-ENOBUFS);

	hdr = genlmsg_put(skb, portid, seq, &net_mat_nl_family, 0, cmd);
	if (!hdr)
		goto out;

	if (nla_put_u32(skb,
			NET_MAT_IDENTIFIER_TYPE, NET_MAT_IDENTIFIER_IFINDEX) ||
	    nla_put_u32(skb, NET_MAT_IDENTIFIER, dev->ifindex)) {
		err = -ENOBUFS;
		goto out;
	}

	err = net_mat_put_headers(skb, h);
	if (err < 0)
		goto out;

	genlmsg_end(skb, hdr);
	return skb;
out:
	nlmsg_free(skb);
	return ERR_PTR(err);
}

static const
struct nla_policy net_mat_action_arg_policy[NET_MAT_ACTION_ARG_MAX + 1] = {
	[NET_MAT_ACTION_ARG_NAME]	= {.type = NLA_STRING, },
	[NET_MAT_ACTION_ARG_TYPE]	= {.type = NLA_U32 },
	[NET_MAT_ACTION_ARG_VALUE]	= {.type = NLA_UNSPEC, },
};

static struct net_device *net_mat_get_dev(struct genl_info *info)
{
	struct net *net = genl_info_net(info);
	int type, ifindex;

	if (!info->attrs[NET_MAT_IDENTIFIER_TYPE] ||
	    !info->attrs[NET_MAT_IDENTIFIER])
		return NULL;

	type = nla_get_u32(info->attrs[NET_MAT_IDENTIFIER_TYPE]);
	switch (type) {
	case NET_MAT_IDENTIFIER_IFINDEX:
		ifindex = nla_get_u32(info->attrs[NET_MAT_IDENTIFIER]);
		break;
	default:
		return NULL;
	}

	return dev_get_by_index(net, ifindex);
}

static int net_mat_table_cmd_get_headers(struct sk_buff *skb,
					 struct genl_info *info)
{
	struct net_mat_header **h;
	struct net_device *dev;
	struct sk_buff *msg;

	dev = net_mat_get_dev(info);
	if (!dev)
		return -EINVAL;

	if (dev->netdev_ops->ndo_mat_get_headers)
		h = dev->netdev_ops->ndo_mat_get_headers(dev);
	else
		h = net_mat_linux_header_list;

	if (!h) {
		dev_put(dev);
		return -EBUSY;
	}

	msg = net_mat_build_headers_msg(h, dev,
					info->snd_portid,
					info->snd_seq,
					NET_MAT_TABLE_CMD_GET_HEADERS);

	dev_put(dev);

	if (IS_ERR(msg))
		return PTR_ERR(msg);

	return genlmsg_reply(msg, info);
}

struct sk_buff *net_mat_build_actions_msg(struct net_mat_action **a,
					  struct net_device *dev,
					  u32 portid, int seq, u8 cmd)
{
	struct genlmsghdr *hdr;
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return ERR_PTR(-ENOBUFS);

	hdr = genlmsg_put(skb, portid, seq, &net_mat_nl_family, 0, cmd);
	if (!hdr)
		goto out;

	if (nla_put_u32(skb,
			NET_MAT_IDENTIFIER_TYPE, NET_MAT_IDENTIFIER_IFINDEX) ||
	    nla_put_u32(skb, NET_MAT_IDENTIFIER, dev->ifindex)) {
		err = -ENOBUFS;
		goto out;
	}

	err = net_mat_put_actions(skb, a);
	if (err < 0)
		goto out;

	genlmsg_end(skb, hdr);
	return skb;
out:
	nlmsg_free(skb);
	return ERR_PTR(err);
}

static int net_mat_table_cmd_get_actions(struct sk_buff *skb,
					 struct genl_info *info)
{
	struct net_device *dev;
	struct sk_buff *msg;

	dev = net_mat_get_dev(info);
	if (!dev)
		return -EINVAL;

	msg = net_mat_build_actions_msg(net_mat_linux_action_list, dev,
					info->snd_portid,
					info->snd_seq,
					NET_MAT_TABLE_CMD_GET_ACTIONS);
	dev_put(dev);

	if (IS_ERR(msg))
		return PTR_ERR(msg);

	return genlmsg_reply(msg, info);
}

static int net_mat_put_header_node(struct sk_buff *skb,
				   struct net_mat_header_node *node)
{
	struct nlattr *hdrs, *jumps, *entry;
	int i, err;

	if (nla_put_string(skb, NET_MAT_HEADER_NODE_NAME, node->name) ||
	    nla_put_u32(skb, NET_MAT_HEADER_NODE_UID, node->uid))
		return -EMSGSIZE;

	/* Insert the set of headers that get extracted at this node */
	hdrs = nla_nest_start(skb, NET_MAT_HEADER_NODE_HDRS);
	if (!hdrs)
		return -EMSGSIZE;
	for (i = 0; node->hdrs[i]; i++) {
		if (nla_put_u32(skb, NET_MAT_HEADER_NODE_HDRS_VALUE,
				node->hdrs[i])) {
			nla_nest_cancel(skb, hdrs);
			return -EMSGSIZE;
		}
	}
	nla_nest_end(skb, hdrs);

	/* Then give the jump table to find next header node in graph */
	jumps = nla_nest_start(skb, NET_MAT_HEADER_NODE_JUMP);
	if (!jumps)
		return -EMSGSIZE;

	for (i = 0; node->jump[i].node; i++) {
		entry = nla_nest_start(skb, NET_MAT_JUMP_ENTRY);
		if (!entry)
			goto errout;

		if (nla_put_u32(skb,
				NET_MAT_FIELD_REF_NEXT_NODE,
				node->jump[i].node))
			goto errout;

		err = net_mat_put_field_ref_top(skb, &node->jump[i].field);
		if (err)
			goto errout;

		err = net_mat_put_field_ref_vals(skb, &node->jump[i].field);
		if (err)
			goto errout;

		nla_nest_end(skb, entry);
	}

	nla_nest_end(skb, jumps);

	return 0;

errout:
	nla_nest_cancel(skb, entry);
	nla_nest_cancel(skb, jumps);
	return -EMSGSIZE;
}

static int net_mat_put_header_graph(struct sk_buff *skb,
				    struct net_mat_header_node **g)
{
	struct nlattr *nodes, *node;
	int err, i;

	nodes = nla_nest_start(skb, NET_MAT_HEADER_GRAPH);
	if (!nodes)
		return -EMSGSIZE;

	for (i = 0; g[i]; i++) {
		node = nla_nest_start(skb, NET_MAT_HEADER_GRAPH_NODE);
		if (!node) {
			err = -EMSGSIZE;
			goto nodes_put_error;
		}

		err = net_mat_put_header_node(skb, g[i]);
		if (err)
			goto node_put_error;

		nla_nest_end(skb, node);
	}

	nla_nest_end(skb, nodes);
	return 0;
node_put_error:
	nla_nest_cancel(skb, node);
nodes_put_error:
	nla_nest_cancel(skb, nodes);
	return err;
}

static
struct sk_buff *net_mat_build_header_graph_msg(struct net_mat_header_node **g,
					       struct net_device *dev,
					       u32 portid, int seq, u8 cmd)
{
	struct genlmsghdr *hdr;
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return ERR_PTR(-ENOBUFS);

	hdr = genlmsg_put(skb, portid, seq, &net_mat_nl_family, 0, cmd);
	if (!hdr)
		goto out;

	if (nla_put_u32(skb,
			NET_MAT_IDENTIFIER_TYPE, NET_MAT_IDENTIFIER_IFINDEX) ||
	    nla_put_u32(skb, NET_MAT_IDENTIFIER, dev->ifindex)) {
		err = -ENOBUFS;
		goto out;
	}

	err = net_mat_put_header_graph(skb, g);
	if (err < 0)
		goto out;

	genlmsg_end(skb, hdr);
	return skb;
out:
	nlmsg_free(skb);
	return ERR_PTR(err);
}

static int net_mat_table_cmd_get_header_graph(struct sk_buff *skb,
					      struct genl_info *info)
{
	struct net_mat_header_node **h;
	struct net_device *dev;
	struct sk_buff *msg;

	dev = net_mat_get_dev(info);
	if (!dev)
		return -EINVAL;

	/* Driver writers may provide a hook for more esoteric packet
	 * patterns. If the op is omitted we assume the device is a
	 * basic device supporting only single stacked headers in their
	 * normal order. For now we assume programmable NICs do not
	 * program the parser during runtime and that the returned 'h'
	 * value is not updated during the get_dev/put_dev lifetime.
	 */
	if (dev->netdev_ops->ndo_mat_get_hdr_graph)
		h = dev->netdev_ops->ndo_mat_get_hdr_graph(dev);
	else
		h = net_mat_linux_header_graph;

	if (!h)
		return -EBUSY;

	msg = net_mat_build_header_graph_msg(h, dev,
					     info->snd_portid,
					     info->snd_seq,
					     NET_MAT_TABLE_CMD_GET_HEADER_GRAPH);
	dev_put(dev);

	if (IS_ERR(msg))
		return PTR_ERR(msg);

	return genlmsg_reply(msg, info);
}

static int net_mat_put_table(struct net_device *dev,
			     struct sk_buff *skb,
			     struct net_mat_table *t)
{
	struct nlattr *matches, *actions;
	int i;

	if (nla_put_string(skb, NET_MAT_TABLE_ATTR_NAME, t->name) ||
	    nla_put_u32(skb, NET_MAT_TABLE_ATTR_UID, t->uid) ||
	    nla_put_u32(skb, NET_MAT_TABLE_ATTR_SOURCE, t->source) ||
	    nla_put_u32(skb, NET_MAT_TABLE_ATTR_APPLY, t->apply_action) ||
	    nla_put_u32(skb, NET_MAT_TABLE_ATTR_SIZE, t->size))
		return -EMSGSIZE;

	matches = nla_nest_start(skb, NET_MAT_TABLE_ATTR_MATCHES);
	if (!matches)
		return -EMSGSIZE;

	for (i = 0; t->matches[i].instance; i++) {
		struct nlattr *field = nla_nest_start(skb, NET_MAT_FIELD_REF);
		int err;

		if (!field) {
			nla_nest_cancel(skb, matches);
			return -EMSGSIZE;
		}

		err = net_mat_put_field_ref_top(skb, &t->matches[i]);
		if (err) {
			nla_nest_cancel(skb, field);
			nla_nest_cancel(skb, matches);
			return -EMSGSIZE;
		}

		nla_nest_end(skb, field);
	}
	nla_nest_end(skb, matches);

	actions = nla_nest_start(skb, NET_MAT_TABLE_ATTR_ACTIONS);
	if (!actions)
		return -EMSGSIZE;

	for (i = 0; t->actions[i]; i++) {
		if (nla_put_u32(skb,
				NET_MAT_ACTION_ATTR_UID,
				t->actions[i])) {
			nla_nest_cancel(skb, actions);
			return -EMSGSIZE;
		}
	}
	nla_nest_end(skb, actions);

	return 0;
}

int net_mat_put_tables(struct net_device *dev,
		       struct sk_buff *skb,
		       struct net_mat_table **tables)
{
	struct nlattr *nest, *t;
	int i, err = 0;

	nest = nla_nest_start(skb, NET_MAT_TABLES);
	if (!nest)
		return -EMSGSIZE;

	if (!tables[0])
		goto errout;

	for (i = 0; tables[i]->uid; i++) {
		t = nla_nest_start(skb, NET_MAT_TABLE);
		if (!t) {
			err = -EMSGSIZE;
			goto errout;
		}

		err = net_mat_put_table(dev, skb, tables[i]);
		if (err) {
			nla_nest_cancel(skb, t);
			goto errout;
		}
		nla_nest_end(skb, t);
	}
	nla_nest_end(skb, nest);
	return 0;
errout:
	nla_nest_cancel(skb, nest);
	return err;
}
EXPORT_SYMBOL(net_mat_put_tables);

struct sk_buff *net_mat_build_tables_msg(struct net_mat_table **t,
					 struct net_device *dev,
					 u32 portid, int seq, u8 cmd)
{
	struct genlmsghdr *hdr;
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (!skb)
		return ERR_PTR(-ENOBUFS);

	hdr = genlmsg_put(skb, portid, seq, &net_mat_nl_family, 0, cmd);
	if (!hdr)
		goto out;

	if (nla_put_u32(skb,
			NET_MAT_IDENTIFIER_TYPE, NET_MAT_IDENTIFIER_IFINDEX) ||
	    nla_put_u32(skb, NET_MAT_IDENTIFIER, dev->ifindex)) {
		err = -ENOBUFS;
		goto out;
	}

	err = net_mat_put_tables(dev, skb, t);
	if (err < 0)
		goto out;

	genlmsg_end(skb, hdr);
	return skb;
out:
	nlmsg_free(skb);
	return ERR_PTR(err);
}

static int net_mat_table_cmd_get_tables(struct sk_buff *skb,
					struct genl_info *info)
{
	struct net_mat_table **tables;
	struct net_device *dev;
	struct sk_buff *msg;

	dev = net_mat_get_dev(info);
	if (!dev)
		return -EINVAL;

	if (!dev->netdev_ops->ndo_mat_get_tables) {
		dev_put(dev);
		return -EOPNOTSUPP;
	}

	tables = dev->netdev_ops->ndo_mat_get_tables(dev);
	if (!tables) { /* transient failure should always have some table */
		dev_put(dev);
		return -EBUSY;
	}

	msg = net_mat_build_tables_msg(tables, dev,
				       info->snd_portid,
				       info->snd_seq,
				       NET_MAT_TABLE_CMD_GET_TABLES);
	dev_put(dev);

	if (IS_ERR(msg))
		return PTR_ERR(msg);

	return genlmsg_reply(msg, info);
}

static const struct genl_ops net_mat_table_nl_ops[] = {
	{
		.cmd = NET_MAT_TABLE_CMD_GET_TABLES,
		.doit = net_mat_table_cmd_get_tables,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NET_MAT_TABLE_CMD_GET_HEADERS,
		.doit = net_mat_table_cmd_get_headers,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NET_MAT_TABLE_CMD_GET_ACTIONS,
		.doit = net_mat_table_cmd_get_actions,
		.flags = GENL_ADMIN_PERM,
	},
	{
		.cmd = NET_MAT_TABLE_CMD_GET_HEADER_GRAPH,
		.doit = net_mat_table_cmd_get_header_graph,
		.flags = GENL_ADMIN_PERM,
	},
};

static int __init net_mat_nl_module_init(void)
{
	return genl_register_family_with_ops(&net_mat_nl_family,
					     net_mat_table_nl_ops);
}

static void net_mat_nl_module_fini(void)
{
	genl_unregister_family(&net_mat_nl_family);
}

module_init(net_mat_nl_module_init);
module_exit(net_mat_nl_module_fini);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("John Fastabend <john.r.fastabend@intel.com>");
MODULE_DESCRIPTION("Netlink interface to Match Action Tables");
MODULE_ALIAS_GENL_FAMILY(NET_MAT_GENL_NAME);
