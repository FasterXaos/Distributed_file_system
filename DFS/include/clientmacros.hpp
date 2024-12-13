#pragma once

#define DEFAULT_PORT 1234
#define RECONNECT_INTERVAL 3000
#define RESPONSE_TIMEOUT 3000
#define CHUNK_SIZE 1024

#define CLIENT "CLIENT"
#define MAIN_SERVER "MAIN_SERVER"
#define REPLICA "REPLICA"

#define RESPONSE_LOGIN_SUCCESS "LOGIN_SUCCESS"
#define RESPONSE_LOGIN_FAILED "LOGIN_FAILED"
#define RESPONSE_REGISTER_SUCCESS "REGISTER_SUCCESS"
#define RESPONSE_REGISTER_USER_EXISTS "REGISTER_USER_EXISTS"
#define RESPONSE_REGISTER_FAILED "REGISTER_FAILED"
#define RESPONSE_DOWNLOAD_READY "DOWNLOAD_READY"
#define RESPONSE_DOWNLOAD_FAILED "DOWNLOAD_FAILED"
#define RESPONSE_DELETE_SUCCESS "DELETE_SUCCESS"
#define RESPONSE_DELETE_FAILED "DELETE_FAILED"
#define RESPONSE_UPLOAD_SUCCESS "UPLOAD_SUCCESS"
#define RESPONSE_UPLOAD_FAILED "UPLOAD_FAILED"
#define RESPONSE_READY_FOR_DATA "READY_FOR_DATA"
#define RESPONSE_CHUNK_RECEIVED "CHUNK_RECEIVED"
#define RESPONSE_FILES_LIST "FILES_LIST"

#define COMMAND_LOGIN "LOGIN"
#define COMMAND_REGISTER "REGISTER"
#define COMMAND_DOWNLOAD "DOWNLOAD"
#define COMMAND_UPLOAD "UPLOAD"
#define COMMAND_DELETE "DELETE"
#define COMMAND_GET_FILES "GET_FILES"
