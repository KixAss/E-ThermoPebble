#ifndef _config_h

#define _config_h

// Server URL
#define SERVER_URL "http://nh.kixass.net/pebble.php"

// HTTP Cookie codes
#define HTTP_TEMP_ID 1976545

// JSON codes / message types
#define MSG_TYPE 1

#define MSG_TEMP_CURRENT 20
#define MSG_TEMP_STATUS 21
#define MSG_TEMP_ACTION 22

#define MSG_TYPE_INIT 0
#define MSG_TYPE_TEMP 2
//...
#define MSG_TYPE_ERROR 999

#define MSG_ERROR_DESCRIPTION 9999

#define MSG_ACTION_NONE 0
#define MSG_ACTION_DOWN 1
#define MSG_ACTION_UP 2
#define MSG_ACTION_STATUS 3
#define MSG_ACTION_FIXED 4

// Used for httpebble HTTP id
#define ANDROID false

#endif