#include <sstream>
#include "Email.h"
#include <string.h>
std::vector<std::string> payload_text;

Email::Email(){
  subject = std::string("Subject: ");
}

void Email::setSubject(const std::string& sub){
  subject +=   sub + std::string("\r\n");
}
void Email::setBody(const std::string& bdy){
  body = "\r\n"+bdy+"\r\n";

}
void Email::setSender(const std::string& sndr){
  sender = sndr;

}

void Email::setCC(const std::string& copyTo){
  cc.push_back(copyTo);

}
void Email::setReceiver(const std::string& rcvr){
  receiver = rcvr;
}
void Email::setAccount(const std::string& usr, const std::string& pass){
  user = usr;
  password = pass;
}
void Email::setEmailServer(const std::string& ssmtp, int port){
  std::stringstream ss;
  ss << ssmtp <<":"<<port;
  smtp = ss.str();
}


size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
 int *lines = (int *)userp;

 if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
   return 0;
 }
 const char *data = payload_text[*lines].c_str();

 if(*lines < payload_text.size()) {
   size_t len = strlen(data);
   memcpy(ptr, data, len);
   (*lines)++;

   return len;
 }

 return 0;
}

void Email::buildPayload(){
  payload_text.push_back(subject);
  payload_text.push_back(body);
}

int Email::sendMail(){
  buildPayload();
  CURL *curl;
  CURLcode res = CURLE_OK;

  int lines = 0;
  lines_read = &lines;

  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, smtp.c_str());

    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    //curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/certificate.pem");

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sender.c_str());

    struct curl_slist *recipients = NULL;

    recipients = curl_slist_append(recipients, receiver.c_str());
    for(int i=0;i<cc.size();i++)
      recipients = curl_slist_append(recipients, cc[i].c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, lines_read);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* Free the list of recipients */
    curl_slist_free_all(recipients);

    /* Always cleanup */
    curl_easy_cleanup(curl);
  }

  return (int)res;

}
