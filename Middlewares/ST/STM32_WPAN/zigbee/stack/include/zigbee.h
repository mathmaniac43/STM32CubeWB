/* Copyright [2009 - 2019] Exegin Technologies Limited. All rights reserved. */

#ifndef ZIGBEE_H
# define ZIGBEE_H

#if defined(__GNUC__)
# define ZB_WARN_UNUSED __attribute__((warn_unused_result))
# define ZB_F_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && (_MSC_VER >= 1800)
# define ZB_WARN_UNUSED
# define ZB_F_DEPRECATED __declspec(deprecated)
#else
# define ZB_WARN_UNUSED
# define ZB_F_DEPRECATED
#endif

#ifdef __CDT_PARSER__
/* Eclipse indexer only code */
#include "zb_make_config.h"
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>
/*lint -save -e829 [ 'stdarg.h' usage should be deprecated - 17.1 REQUIRED] */
/*lint -save -e451 [ header file included withoutstandard guard - 4.10 REQUIRED] */
#include <stdarg.h>
/*lint -restore */
/*lint -restore */

#include "ieee802154.api.h"
#include "llist.h"

/* ZigBee Protocol Versions as related to the spec version. */
#if 0 /* deprecated */
#define ZB_PROTOCOL_VERSION_2004        0x0001U
#endif
#define ZB_PROTOCOL_VERSION_2007        0x0002U

/* Channel Mask / Page Helpers */
#define ZB_CHANNELMASK_GETPAGE(x)       (uint8_t)(((x) >> WPAN_PAGE_CHANNELS_MAX) & 0x1FU)
#define ZB_CHANNELMASK(mask, page)      ((page != 0U) ? \
                                         ((mask) | ((((uint32_t)page) & 0x1FU) << WPAN_PAGE_CHANNELS_MAX)) : (mask))

/* A value of 0xffff means the device is not associated. */
#define ZB_PANID_MAX                    0xfffeU

#define ZB_EPID_MIN                     0x0000000000000001ULL
#define ZB_EPID_MAX                     0xfffffffffffffffeULL

/* The ZigBee distributed trust center address. */
#define ZB_DISTRIBUTED_TC_ADDR          0xffffffffffffffffULL

/* A subset of WPAN_CHANNELMASK_2400MHZ (HA and SE preferred channels) */
#define ZB_CHANNELMASK_2400MHZ_HA       0x0318C800U /* Channels 11, 14, 15, 19, 20, 24, 25 */

/* ZigBee Status Codes */
enum ZbStatusCodeT {
    /* General Status Codes */
    ZB_STATUS_SUCCESS = 0x00,
    ZB_STATUS_ALLOC_FAIL = 0x70, /* Memory allocation failure. */
    ZB_STATUS_TIMEOUT = 0x71, /* Message timeout. */

    /* ZDP Status Codes */
    ZB_ZDP_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_ZDP_STATUS_INV_REQTYPE = 0x80,
    ZB_ZDP_STATUS_DEVNOTFOUND = 0x81,
    ZB_ZDP_STATUS_INVALID_EP = 0x82,
    ZB_ZDP_STATUS_NOT_ACTIVE = 0x83,
    ZB_ZDP_STATUS_NOT_SUPPORTED = 0x84,
    ZB_ZDP_STATUS_TIMEOUT = 0x85,
    ZB_ZDP_STATUS_NO_MATCH = 0x86,
    ZB_ZDP_STATUS_NO_ENTRY = 0x88,
    ZB_ZDP_STATUS_NO_DESCRIPTOR = 0x89,
    ZB_ZDP_STATUS_INSUFFICIENT_SPACE = 0x8a,
    ZB_ZDP_STATUS_NOT_PERMITTED = 0x8b,
    ZB_ZDP_STATUS_TABLE_FULL = 0x8c,
    ZB_ZDP_STATUS_NOT_AUTHORIZED = 0x8d,
    ZB_ZDP_STATUS_DEVICE_BINDING_TABLE_FULL = 0x8e,
    ZB_ZDP_STATUS_INVALID_INDEX = 0x8f,

