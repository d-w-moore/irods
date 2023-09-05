#include "irods/get_resource_info_for_operation.h"

#include "irods/getRemoteZoneResc.h"
#include "irods/irods_logger.hpp"
#include "irods/irods_resource_backport.hpp"
#include "irods/irods_resource_redirect.hpp"

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <array>
#include <cstring> // for strdup
#include <string>

int rs_get_resource_info_for_operation(rsComm_t* _rsComm, dataObjInp_t* _dataObjInp, char** _out_info)
{
    using log_api = irods::experimental::log::api;
    rodsServerHost_t* rodsServerHost = nullptr;
    const int remoteFlag = getAndConnRemoteZone(_rsComm, _dataObjInp, &rodsServerHost, REMOTE_OPEN);
    if (remoteFlag < 0) {
        return remoteFlag;
    }

    if (REMOTE_HOST == remoteFlag) {
        return rc_get_resource_info_for_operation(rodsServerHost->conn, _dataObjInp, _out_info);
    }

    std::string hier;
    std::string location;
    const char* op_type = getValByKey(&_dataObjInp->condInput, GET_RESOURCE_INFO_OP_TYPE_KW);
    if (op_type == nullptr) {
        return SYS_INVALID_INPUT_PARAM;
    }
    const std::array allowed_ops{
        irods::CREATE_OPERATION, irods::WRITE_OPERATION, irods::UNLINK_OPERATION, irods::OPEN_OPERATION};
    const auto found_operation = std::find(allowed_ops.begin(), allowed_ops.end(), op_type);
    if (allowed_ops.end() == found_operation) {
        return SYS_INVALID_INPUT_PARAM;
    }
    if (const char * hier_cstr = getValByKey(&_dataObjInp->condInput, RESC_HIER_STR_KW); hier_cstr == nullptr) {
        try {
            auto result = irods::resolve_resource_hierarchy(*found_operation, _rsComm, *_dataObjInp);
            hier = std::get<std::string>(result);
        }
        catch (const irods::exception& e) {
            log_api::error(e.what());
            return e.code();
        }
    } // if keyword
    else {
        hier = hier_cstr;
    }

    // extract the host location from the resource hierarchy
    irods::error ret = irods::get_loc_for_hier_string(hier, location);
    if (!ret.ok()) {
        auto error = PASSMSG(fmt::format("{} - failed in get_loc_for_hier_string", __func__), ret);
        log_api::error(error.result());
        return ret.code();
    }

    nlohmann::json resc_info;
    resc_info["host"] = location;
    resc_info["resc_hier"] = hier;
    *_out_info = strdup(resc_info.dump().c_str());
    return 0;
}
