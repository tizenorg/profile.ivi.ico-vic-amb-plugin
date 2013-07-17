#include <sstream>
#include <string>
#include <vector>

struct JsonData {
    std::string propertyName;
    std::string value;
    double timestamp;
    int sequence;
};

class JsonMessage {
public:
    JsonMessage();
    ~JsonMessage();
    char *
    encode(std::string type_, std::string name_, std::string transactionid_,
           std::vector<JsonData>& data_);
    void
    decode(std::string msg, size_t len);

    std::string
    getType();
    std::string
    getName();
    std::string
    getTransactionid();
    std::vector<JsonData>
    getData();

    static const int BUFSIZE = 256;
protected:
    std::stringstream sstr;

    std::string type;
    std::string name;
    std::string transactionid;
    std::vector<JsonData> data;
    char encodebuf[BUFSIZE];
};
