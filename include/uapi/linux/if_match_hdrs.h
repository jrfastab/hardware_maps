#include <uapi/linux/if_match.h>

/* The following is a set of basic headers devices are free to use
 * for defining packet parsing capabilities. The typical use for these
 * is users will query for some minimal set of support on the device
 * before placing workloads on a system that require hardware
 * acceleration.
 *
 * Driver writers may support additional protocols or a subset of these
 * by implementing the get_headers callback. Additional protocols may
 * be added here if they are sufficiently generic. Either way driver
 * writers must provide a definition of the supported headers so we
 * can emulate hardware in software if needed.
 *
 * If driver writers are exposing custom headers or proprietary fields
 * then they do not need to be placed here and be exposed by the driver
 * itself.
 */
enum net_mat_linux_header_ids {
	HEADER_ETHERNET = 1,
	HEADER_VLAN,
	HEADER_VXLAN,
	HEADER_IPV4,
	HEADER_TCP,
	HEADER_UDP,
};

enum net_mat_linux_header_ethernet_ids {
	HEADER_ETHERNET_SRC_MAC = 1,
	HEADER_ETHERNET_DST_MAC,
	HEADER_ETHERNET_ETHERTYPE,
};

static struct net_mat_field net_mat_ethernet_fields[3] = {
	{ .name = "src_mac", .uid = HEADER_ETHERNET_SRC_MAC, .bitwidth = 48},
	{ .name = "dst_mac", .uid = HEADER_ETHERNET_DST_MAC, .bitwidth = 48},
	{ .name = "ethertype",
	  .uid = HEADER_ETHERNET_ETHERTYPE,
	  .bitwidth = 16},
};

static struct net_mat_header net_mat_ethernet = {
	.name = "ether",
	.uid = HEADER_ETHERNET,
	.field_sz = ARRAY_SIZE(net_mat_ethernet_fields),
	.fields = net_mat_ethernet_fields,
};

enum net_mat_linux_header_vlan_ids {
	HEADER_VLAN_PCP = 1,
	HEADER_VLAN_CFI,
	HEADER_VLAN_VID,
	HEADER_VLAN_ETHERTYPE,
};

static struct net_mat_field net_mat_vlan_fields[4] = {
	{ .name = "pcp", .uid = HEADER_VLAN_PCP, .bitwidth = 3,},
	{ .name = "cfi", .uid = HEADER_VLAN_CFI, .bitwidth = 1,},
	{ .name = "vid", .uid = HEADER_VLAN_VID, .bitwidth = 12,},
	{ .name = "ethertype", .uid = HEADER_VLAN_ETHERTYPE, .bitwidth = 16,},
};

static struct net_mat_header net_mat_vlan = {
	.name = "vlan",
	.uid = HEADER_VLAN,
	.field_sz = ARRAY_SIZE(net_mat_vlan_fields),
	.fields = net_mat_vlan_fields,
};

enum net_mat_linux_header_vxlan_ids {
	HEADER_VXLAN_FLAGS = 1,
	HEADER_VXLAN_RESERVED1,
	HEADER_VXLAN_VNI,
	HEADER_VXLAN_RESERVED2,
};

static struct net_mat_field net_mat_vxlan_fields[] = {
	{ .name = "flags", .uid = HEADER_VXLAN_FLAGS, .bitwidth = 8,},
	{ .name = "reserved1", .uid = HEADER_VXLAN_RESERVED1, .bitwidth = 24,},
	{ .name = "vni", .uid = HEADER_VXLAN_VNI, .bitwidth = 24,},
	{ .name = "reserved2", .uid = HEADER_VXLAN_RESERVED2, .bitwidth = 8,},
};

static struct net_mat_header net_mat_vxlan = {
	.name = "vxlan",
	.uid = HEADER_VXLAN,
	.field_sz = ARRAY_SIZE(net_mat_vxlan_fields),
	.fields = net_mat_vxlan_fields,
};

enum net_mat_linux_header_ipv4_ids {
	HEADER_IPV4_VERSION = 1,
	HEADER_IPV4_IHL,
	HEADER_IPV4_TOS,
	HEADER_IPV4_LENGTH,
	HEADER_IPV4_IDENTIFICATION,
	HEADER_IPV4_FLAGS,
	HEADER_IPV4_FRAGMENT_OFFSET,
	HEADER_IPV4_TTL,
	HEADER_IPV4_PROTOCOL,
	HEADER_IPV4_CSUM,
	HEADER_IPV4_SRC_IP,
	HEADER_IPV4_DST_IP,
	HEADER_IPV4_OPTIONS,
};