    /* APS Status Codes. */
    ZB_APS_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_APS_STATUS_ASDU_TOO_LONG = 0xa0,
    ZB_APS_STATUS_DEFRAG_DEFERRED = 0xa1,
    ZB_APS_STATUS_DEFRAG_UNSUPPORTED = 0xa2,
    ZB_APS_STATUS_ILLEGAL_REQUEST = 0xa3,
    ZB_APS_STATUS_INVALID_BINDING = 0xa4,
    ZB_APS_STATUS_INVALID_GROUP = 0xa5,
    ZB_APS_STATUS_INVALID_PARAMETER = 0xa6,
    ZB_APS_STATUS_NO_ACK = 0xa7,
    ZB_APS_STATUS_NO_BOUND_DEVICE = 0xa8,
    ZB_APS_STATUS_NO_SHORT_ADDRESS = 0xa9,
    ZB_APS_STATUS_NOT_SUPPORTED = 0xaa,
    ZB_APS_STATUS_SECURED_LINK_KEY = 0xab,
    ZB_APS_STATUS_SECURED_NWK_KEY = 0xac,
    ZB_APS_STATUS_SECURITY_FAIL = 0xad,
    ZB_APS_STATUS_TABLE_FULL = 0xae,
    ZB_APS_STATUS_UNSECURED = 0xaf,
    ZB_APS_STATUS_UNSUPPORTED_ATTRIBUTE = 0xb0,
    /* These values used in case of internal errors. */
    ZB_APS_STATUS_INVALID_INDEX = 0xbd,

    /* NWK Status Codes. */
    ZB_NWK_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_NWK_STATUS_INVALID_PARAMETER = 0xc1,
    ZB_NWK_STATUS_INVALID_REQUEST = 0xc2,
    ZB_NWK_STATUS_NOT_PERMITTED = 0xc3,
    ZB_NWK_STATUS_STARTUP_FAILURE = 0xc4,
    ZB_NWK_STATUS_ALREADY_PRESENT = 0xc5,
    ZB_NWK_STATUS_SYNC_FAILURE = 0xc6,
    ZB_NWK_STATUS_TABLE_FULL = 0xc7,
    ZB_NWK_STATUS_UNKNOWN_DEVICE = 0xc8,
    ZB_NWK_STATUS_UNSUPPORTED_ATTRIBUTE = 0xc9,
    ZB_NWK_STATUS_NO_NETWORKS = 0xca,
    ZB_NWK_STATUS_LEAVE_UNCONFIRMED = 0xcb,
    ZB_NWK_STATUS_MAX_FRM_CNTR = 0xcc,
    ZB_NWK_STATUS_NO_KEY = 0xcd,
    ZB_NWK_STATUS_BAD_CCM_OUTPUT = 0xce,
    ZB_NWK_STATUS_NO_ROUTING_CAPACITY = 0xcf,
    ZB_NWK_STATUS_ROUTE_DISCOVERY_FAILED = 0xd0,
    ZB_NWK_STATUS_ROUTE_ERROR = 0xd1,
    ZB_NWK_STATUS_BT_TABLE_FULL = 0xd2,
    ZB_NWK_STATUS_FRAME_NOT_BUFFERED = 0xd3,
    /* Exegin Custom Status Values. */
    ZB_NWK_STATUS_INVALID_INDEX = 0xd4,
    ZB_NWK_STATUS_INTERNAL_ERR = 0xd6,

