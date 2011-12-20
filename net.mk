HAVE_LWIP = $(shell (cd lwip 2> /dev/null && echo y) || echo n)

ifeq ($(HAVE_LWIP),y)

OBJS += $(O)/liblwip.a
CFLAGS += -Ilwip/src/include -Inet -Ilwip/src/include/ipv4 -I. -DLWIP

LWIP_CFLAGS = -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb \
	 -m64 -Werror -std=c99 -fms-extensions -mno-sse -mcmodel=large -I$(QEMUSRC) \
	 -fno-omit-frame-pointer -DHW_$(HW) -include param.h \
	 -Wno-attributes -Wno-address -Wno-char-subscripts
LWIP_CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)

LWIP_INCLUDES := \
	-Ilwip/src/include \
	-Ilwip/src/include/ipv4 \
	-Inet \
	-Inet/arch \
	-I.

LWIP_SRCFILES += \
	lwip/src/api/api_lib.c \
	lwip/src/api/api_msg.c \
	lwip/src/api/err.c \
	lwip/src/api/sockets.c \
	lwip/src/api/tcpip.c \
	lwip/src/api/netbuf.c \
	lwip/src/core/init.c \
	lwip/src/core/tcp_in.c \
	lwip/src/core/dhcp.c \
	lwip/src/core/def.c \
	lwip/src/core/mem.c \
	lwip/src/core/memp.c \
	lwip/src/core/netif.c \
	lwip/src/core/pbuf.c \
	lwip/src/core/raw.c \
	lwip/src/core/stats.c \
	lwip/src/core/sys.c \
	lwip/src/core/tcp.c \
	lwip/src/core/timers.c \
	lwip/src/core/ipv4/ip_addr.c \
	lwip/src/core/ipv4/icmp.c \
	lwip/src/core/ipv4/ip.c \
	lwip/src/core/ipv4/ip_frag.c \
	lwip/src/core/ipv4/inet_chksum.c \
	lwip/src/core/ipv4/inet.c \
	lwip/src/core/tcp_out.c \
	lwip/src/core/udp.c \
	lwip/src/netif/etharp.c \
	net/sys_arch.c \

LWIP_OBJFILES := $(patsubst %.c, $(O)/%.o, $(LWIP_SRCFILES))
LWIP_OBJFILES := $(patsubst %.S, $(O)/%.o, $(LWIP_OBJFILES))

$(O)/net/%.o: net/%.c
	@echo "  CC     $@"
	$(Q)mkdir -p $(@D)
	$(Q)$(CC) $(LWIP_CFLAGS) $(LWIP_INCLUDES) -c -o $@ $<

$(O)/lwip/src/%.o: lwip/src/%.c
	@echo "  CC     $@"
	$(Q)mkdir -p $(@D)
	$(Q)$(CC) $(LWIP_CFLAGS) $(LWIP_INCLUDES) -c -o $@ $<

$(O)/liblwip.a: $(LWIP_OBJFILES)
	@echo "  AR     $@"
	$(Q)mkdir -p $(@D)
	$(Q)$(AR) r $@ $(LWIP_OBJFILES)

endif