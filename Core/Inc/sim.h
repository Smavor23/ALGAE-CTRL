extern char ATcommand[200];
typedef struct {
    float latitude;
    char latDirection;
    float longitude;
    char lonDirection;
} Coordinates;

void sendATCommandAndWaitForResponse(const char* command, const char* expectedResponse, uint32_t responseTimeout_ms);
int sendATCommandAndWaitForResponse_signalquality(char* command, char* expectedResponse, uint32_t responseTimeout_ms);
void SIMTransmit(char *cmd);
void SIM_INIT();
Coordinates parseNMEA(char* nmea);
float convertDMSToDecimal(float dms, char direction);
void HTTPConnect(char* data_Json);
void extractData(const char* input, char* output);
void sendATCommandAndWaitForResponse_gps(const char* command, const char* expectedResponse, uint32_t responseTimeout_ms, char* buffer_gps);
void sendATCommandAndWaitForResponse_AT(const char* command, const char* expectedResponse, uint32_t responseTimeout_ms);