    /* WPAN Status Codes (copied from ieee802154.api.h to here for MISRA) */
    ZB_WPAN_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_WPAN_STATUS_COUNTER_ERROR = mcp_STATUS_COUNTER_ERROR,
    ZB_WPAN_STATUS_IMPROPER_KEY_TYPE = mcp_STATUS_IMPROPER_KEY_TYPE,
    ZB_WPAN_STATUS_IMPROPER_SECURITY_LEVEL = mcp_STATUS_IMPROPER_SECURITY_LEVEL,
    ZB_WPAN_STATUS_UNSUPPORTED_LEGACY = mcp_STATUS_UNSUPPORTED_LEGACY,
    ZB_WPAN_STATUS_UNSUPPORTED_SECURITY = mcp_STATUS_UNSUPPORTED_SECURITY,
    ZB_WPAN_STATUS_BEACON_LOSS = mcp_STATUS_BEACON_LOSS,
    ZB_WPAN_STATUS_CHANNEL_ACCESS_FAILURE = mcp_STATUS_CHANNEL_ACCESS_FAILURE,
    ZB_WPAN_STATUS_DENIED = mcp_STATUS_DENIED,
    ZB_WPAN_STATUS_DISABLE_TRX_FAILURE = mcp_STATUS_DISABLE_TRX_FAILURE,
    ZB_WPAN_STATUS_SECURITY_ERROR = mcp_STATUS_SECURITY_ERROR,
    ZB_WPAN_STATUS_FRAME_TOO_LONG = mcp_STATUS_FRAME_TOO_LONG,
    ZB_WPAN_STATUS_INVALID_GTS = mcp_STATUS_INVALID_GTS,
    ZB_WPAN_STATUS_INVALID_HANDLE = mcp_STATUS_INVALID_HANDLE,
    ZB_WPAN_STATUS_INVALID_PARAMETER = mcp_STATUS_INVALID_PARAMETER,
    ZB_WPAN_STATUS_NO_ACK = mcp_STATUS_NO_ACK,
    ZB_WPAN_STATUS_NO_BEACON = mcp_STATUS_NO_BEACON,
    ZB_WPAN_STATUS_NO_DATA = mcp_STATUS_NO_DATA,
    ZB_WPAN_STATUS_NO_SHORT_ADDRESS = mcp_STATUS_NO_SHORT_ADDRESS,
    ZB_WPAN_STATUS_OUT_OF_CAP = mcp_STATUS_OUT_OF_CAP,
    ZB_WPAN_STATUS_PAN_ID_CONFLICT = mcp_STATUS_PAN_ID_CONFLICT,
    ZB_WPAN_STATUS_REALIGNMENT = mcp_STATUS_REALIGNMENT,
    ZB_WPAN_STATUS_TRANSACTION_EXPIRED = mcp_STATUS_TRANSACTION_EXPIRED,
    ZB_WPAN_STATUS_TRANSACTION_OVERFLOW = mcp_STATUS_TRANSACTION_OVERFLOW,
    ZB_WPAN_STATUS_TX_ACTIVE = mcp_STATUS_TX_ACTIVE,
    ZB_WPAN_STATUS_UNAVAILABLE_KEY = mcp_STATUS_UNAVAILABLE_KEY,
    ZB_WPAN_STATUS_UNSUPPORTED_ATTRIBUTE = mcp_STATUS_UNSUPPORTED_ATTRIBUTE,
    ZB_WPAN_STATUS_INVALID_ADDRESS = mcp_STATUS_INVALID_ADDRESS,
    ZB_WPAN_STATUS_ON_TIME_TOO_LONG = mcp_STATUS_ON_TIME_TOO_LONG,
    ZB_WPAN_STATUS_PAST_TIME = mcp_STATUS_PAST_TIME,
    ZB_WPAN_STATUS_TRACKING_OFF = mcp_STATUS_TRACKING_OFF,
    ZB_WPAN_STATUS_INVALID_INDEX = mcp_STATUS_INVALID_INDEX,
    ZB_WPAN_STATUS_LIMIT_REACHED = mcp_STATUS_LIMIT_REACHED,
    ZB_WPAN_STATUS_READ_ONLY = mcp_STATUS_READ_ONLY,
    ZB_WPAN_STATUS_SCAN_IN_PROGRESS = mcp_STATUS_SCAN_IN_PROGRESS,
    ZB_WPAN_STATUS_SUPERFRAME_OVERLAP = mcp_STATUS_SUPERFRAME_OVERLAP,
    ZB_WPAN_STATUS_DRIVER_ERROR = mcp_STATUS_DRIVER_ERROR,
    ZB_WPAN_STATUS_DEVICE_ERROR = mcp_STATUS_DEVICE_ERROR
};

/* Trust Center Swap Out status codes */
enum ZbTcsoStatusT {
    ZB_TCSO_STATUS_SUCCESS = 0x00, /* Successfully performed TCSO with new TC */
    ZB_TCSO_STATUS_DISCOVERY_UNDERWAY, /* ZCL Keepalive has initiated TCSO, because of three consecutive failures. */
    ZB_TCSO_STATUS_REJOIN_PREV, /* Found previous TC and rejoined to it. */
    ZB_TCSO_STATUS_NOT_FOUND, /* Didn't find new or old TC, or process was aborted, resumed previous operation. */
    ZB_TCSO_STATUS_FATAL /* TCSO failed and unable to restore previous operation. */
};

