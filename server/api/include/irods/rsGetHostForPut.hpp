#ifndef RS_GET_HOST_FOR_PUT_HPP
#define RS_GET_HOST_FOR_PUT_HPP

#include "irods/rcConnect.h"
#include "irods/dataObjInpOut.h"

int rsGetHostForPut( rsComm_t *rsComm, dataObjInp_t *dataObjInp, char **outHost );
int rs_get_rescinfo_for_put( rsComm_t *rsComm, dataObjInp_t *dataObjInp, char **response );

#endif
