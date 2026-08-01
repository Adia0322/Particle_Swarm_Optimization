
#include "system_comm.h"
#include "cost_function/DIC_comm.h"


struct sys_info *sys_create() {
    struct sys_info *info = malloc(sizeof(struct sys_info));
    if (!info) {
        SYS_DBG("fail to create system memory!\n");
        return NULL;
    } else {
        SYS_DBG("Allocate memory (%p) for info successfully\n", info);
    }
    return info;
}

int sys_clean(sys_info *info) {
    if (!info) {
        SYS_DBG("sys_clean failed!!\n");
        return SYS_FAILURE;
    } else {
        free(info);
    }
    return SYS_SUCCESS;
}