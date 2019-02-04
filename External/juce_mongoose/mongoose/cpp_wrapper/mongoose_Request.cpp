
static int lowercase(const char *s);
static int mg_strncasecmp(const char *s1, const char *s2, size_t len);

static void mg_strlcpy(register char *dst, register const char *src, size_t n) {
  for (; *src != '\0' && n > 1; n--) {
    *dst++ = *src++;
  }
  *dst = '\0';
}

/*
static int mg_strcasecmp(const char *s1, const char *s2) {
  int diff;

  do {
    diff = lowercase(s1++) - lowercase(s2++);
  } while (diff == 0 && s1[-1] != '\0');

  return diff;
}
*/

static const char *mg_strcasestr(const char *big_str, const char *small_str) {
  int i, big_len = (int) strlen(big_str), small_len = (int) strlen(small_str);

  for (i = 0; i <= big_len - small_len; i++) {
    if (mg_strncasecmp(big_str + i, small_str, (size_t) small_len) == 0) {
      return big_str + i;
    }
  }

  return NULL;
}

static int mg_get_cookie(const char *cookie_header, const char *var_name,
                  char *dst, size_t dst_size) {
  const char *s, *p, *end;
  int name_len, len = -1;

  if (dst == NULL || dst_size == 0) {
    len = -2;
  } else if (var_name == NULL || (s = cookie_header) == NULL) {
    len = -1;
    dst[0] = '\0';
  } else {
    name_len = (int) strlen(var_name);
    end = s + strlen(s);
    dst[0] = '\0';

    for (; (s = mg_strcasestr(s, var_name)) != NULL; s += name_len) {
      if (s[name_len] == '=') {
        s += name_len + 1;
        if ((p = strchr(s, ' ')) == NULL)
          p = end;
        if (p[-1] == ';')
          p--;
        if (*s == '"' && p[-1] == '"' && p > s + 1) {
          s++;
          p--;
        }
        if ((size_t) (p - s) < dst_size) {
          len = (int) (p - s);
          mg_strlcpy(dst, s, (size_t) len + 1);
        } else {
          len = -3;
        }
        break;
      }
    }
  }
  return len;
}

namespace Mongoose
{
    Request::Request(struct mg_connection *connection_) :
        connection(connection_)
    {
        url = std::string(connection->uri);
        method = std::string(connection->request_method);

        // Downloading POST data
        std::ostringstream postData;
        postData.write(connection->content, connection->content_len);
        data = postData.str();
    }

    std::string Request::getUrl()
    {
        return url;
    }

    std::string Request::getMethod()
    {
        return method;
    }

    std::string Request::getData()
    {
        return data;
    }

#ifdef ENABLE_REGEX_URL
    smatch Request::getMatches()
    {
        return matches;
    }

    bool Request::match(string pattern)
    {
        key = method + ":" + url;
        return regex_match(key, matches, regex(pattern));
    }
#endif

    void Request::writeResponse(Response *response)
    {
        std::string responseData = response->getData();
        mg_write(connection, responseData.c_str(), (int) responseData.size());
    }

    bool Request::hasVariable(std::string key)
    {
        const char *dataField;
        char dummy[10];

        // Looking on the query string
        dataField = connection->query_string;
        if (dataField != NULL && mg_get_var(connection, key.c_str(), dummy, 1) != -1) {
            return true;
        }

        return false;
    }

    std::map<std::string, std::string> Request::getAllVariable()
    {
        std::map<std::string, std::string> mapKeyValue;
        std::stringstream ss(data);
        std::string param;
        while(std::getline(ss, param, '&')){ //block for '&'
            const std::string& key = param.substr(0, param.find('='));
            const std::string& value = param.substr(param.find('=')+1);
            mapKeyValue[key] = value; // insert map
        }
        return mapKeyValue;
    }

    bool Request::readVariable(const char *dataToRead, std::string key, std::string &output)
    {
        int size = 1024, ret;
        char *buffer = new char[(size_t) size];

        do {
            ret = mg_get_var(connection, key.c_str(), buffer, (size_t) size);

            if (ret == -1) {
                return false;
            }

            if (ret == -2) {
                size *= 2;
                delete[] buffer;
                buffer = new char[(size_t) size];
            }
        } while (ret == -2);

        output = std::string(buffer);
        delete[] buffer;

        return true;
    }


    std::string Request::get(std::string key, std::string fallback)
    {
        const char *dataField;
        std::string output;

        // Looking on the query string
        dataField = connection->query_string;
        if (dataField != NULL && readVariable(dataField, key, output)) {
            return output;
        }

        // Looking on the POST data
        dataField = data.c_str();
        if (dataField != NULL && readVariable(dataField, key, output)) {
            return output;
        }

        return fallback;
    }

    bool Request::hasCookie(std::string key)
    {
        int i;
        char dummy[10];

        for (i=0; i<connection->num_headers; i++) {
            const struct mg_connection::mg_header *header = &connection->http_headers[i];

            if (strcmp(header->name, "Cookie") == 0) {
                if (mg_get_cookie(header->value, key.c_str(), dummy, sizeof(dummy)) != -1) {
                    return true;
                }
            }
        }

        return false;
    }

    std::string Request::getCookie(std::string key, std::string fallback)
    {
        std::string output;
        int i;
        int size = 1024;
        int ret;
        char *buffer = new char[(size_t) size];
        char dummy[10];
        const char *place = NULL;

        for (i=0; i<connection->num_headers; i++) {
            const struct mg_connection::mg_header *header = &connection->http_headers[i];

            if (strcmp(header->name, "Cookie") == 0) {
                if (mg_get_cookie(header->value, key.c_str(), dummy, sizeof(dummy)) != -1) {
                    place = header->value;
                }
            }
        }

        if (place == NULL) {
            return fallback;
        }

        do {
            ret = mg_get_cookie(place, key.c_str(), buffer, (size_t) size);

            if (ret == -2) {
                size *= 2;
                delete[] buffer;
                buffer = new char[(size_t) size];
            }
        } while (ret == -2);

        output = std::string(buffer);
        delete[] buffer;

        return output;
    }

    std::string Request::getHeaderKeyValue(const std::string& header_key) {
      std::string output;
      for (int i=0; i<connection->num_headers; i++) {
        const struct mg_connection::mg_header *header = &connection->http_headers[i];
        if (strcmp(header->name, header_key.c_str()) == 0) {
            output = header->value;
            break;
        }
      }
      return output;
    }

    void Request::handleUploads()
    {
        char var_name[1024];
        char file_name[1024];
        const char *dataToUpload;
        int data_len;

        if (mg_parse_multipart(connection->content, connection->content_len,
                    var_name, sizeof(var_name), file_name, sizeof(file_name), &dataToUpload, &data_len)) {
            uploadFiles.push_back(UploadFile(std::string(file_name), std::string(dataToUpload, (size_t) data_len)));
        }
    }
}