/* Debugging log mask. */
#define ZB_LOG_MASK_FATAL               0x00000001U /* Unrecoverable errors. */
#define ZB_LOG_MASK_ERROR               0x00000002U /* Recoverable internal errors. */
#define ZB_LOG_MASK_WARNING             0x00000004U /* Misused API calls, network problems, etc... */
#define ZB_LOG_MASK_INFO                0x00000008U /* Basic debugging info. Less verbose than ZB_LOG_MASK_DEBUG. */
#define ZB_LOG_MASK_DEBUG               0x00000010U /* General debug info */
#define ZB_LOG_MASK_PERSIST             0x00000020U /* Persistence */
/* NWK */
#define ZB_LOG_MASK_NWK_ROUTING         0x00000040U /* Network routing details. */
#define ZB_LOG_MASK_NWK_LQI             0x00000080U /* Network link status and lqi updates. */
#define ZB_LOG_MASK_NWK_SECURITY        0x00000100U /* Network security. */
#define ZB_LOG_MASK_NWK_ADDR_MAP        0x00000200U /* Network address map changes */
/* APS */
#define ZB_LOG_MASK_APS_SEND            0x00000400U /* APS packet transmission */
#define ZB_LOG_MASK_APS_FRAG            0x00000800U /* APS fragmentation debugging */
/* ZDO */
#define ZB_LOG_MASK_ZDO_ANNCE           0x00001000U /* Print on reception of ZDO Device_Annce */
/* ZCL */
#define ZB_LOG_MASK_ZCL                 0x00002000U
/* Green Power */
#define ZB_LOG_MASK_GREENPOWER          0x00004000U
/* Reserved                             0x0fffc000U */
/* MAC */
#define ZB_LOG_MASK_MAC_RSSI            0x10000000U /* Print debug message per MCPS-DATA.indication showing RSSI */

/* Log mask helpers */
#define ZB_LOG_MASK_LEVEL_0             0x00000000U
#define ZB_LOG_MASK_LEVEL_1             (ZB_LOG_MASK_FATAL)
#define ZB_LOG_MASK_LEVEL_2             (ZB_LOG_MASK_LEVEL_1 | ZB_LOG_MASK_ERROR)
#define ZB_LOG_MASK_LEVEL_3             (ZB_LOG_MASK_LEVEL_2 | ZB_LOG_MASK_WARNING)
#define ZB_LOG_MASK_LEVEL_4             (ZB_LOG_MASK_LEVEL_3 | ZB_LOG_MASK_INFO)
#define ZB_LOG_MASK_LEVEL_5             (ZB_LOG_MASK_LEVEL_4 | ZB_LOG_MASK_DEBUG)
#define ZB_LOG_MASK_LEVEL_6             (ZB_LOG_MASK_LEVEL_5 | ZB_LOG_MASK_ZCL)
#define ZB_LOG_MASK_LEVEL_ALL           0xFFFFFFFFU

/* The Exegin manufacturer ID. */
#define ZB_MFG_CODE_EXEGIN              0x10D7U
#define ZB_MFG_CODE_WILDCARD            0xFFFFU

/*---------------------------------------------------------------
 * Channel List Structure
 *---------------------------------------------------------------
 */
/* arbitrary maximum value - the most should ever 5 =>
 * one 2.4 GHz mask + four GB-868 masks */
#define MAX_CHANNEL_LIST_ENTRIES        8U

struct ZbChannelListT {
    /* Number of channel masks in 'list' */
    uint8_t count;
    struct {
        /* 802.15.4 Channel Page */
        uint8_t page;
        /* e.g. WPAN_CHANNELMASK_2400MHZ or ZB_CHANNELMASK_2400MHZ_HA for Page 0. */
        uint32_t channelMask;
    } list[MAX_CHANNEL_LIST_ENTRIES];
};

/*---------------------------------------------------------------
 * Stack Initialization
 *---------------------------------------------------------------
 */
/* Opaque stack structures. */
struct ZigBeeT;
struct ZbMsgFilterT;

/* A pointer to this struct type is passed to ZbInit to define the various
 * ZigBee tables used in the stack. If the pointer to ZbInit is NULL, the
 * default sizes are used. */
