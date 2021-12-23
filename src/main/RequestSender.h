


class RequestSender {

public:
    RequestSender();
    ~RequestSender();
    bool requestToken(const char* server, const char* requestData, char** token);
};
