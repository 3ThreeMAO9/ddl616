#include "item_config.h"
#include "config.h"
#define OB_LOG_LEVEL OB_LOG_LEVEL_DEFAULT
#include "ob_log.h"
#define TAG "item_config"

/*************************Variable*************************/
static const device_version_info_t device_version = {
    .client_project = CLIENT_ITEM_SN,
    .outside_version = PATCH_VERSION,
};

// ---------------------------------------------------------
uint16_t readDeviceClientProjectSn(void)
{
    return (uint16_t)(device_version.client_project);
}

uint16_t readDeviceVersion(void)
{
    return (uint16_t)(device_version.outside_version);
}

void read_device_version_info(void)
{
    OB_LOGD(TAG,"client_project: %05u, outside_version: V0.%02u", device_version.client_project, device_version.outside_version);
}