typedef struct {
    /* NWK Table Sizes */
    unsigned int nwkNeighborTblSz; /* Default: 64 */
    unsigned int nwkRouteTblSz; /* Default: 32 */
    unsigned int nwkAddrMapTblSz; /* Default: 32 */
    unsigned int nwkBttSz; /* Default is 32 */
    unsigned int nwkRReqSz; /* default 16 */
#if 0 /* TODO? */
    unsigned int nwkRRecBits; /* default 10 */
#endif

    /* APS Table Sizes */
    unsigned int apsPeerLinkKeyTblSz; /* Default: 32 */
#if 0 /* TODO? */
    unsigned int aps_binding_table_size; /* Default: 64 */
    unsigned int aps_group_table_size; /* Default: 16 */
#endif
} ZbInitTblSizesT;

/* Same parameters as ZbSetLogging takes. Allows debug log output
 * as stack is being initialized. */
typedef struct {
    uint32_t mask;
    void (*func)(struct ZigBeeT *zb, uint32_t mask, const char *hdr,
        const char *fmt, va_list argptr);
} ZbInitSetLoggingT;

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbInit
 *
 *  DESC
 *      Allocates a new Zigbee stack instance.
 *
 *  PARAMS
 *      extAddr         ; The extended address for this stack instance.
 *      tblSizes        ; defines various table sizes with the stack.
 *                        If NULL, default values are used.
 *      setLogging      ; calls ZbSetLogging with info provided.
 *                        Allows debug output during ZbInit process.
 *                        If NULL, ZbSetLogging is not called.
 *
 *  RETURNS
 *      Pointer to allocated ZigBee stack instance. Used to reference the stack instance in
 *      many of the function calls to and from the stack.
 */
struct ZigBeeT * ZbInit(uint64_t extAddr, ZbInitTblSizesT *tblSizes, ZbInitSetLoggingT *setLogging);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbDestroy
 *  DESC
 *      Deallocates a Zigbee stack instance.
 *  PARAMS
 *      none
 *  RETURNS
 *      none
 *----------------------------------------------------------------
 */
void ZbDestroy(struct ZigBeeT *zb);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbSeedRand
 *  DESC
 *      Help seed the stack's PRNG. If the data has real entropy, set the has_entropy flag
 *      to true.
 *  PARAMS
 *      none
 *  RETURNS
 *      none
 *----------------------------------------------------------------
 */
void ZbSeedRand(struct ZigBeeT *zb, uint8_t *randBuf, unsigned int len, bool has_entropy);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbTimerWork
 *  DESC
 *      Non-blocking, must be called periodically to run the stack. Returns the length of
 *      time (in milliseconds) until the next scheduled timer will elapse, or zero if there
 *      are no scheduled timers.
 *  PARAMS
 *      none
 *  RETURNS
 *      none
 *----------------------------------------------------------------
 */
void ZbTimerWork(struct ZigBeeT *zb);
unsigned int ZbCheckTime(struct ZigBeeT *zb);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbWakeupCallbackConfig
 *  DESC
 *      Configure a callback to wakeup the application if there's a new stack event to
 *      process. Not all stack ports require this.
 *  PARAMS
 *      none
 *  RETURNS
 *      none
 *----------------------------------------------------------------
 */
void ZbWakeupCallbackConfig(struct ZigBeeT *zb, void (*wakeup_cb)(void));

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbPortStackEventFd
 *  DESC
 *      Called to get the file descriptor to be used to wake-up the stack thread that is
 *      calling ZbTimerWork if something in the stack needs attention. This is only required
 *      in multi-threaded environments. Without this event, it is possible for a user thread
 *      to initiate a stack function which* doesn't tickle the MAC layer, which in turn would
 *      wake up the stack thread.
 *  PARAMS
 *      none
 *  RETURNS
 *      none
 *----------------------------------------------------------------
 */
int ZbPortStackEventFd(struct ZigBeeT *zb);

void ZbChangeExtAddr(struct ZigBeeT *zb, uint64_t extAddr);

struct ZbNlmeLeaveConfT;
enum ZbStatusCodeT ZB_WARN_UNUSED ZbLeaveReq(struct ZigBeeT *zb, void (*callback)(struct ZbNlmeLeaveConfT *conf, void *arg), void *cbarg);
void ZbLeaveWait(struct ZigBeeT *zb);

