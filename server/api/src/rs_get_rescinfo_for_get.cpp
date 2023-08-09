#include <cstring> // for strdup.
#include <nlohmann/json.hpp> // for strdup.

//// ----------- TODO: make this file
// #include "irods/rs_get_rescinfo_for_put.hpp"

#include "irods/catalog_utilities.hpp"
#include "irods/irods_logger.hpp"
#include "irods/rodsErrorTable.h"

#include "irods/getHostForPut.h" // for definition of macro: THIS_ADDRESS
#include "irods/getHostForGet.h"
#include "irods/rodsLog.h"
#include "irods/rsGlobalExtern.hpp"
#include "irods/rcGlobalExtern.h"
#include "irods/getRemoteZoneResc.h"
#include "irods/dataObjCreate.h"
#include "irods/objMetaOpr.hpp"
#include "irods/resource.hpp"
#include "irods/collection.hpp"
#include "irods/specColl.hpp"
#include "irods/miscServerFunct.hpp"
#include "irods/rsGetHostForGet.hpp"

// =-=-=-=-=-=-=-
#include "irods/irods_resource_backport.hpp"
#include "irods/irods_resource_redirect.hpp"


namespace
{
    using log_api = irods::experimental::log::api;
} // anonymous namespace


auto rs_get_rescinfo_for_get(RsComm* rsComm, dataObjInp_t *dataObjInp, char** _resp) -> int
{
        rodsServerHost_t *rodsServerHost;
        const int remoteFlag = getAndConnRemoteZone(rsComm, dataObjInp, &rodsServerHost, REMOTE_OPEN);
        if (remoteFlag < 0) {
            return remoteFlag;
        }
        else if (REMOTE_HOST == remoteFlag) {
            const int status = rc_get_rescinfo_for_get(rodsServerHost->conn, dataObjInp, _resp);
            if (status < 0) {
                return status;
            }
        }
        else {
            // =-=-=-=-=-=-=-
            // default behavior
            *_resp = strdup( THIS_ADDRESS );

            // =-=-=-=-=-=-=-
            // working on the "home zone", determine if we need to redirect to a different
            // server in this zone for this operation.  if there is a RESC_HIER_STR_KW then
            // we know that the redirection decision has already been made
            if ( isColl( rsComm, dataObjInp->objPath, NULL ) < 0 ) {
                std::string hier{};
                if ( getValByKey( &dataObjInp->condInput, RESC_HIER_STR_KW ) == NULL ) {
                    try {
                        auto result = irods::resolve_resource_hierarchy(irods::OPEN_OPERATION, rsComm, *dataObjInp);
                        hier = std::get<std::string>(result);
                    }
                    catch (const irods::exception& e ) {
                        irods::log(e);
                        return e.code();
                    }
                    addKeyVal( &dataObjInp->condInput, RESC_HIER_STR_KW, hier.c_str() );
                } // if keyword

                // =-=-=-=-=-=-=-
                // extract the host location from the resource hierarchy
                std::string location;
                irods::error ret = irods::get_loc_for_hier_string( hier, location );
                if ( !ret.ok() ) {
                    irods::log( PASSMSG( "rsGetHostForGet - failed in get_loc_for_hier_string", ret ) );
                    return -1;
                }

                // =-=-=-=-=-=-=-
                // set the out variable
                nlohmann::json J;
                J["host"] = location;
                J["resc_hier"] = hier;
                *_resp = strdup( J.dump().c_str() );

            } // if not a collection
        }

        return 0;

} // rs_get_rescinfo_for_get

