#include <uapi/linux/if_match.h>

enum net_mat_linux_action_ids {
	ACTION_SET_EGRESS_PORT = 1,
	ACTION_DROP_PACKET,
	ACTION_PERMIT,
	ACTION_COUNT,
};

static struct net_mat_action_arg set_egress_port_args[] = {
	{
		.name = "egress_port",
		.type = NET_MAT_ACTION_ARG_TYPE_U32,
		.value_u32 = 0,
	},
	{
		.name = "",
		.type = NET_MAT_ACTION_ARG_TYPE_UNSPEC,
	},
};

static struct net_mat_action set_egress_port = {
	.name = "set_egress_port",
	.uid = ACTION_SET_EGRESS_PORT,
	.args = set_egress_port_args,
};

static struct net_mat_action drop_packet = {
	.name = "drop",
	.uid = ACTION_DROP_PACKET,
	.args = NULL,
};

static struct net_mat_action permit = {
	.name = "permit",
	.uid = ACTION_PERMIT,
	.args = NULL,
};

static struct net_mat_action count = {
	.name = "count",
	.uid = ACTION_COUNT,
	.args = NULL,
};

static struct net_mat_action *net_mat_linux_action_list[] = {
	&set_egress_port,
	&drop_packet,
	&permit,
	&count,
	NULL,
};