/* Helper function to perform an APS and NWK reset */
void ZbReset(struct ZigBeeT *zb);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbIfAttach
 *  DESC
 *      Attaches an IEEE 802.15.4 device driver to the ZigBee stack. Uses the link pointers
 *      within the device structure for linking.
 *  PARAMS
 *      zb              ; ZigBee stack instance pointer
 *      dev             ; pointer to driver instance to attach.
 *  RETURNS
 *      true (1) or false (0)
 */
bool ZbIfAttach(struct ZigBeeT *zb, struct WpanPublicT *dev);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbIfDetach
 *  DESC
 *      Detaches an IEEE 802.15.4 device driver from the ZigBee stack.
 *  PARAMS
 *      zb          ; ZigBee stack instance pointer
 *      dev         ; pointer to driver instance to detach.
 *  RETURNS
 *      none
 */
void ZbIfDetach(struct ZigBeeT *zb, struct WpanPublicT *dev);

/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbSetLogging
 *  DESC
 *      Specifies the level of logging to use, and a callback that outputs the log information.
 *  PARAMS
 *      zb          ; ZigBee stack instance pointer
 *      mask        ; Log mask of the debug messages you wish to receive.
 *  RETURNS
 *      none
 *---------------------------------------------------------------
 */
void ZbSetLogging(struct ZigBeeT *zb, uint32_t mask,
    void (*func)(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argptr));

void ZbGetLogging(struct ZigBeeT *zb, uint32_t *mask,
    void(**func)(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argptr));

/*---------------------------------------------------------------
 * ZCL Basic Server API
 *---------------------------------------------------------------
 */
/* Controls whether the Basic Server is allowed to process the ZCL_BASIC_RESET_FACTORY command. */
void ZbZclBasicServerResetCmdConfig(struct ZigBeeT *zb, bool allow_reset);
/* Write to the local attributes (e.g. ZCL_BASIC_ATTR_MFR_NAME) */
enum ZclStatusCodeT ZbZclBasicWriteDirect(struct ZigBeeT *zb, uint8_t endpoint, uint16_t attributeId, const void *ptr, unsigned int len);
/* Post an alarm code to the Basic Cluster */
bool ZbZclBasicPostAlarm(struct ZigBeeT *zb, uint8_t endpoint, uint8_t alarm_code);

/*---------------------------------------------------------------
 * Uptime
 *---------------------------------------------------------------
 */
typedef unsigned long ZbUptimeT;

#define ZB_UPTIME_MAX               (ULONG_MAX)

/* Max timeout = 12 days
 * FIXME - should probably limit this to on the order of an hour.
 *
 * For long timeouts, keep resetting a timer to a timeout of an hour
 * or the difference between the desired timeout and the current time,
 * whichever is less. And use a real-time clock to get accurate uptime. */
#define ZB_TIMEOUT_MAX              ((UINT_MAX) / 4U)

/* Returns current uptime in milliseconds.
 * Special behaviour is that it never returns 0. Values are from 1 to ZB_UPTIME_MAX. */
ZbUptimeT ZbUptime(void);

unsigned int ZbTimeoutRemaining(ZbUptimeT now, ZbUptimeT expire_time);

/*---------------------------------------------------------------
 * ZigBee Timer
 *---------------------------------------------------------------
 */
/* Opaque timer structure. */
struct ZbTimerT;

/* Creates a ZigBee timer structure. */
struct ZbTimerT * ZbTimerAlloc(struct ZigBeeT *zb, void (*callback)(struct ZigBeeT *zb, void *cb_arg), void *arg);
void ZbTimerChangeCallback(struct ZbTimerT *timer, void (*callback)(struct ZigBeeT *zb, void *cb_arg), void *arg);
void ZbTimerStop(struct ZbTimerT *timer);
void ZbTimerFree(struct ZbTimerT *timer);
/* Resets and schedules a ZigBee timer. */
void ZbTimerReset(struct ZbTimerT *timer, unsigned int timeout);
bool ZbTimerRunning(struct ZbTimerT *timer);
/* ZbTimerRemaining returns time remaining in mS for given timer, or UINT_MAX if timer is not running. */
unsigned int ZbTimerRemaining(struct ZbTimerT *timer);

/*---------------------------------------------------------------
 * Asynchronous messaging filter API
 *---------------------------------------------------------------
 */
