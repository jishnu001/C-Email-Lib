#include <vector>
#include <string>
#include <curl/curl.h>

class Email{

public:
  Email();
  void setSubject(const std::string& subject);
  void setBody(const std::string& body);
  void setSender(const std::string& sender);
  void setReceiver(const std::string& receiver);
  void setCC(const std::string& copyTo);
  void setAccount(const std::string& user, const std::string& password);
  void setEmailServer(const std::string& ssmtp, int port);
  void buildPayload();
  int sendMail();

private:
  std::string user;
  std::string password;
  std::string smtp;
  std::string subject;
  std::string body;
  std::string sender;
  std::string receiver;
  std::vector<std::string> cc;
  int* lines_read;
};