static struct net_mat_field net_mat_ipv4_fields[13] = {
	{ .name = "version",
	  .uid = HEADER_IPV4_VERSION,
	  .bitwidth = 4,},
	{ .name = "ihl",
	  .uid = HEADER_IPV4_IHL,
	  .bitwidth = 4,},
	{ .name = "tos",
	  .uid = HEADER_IPV4_TOS,
	  .bitwidth = 8,},
	{ .name = "length",
	  .uid = HEADER_IPV4_LENGTH,
	  .bitwidth = 8,},
	{ .name = "ident",
	  .uid = HEADER_IPV4_IDENTIFICATION,
	  .bitwidth = 8,},
	{ .name = "flags",
	  .uid = HEADER_IPV4_FLAGS,
	  .bitwidth = 3,},
	{ .name = "frag_off",
	  .uid = HEADER_IPV4_FRAGMENT_OFFSET,
	  .bitwidth = 13,},
	{ .name = "ttl",
	  .uid = HEADER_IPV4_TTL,
	  .bitwidth = 1,},
	{ .name = "protocol",
	  .uid = HEADER_IPV4_PROTOCOL,
	  .bitwidth = 8,},
	{ .name = "csum",
	  .uid = HEADER_IPV4_CSUM,
	  .bitwidth = 8,},
	{ .name = "src_ip",
	  .uid = HEADER_IPV4_SRC_IP,
	  .bitwidth = 32,},
	{ .name = "dst_ip",
	  .uid = HEADER_IPV4_DST_IP,
	  .bitwidth = 32,},
	{ .name = "options",
	  .uid = HEADER_IPV4_OPTIONS,
	  .bitwidth = 0,},
	/* TBD options */
};

static struct net_mat_header net_mat_ipv4 = {
	.name = "ipv4",
	.uid = HEADER_IPV4,
	.field_sz = ARRAY_SIZE(net_mat_ipv4_fields),
	.fields = net_mat_ipv4_fields,
};

enum net_mat_linux_header_tcp_ids {
	HEADER_TCP_SRC_PORT = 1,
	HEADER_TCP_DST_PORT,
	HEADER_TCP_SEQ,
	HEADER_TCP_ACK,
	HEADER_TCP_OFFSET,
	HEADER_TCP_RESERVED,
	HEADER_TCP_FLAGS,
	HEADER_TCP_WINDOW,
	HEADER_TCP_CSUM,
	HEADER_TCP_URGENT,
};

static struct net_mat_field net_mat_tcp_fields[10] = {
	{ .name = "src_port",
	  .uid = HEADER_TCP_SRC_PORT,
	  .bitwidth = 16,
	},
	{ .name = "dst_port",
	  .uid = HEADER_TCP_DST_PORT,
	  .bitwidth = 16,
	},
	{ .name = "seq",
	  .uid = HEADER_TCP_SEQ,
	  .bitwidth = 32,
	},
	{ .name = "ack",
	  .uid = HEADER_TCP_ACK,
	  .bitwidth = 32,
	},
	{ .name = "offset",
	  .uid = HEADER_TCP_OFFSET,
	  .bitwidth = 4,
	},
	{ .name = "reserved",
	  .uid = HEADER_TCP_RESERVED,
	  .bitwidth = 3},
	{ .name = "flags",
	  .uid = HEADER_TCP_FLAGS,
	  .bitwidth = 9},
	{ .name = "window",
	  .uid = HEADER_TCP_WINDOW,
	  .bitwidth = 8,},
	{ .name = "csum",
	  .uid = HEADER_TCP_CSUM,
	  .bitwidth = 16,},
	{ .name = "urgent",
	  .uid = HEADER_TCP_URGENT,
	  .bitwidth = 16},
	/* TBD options */
};

static struct net_mat_header net_mat_tcp = {
	.name = "tcp",
	.uid = HEADER_TCP,
	.field_sz = ARRAY_SIZE(net_mat_tcp_fields),
	.fields = net_mat_tcp_fields,
};

enum net_mat_linux_header_udp_ids {
	HEADER_UDP_SRC_PORT = 1,
	HEADER_UDP_DST_PORT,
	HEADER_UDP_LENGTH,
	HEADER_UDP_CSUM,
};

static struct net_mat_field net_mat_udp_fields[4] = {
	{ .name = "src_port",
	  .uid = HEADER_UDP_SRC_PORT,
	  .bitwidth = 16},
	{ .name = "dst_port",
	  .uid = HEADER_UDP_DST_PORT,
	  .bitwidth = 16},
	{ .name = "length",
	  .uid = HEADER_UDP_LENGTH,
	  .bitwidth = 16},
	{ .name = "csum",
	  .uid = HEADER_UDP_CSUM,
	  .bitwidth = 16},
};

