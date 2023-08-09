#ifndef IRODS_GET_RESOURCE_INFO_FOR_OPERATION_H
#define IRODS_GET_RESOURCE_INFO_FOR_OPERATION_H

struct RcComm;
struct DataObjInp;

/**
 * \brief Get preferred-resource info for the specified operation.
 * \param[in] _conn - A rcComm_t connection handle to the server.
 * \param[in] _dataObjInp - generic dataObj input. Relevant items are:
 *      \n      \b objPath - the path of the target data object.
 *      \n      \b condInput - conditional Input
 *      \n              -- REPL_NUM_KW  - The replica number of the copy to upload.
 *      \n              -- GET_RESOURCE_INFO_FOR_OPERATION_KW - The *_OPERATION string: "CREATE", "WRITE", "OPEN", or "UNLINK".
 *      \n              -- RESC_NAME_KW - The default destination resource. Only used
 *                              to create a new file, no overwrite of existing files.
 * \param[out] _out_info - a JSON string with keys "host" and "resc_hier", identifying target server and resource hierarchy.
 *                         Example:
 *                         {
 *                             "host": <string>,
 *                             "resc_hier": <string>
 *                         }
 * \return integer
 * \retval 0 on success.
**/
#ifdef __cplusplus
extern "C"
#endif
int rc_get_resource_info_for_operation( struct RcComm *_conn, const struct DataObjInp *_dataObjInp, char **_out_info );

#endif // IRODS_GET_RESOURCE_INFO_FOR_OPERATION_H
