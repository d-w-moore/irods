#ifndef IRODS_RS_GET_RESOURCE_INFO_FOR_OPERATION_HPP
#define IRODS_RS_GET_RESOURCE_INFO_FOR_OPERATION_HPP

struct RsComm;
struct DataObjInp;

int rs_get_resource_info_for_operation(struct RsComm* _rsComm, struct DataObjInp* _dataObjInp, char** _out_info);

#endif // IRODS_RS_GET_RESOURCE_INFO_FOR_OPERATION_HPP