static struct net_mat_header net_mat_udp = {
	.name = "udp",
	.uid = HEADER_UDP,
	.field_sz = ARRAY_SIZE(net_mat_udp_fields),
	.fields = net_mat_udp_fields,
};

static struct net_mat_header *net_mat_linux_header_list[] = {
	&net_mat_ethernet,
	&net_mat_vlan,
	&net_mat_ipv4,
	&net_mat_tcp,
	&net_mat_udp,
	&net_mat_vxlan,
	NULL,
};

/* Defined headers are organized into packets but depending on the
 * hardware device we may or may not support parsing specific packets.
 * In order for users to recognize which packets can be handled by
 * their device we generate packet graphs. The following is a default
 * graph that _many_ devices can support. If devices support greater
 * parsing capabilities for example more stacked tags then they can
 * use the above defined headers to build a more complex graph.
 */
enum net_mat_header_instance {
	HEADER_INSTANCE_ETHERNET = 1,
	HEADER_INSTANCE_VLAN_OUTER,
	HEADER_INSTANCE_VLAN_INNER,
	HEADER_INSTANCE_IPV4,
	HEADER_INSTANCE_TCP,
	HEADER_INSTANCE_UDP,
	HEADER_INSTANCE_VXLAN,
	HEADER_INSTANCE_ETHERNET_INNER,
	HEADER_INSTANCE_VLAN_OUTER_INNER,
	HEADER_INSTANCE_VLAN_INNER_INNER,
	HEADER_INSTANCE_IPV4_INNER,
	HEADER_INSTANCE_TCP_INNER,
	HEADER_INSTANCE_UDP_INNER,
};

