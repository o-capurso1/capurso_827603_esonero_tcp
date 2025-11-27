#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define SERVER_PORT 56700 // Server port
#define BUFFER_SIZE 512 // Buffer size for messages
#define QUEUE_SIZE 5 // Size of pending connections queue

// Status codes
#define STATUS_OK          0
#define STATUS_CITY_ERR    1
#define STATUS_INVALID_REQ 2

// Supported types
#define TYPE_TEMP 't'
#define TYPE_HUM 'h'
#define TYPE_WIND 'w'
#define TYPE_PRESS 'p'

// Request structure
typedef struct {
    char type; // Weather data type: 't', 'h', 'w', 'p'
    char city[64]; // City name (null-terminated string)
} weather_request_t;

// Response structure
typedef struct {
    unsigned int status; // Response status code
    char type; // Echo of request type
    float value; // Weather data value
} weather_response_t;

// City validation prototype
int is_supported_city(const char*);

// Weather generator prototypes
float get_temperature(void); // -10.0 to 40.0
float get_humidity(void);    // 20.0 to 100.0
float get_wind(void);        // 0.0 to 100.0
float get_pressure(void);    // 950.0 to 1050.0

#endif /* PROTOCOL_H_ */