/* Asynchronous message filter mask. */
/* NWK Indications */
#define ZB_MSG_FILTER_JOIN_IND                  0x00000001U /* NLME-JOIN.indication (ZbNlmeJoinIndT) */
#define ZB_MSG_FILTER_LEAVE_IND                 0x00000002U /* NLME-LEAVE.indication (ZbNlmeLeaveIndT) */
#define ZB_MSG_FILTER_STATUS_IND                0x00000004U /* NLME-NETWORK-STATUS.indication (ZbNlmeNetworkStatusIndT) */
/* APS Indications */
#define ZB_MSG_FILTER_TRANSPORT_KEY_IND         0x00000008U /* APSME-TRANSPORT-KEY.indication (ZbApsmeTransKeyIndT) */
#define ZB_MSG_FILTER_UPDATE_DEVICE_IND         0x00000010U /* APSME-UPDATE-DEVICE.indication (ZbApsmeUpdateDeviceIndT) */
#define ZB_MSG_FILTER_REMOVE_DEVICE_IND         0x00000020U /* APSME-REMOVE-DEVICE.indication (ZbApsmeRemoveDeviceIndT) */
#define ZB_MSG_FILTER_REQUEST_KEY_IND           0x00000040U /* APSME-REQUEST-KEY.indication (ZbApsmeRequestKeyIndT) */
#define ZB_MSG_FILTER_SWITCH_KEY_IND            0x00000080U /* APSME-SWITCH-KEY.indication (ZbApsmeSwitchKeyIndT) */
#define ZB_MSG_FILTER_VERIFY_KEY_IND            0x00000100U /* APSME-VERIFY-KEY.indication (ZbApsmeVerifyKeyIndT) */
#define ZB_MSG_FILTER_CONFIRM_KEY_IND           0x00000200U /* APSME-CONFIRM-KEY.indication (ZbApsmeConfirmKeyIndT) */
/* Data Indications */
#define ZB_MSG_FILTER_MCPS_DATA_IND             0x00000400U /* MCPS-DATA.indication (struct wpan_data_ind) */
#define ZB_MSG_FILTER_NLDE_DATA_IND             0x00000800U /* NLDE-DATA.indication (ZbNldeDataIndT) */
#define ZB_MSG_FILTER_APSDE_DATA_IND            0x00001000U /* APSDE-DATA.indication (ZbApsdeDataIndT) */
/* Startup Indications */
#define ZB_MSG_FILTER_STARTUP_IND               0x00002000U /* (struct ZbMsgStartupInd) */
/* Note, max filter bit we can specify here is  0x00008000U */

/* Groups of messages that are filterable. */
#define ZB_MSG_FILTER_NLME \
    (ZB_MSG_FILTER_JOIN_IND | ZB_MSG_FILTER_LEAVE_IND | ZB_MSG_FILTER_STATUS_IND)

#define ZB_MSG_FILTER_APSME \
    (ZB_MSG_FILTER_TRANSPORT_KEY_IND | ZB_MSG_FILTER_UPDATE_DEVICE_IND | ZB_MSG_FILTER_REMOVE_DEVICE_IND | \
     ZB_MSG_FILTER_REQUEST_KEY_IND | ZB_MSG_FILTER_SWITCH_KEY_IND | ZB_MSG_FILTER_VERIFY_KEY_IND | \
     ZB_MSG_FILTER_CONFIRM_KEY_IND)

/* Message filter priorities (255 = highest, 0 = lowest) */
#define ZB_MSG_INTERNAL_PRIO                    128 /* default stack priority */
#define ZB_MSG_DEFAULT_PRIO                     64 /* default application priority */

/* Message filter return values. */
#define ZB_MSG_CONTINUE                         0 /* Continue processing any further filter callbacks. */
#define ZB_MSG_DISCARD                          1 /* Stop processing further filter callbacks. */

struct ZbApsFilterT;
struct ZbMsgFilterT * ZbMsgFilterRegister(struct ZigBeeT *zb, uint32_t mask, uint8_t prio,
    int (*callback)(struct ZigBeeT *zb, uint32_t id, void *msg, void *cbarg), void *arg);
void ZbMsgFilterRemove(struct ZigBeeT *zb, struct ZbMsgFilterT *filter);

struct ZbMsgStartupInd {
    enum ZbStatusCodeT status;
};

/*---------------------------------------------------------
 * Persistence
 *---------------------------------------------------------
 */