static struct net_mat_jump_table net_mat_parse_ethernet[] = {
	{
		.node = HEADER_INSTANCE_IPV4,
		.field = {
			.header = HEADER_ETHERNET,
			.field = HEADER_ETHERNET_ETHERTYPE,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 0x0800,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = HEADER_INSTANCE_VLAN_OUTER,
		.field = {
			.header = HEADER_ETHERNET,
			.field = HEADER_ETHERNET_ETHERTYPE,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 0x8100,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static __u32 net_mat_ethernet_headers[] = {HEADER_ETHERNET, 0};
static struct net_mat_header_node net_mat_node_ethernet = {
	.name = "ethernet",
	.uid = HEADER_INSTANCE_ETHERNET,
	.hdrs = net_mat_ethernet_headers,
	.jump = net_mat_parse_ethernet,
};

static struct net_mat_jump_table net_mat_parse_vlan[] = {
	{
		.node = HEADER_INSTANCE_IPV4,
		.field = {
			.header = HEADER_ETHERNET,
			.field = HEADER_ETHERNET_ETHERTYPE,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 0x0800,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static __u32 net_mat_vlan_headers[] = {HEADER_VLAN, 0};
static struct net_mat_header_node net_mat_node_vlan = {
	.name = "vlan",
	.uid = HEADER_INSTANCE_VLAN_OUTER,
	.hdrs = net_mat_vlan_headers,
	.jump = net_mat_parse_vlan,
};

static struct net_mat_jump_table net_mat_terminal_headers[] = {
	{
		.node = NET_MAT_JUMP_TABLE_DONE,
		.field = {0},
	},
	{
		.node = 0,
	},
};

static __u32 net_mat_tcp_headers[] = {HEADER_TCP, 0};
static struct net_mat_header_node net_mat_node_tcp = {
	.name = "tcp",
	.uid = HEADER_INSTANCE_TCP,
	.hdrs = net_mat_tcp_headers,
	.jump = net_mat_terminal_headers,
};

static struct net_mat_jump_table net_mat_parse_ipv4[] = {
	{
		.node = HEADER_INSTANCE_TCP,
		.field = {
			.header = HEADER_IPV4,
			.field = HEADER_IPV4_PROTOCOL,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 6,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = HEADER_INSTANCE_UDP,
		.field = {
			.header = HEADER_IPV4,
			.field = HEADER_IPV4_PROTOCOL,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 17,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static __u32 net_mat_ipv4_headers[] = {HEADER_IPV4, 0};
static struct net_mat_header_node net_mat_node_ipv4 = {
	.name = "ipv4",
	.uid = HEADER_INSTANCE_IPV4,
	.hdrs = net_mat_ipv4_headers,
	.jump = net_mat_parse_ipv4,
};

#define VXLAN_UDP_PORT 1234
#define VXLAN_GPE_UDP_PORT 4790

static struct net_mat_jump_table net_mat_parse_udp[] = {
	{
		.node = HEADER_INSTANCE_VXLAN,
		.field = {
			.header = HEADER_UDP,
			.field = HEADER_UDP_SRC_PORT,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = VXLAN_UDP_PORT,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static __u32 net_mat_udp_headers[] = {HEADER_UDP, 0};
static struct net_mat_header_node net_mat_node_udp = {
	.name = "udp",
	.uid = HEADER_INSTANCE_UDP,
	.hdrs = net_mat_udp_headers,
	.jump = net_mat_parse_udp,
};

static struct net_mat_jump_table net_mat_parse_vxlan[] = {
	{
		.node = HEADER_INSTANCE_ETHERNET_INNER,
		.field = {0},
	},
	{
		.node = 0,
	},
};

static __u32 net_mat_vxlan_headers[] = {HEADER_VXLAN, 0};
static struct net_mat_header_node net_mat_node_vxlan = {
	.name = "vxlan",
	.uid = HEADER_INSTANCE_VXLAN,
	.hdrs = net_mat_vxlan_headers,
	.jump = net_mat_parse_vxlan,
};

static struct net_mat_jump_table net_mat_parse_ethernet_inner[] = {
	{
		.node = HEADER_INSTANCE_IPV4_INNER,
		.field = {
			.header = HEADER_ETHERNET,
			.field = HEADER_ETHERNET_ETHERTYPE,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 0x0800,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = HEADER_INSTANCE_VLAN_OUTER_INNER,
		.field = {
			.header = HEADER_ETHERNET,
			.field = HEADER_ETHERNET_ETHERTYPE,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 0x8100,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static struct net_mat_header_node net_mat_node_ethernet_inner = {
	.name = "inner_ethernet",
	.uid = HEADER_INSTANCE_ETHERNET_INNER,
	.hdrs = net_mat_ethernet_headers,
	.jump = net_mat_parse_ethernet_inner,
};

static struct net_mat_jump_table net_mat_parse_vlan_inner[] = {
	{
		.node = HEADER_INSTANCE_IPV4_INNER,
		.field = {
			.header = HEADER_ETHERNET,
			.field = HEADER_ETHERNET_ETHERTYPE,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 0x0800,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static struct net_mat_header_node net_mat_node_vlan_inner = {
	.name = "inner_vlan",
	.uid = HEADER_INSTANCE_VLAN_OUTER_INNER,
	.hdrs = net_mat_vlan_headers,
	.jump = net_mat_parse_vlan_inner,
};

static struct net_mat_jump_table net_mat_parse_ipv4_inner[] = {
	{
		.node = HEADER_INSTANCE_TCP_INNER,
		.field = {
			.header = HEADER_IPV4,
			.field = HEADER_IPV4_PROTOCOL,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 6,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = HEADER_INSTANCE_UDP_INNER,
		.field = {
			.header = HEADER_IPV4,
			.field = HEADER_IPV4_PROTOCOL,
			.type = NET_MAT_FIELD_REF_ATTR_TYPE_U16,
			.value_u16 = 17,
			.mask_u16 = 0xFFFF,
		}
	},
	{
		.node = 0,
	},
};

static struct net_mat_header_node net_mat_node_tcp_inner = {
	.name = "inner_tcp",
	.uid = HEADER_INSTANCE_TCP_INNER,
	.hdrs = net_mat_tcp_headers,
	.jump = net_mat_terminal_headers,
};

static struct net_mat_header_node net_mat_node_udp_inner = {
	.name = "inner_udp",
	.uid = HEADER_INSTANCE_UDP_INNER,
	.hdrs = net_mat_udp_headers,
	.jump = net_mat_terminal_headers,
};

static struct net_mat_header_node net_mat_node_ipv4_inner = {
	.name = "inner_ipv4",
	.uid = HEADER_INSTANCE_IPV4_INNER,
	.hdrs = net_mat_ipv4_headers,
	.jump = net_mat_parse_ipv4_inner,
};

static struct net_mat_header_node *net_mat_linux_header_graph[] = {
	&net_mat_node_ethernet,
	&net_mat_node_vlan,
	&net_mat_node_ipv4,
	&net_mat_node_udp,
	&net_mat_node_tcp,
	&net_mat_node_vxlan,
	&net_mat_node_ethernet_inner,
	&net_mat_node_vlan_inner,
	&net_mat_node_ipv4_inner,
	&net_mat_node_udp_inner,
	&net_mat_node_tcp_inner,
	NULL,
};