unsigned int ZbPersistGet(struct ZigBeeT *zb, uint8_t *buf, unsigned int maxlen);
bool ZbPersistNotifyRegister(struct ZigBeeT *zb, void (*callback)(struct ZigBeeT *zb, void *cbarg), void *cbarg);

/*---------------------------------------------------------
 * ZED Shutdown
 *---------------------------------------------------------
 */
/*FUNCTION:------------------------------------------------------
 *  NAME
 *      ZbShutdown
 *  DESC
 *      This API moves the stack to shutdown mode, used in case of
 *      a sleepy end device to conserve power.
 *  PARAMS
 *      zb          ; ZigBee stack instance pointer
 *      mask        ; Log mask of the debug messages you wish to receive.
 *  RETURNS
 *      none
 *---------------------------------------------------------------
 */
void ZbShutdown(struct ZigBeeT *zb);

/*---------------------------------------------------------------
 * Test Case Hooks
 *---------------------------------------------------------------
 */
/* These represent bits in a 32-bit bitmask. */
enum ZbTestcaseT {
    ZB_TESTCASE_NONE = 0,
    ZB_TESTCASE_SE1X_15_47, /* Server sends a truncated INITIATE_KEY response */
    ZB_TESTCASE_SE1X_15_48, /* Client sends a truncated EPHEMERAL_DATA request */
    ZB_TESTCASE_CBKE_DELAY_EPH_DATA, /* e.g. SE 1.4 test case 15.25 */
    ZB_TESTCASE_CBKE_DELAY_RESPONSE, /* e.g. SE 1.4 test case 15.26 */
    ZB_TESTCASE_GB868_EBR_06, /* Bogus EBR */
    /* Override tx power management in the MAC and always set a minimum
     * power level. Useful for co-ax testing to prevent cross-talk. */
    ZB_TESTCASE_GB868_MIN_TX_POWER,
    ZB_TESTCASE_LINKPOWER_DROP_NOTIFY,
    ZB_TESTCASE_LINKPOWER_DROP_REQUEST,
    ZB_TESTCASE_REJOIN_DROP_RSP,
    ZB_TESTCASE_REJOIN_RETURN_FULL,
    ZB_TESTCASE_EDKA_DROP_REQUEST,
    ZB_TESTCASE_ASSOC_RSP_FULL,
    ZB_TESTCASE_TOUCHLINK_DEBUG_KEY,
    ZB_TESTCASE_ZED_STACK_SHUTDOWN
};

/* External API */
void ZbTestCaseEnable(struct ZigBeeT *zb, enum ZbTestcaseT testcase);
void ZbTestCaseDisable(struct ZigBeeT *zb, enum ZbTestcaseT testcase);
void ZbTestCaseClear(struct ZigBeeT *zb);
uint32_t ZbTestCaseCurrent(struct ZigBeeT *zb);

/* Should only be required for the stack */
bool ZbTestCaseIsEnabled(struct ZigBeeT *zb, enum ZbTestcaseT testcase);

/*---------------------------------------------------------------
 * Misc. Helper Functions
 *---------------------------------------------------------------
 */
uint64_t ZbExtendedAddress(struct ZigBeeT *zb);
uint16_t ZbShortAddress(struct ZigBeeT *zb);

/* Total memory allocated from the heap. */
unsigned int ZbMallocTotalSz(void);

/* Memory allocated from the internal ZigBee heap. */
unsigned long ZbHeapUsed(struct ZigBeeT *zb);

unsigned long ZbHeapHighWaterMark(struct ZigBeeT *zb);

/*
 * Dumps outstanding memory allocation information.
 * If printfCb is NULL, output is sent to log output provided by
 * ZbSetLogging.
 */
typedef void (*ZbHeapDumpCallbackT)(void *cbarg, const char *fmt, ...);
void ZbHeapDumpMemAllocTbl(struct ZigBeeT *zb, ZbHeapDumpCallbackT callback, void *cbarg);

/*---------------------------------------------------------------
 * Additional Layer Includes
 *---------------------------------------------------------------
 */
#include "zigbee.security.h"
#include "zigbee.bdb.h"
#include "zigbee.aps.h"
#include "zigbee.nwk.h"
#include "zigbee.startup.h"
#include "zigbee.zdo.h"

#endif /* ZIGBEE_H */
