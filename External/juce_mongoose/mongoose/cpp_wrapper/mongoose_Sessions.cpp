
static char charset[] = "abcdeghijklmnpqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
#define CHARSET_SIZE (sizeof(charset)/sizeof(char))

namespace Mongoose
{
    Sessions::Sessions(std::string key_)
        : sessions(), key(key_)
    {
    }
	
	Sessions::~Sessions()
	{
		std::map<std::string, Session *>::iterator it;
		for (it=sessions.begin(); it!=sessions.end(); it++) {
			delete (*it).second;
		}
	}

    std::string Sessions::getId(Request &request, Response &response)
    {
        if (request.hasCookie(key)) {
            return request.getCookie(key);
        } else {
            std::ostringstream newCookie;
            int i;

            for (i=0; i<30; i++) {
                newCookie << charset[(size_t) ((size_t)rand()%CHARSET_SIZE)];
            }

            response.setCookie(key, newCookie.str());

            return newCookie.str();
        }
    }

    Session &Sessions::get(Request &request, Response &response)
    { 
        std::string id = getId(request, response);
		Session *session = NULL;
        
        mutex.lock();
		if (sessions.find(id) != sessions.end()) {
			session = sessions[id];
		} else {
			session = new Session();
			sessions[id] = session;
		}
        mutex.unlock();

        return *session;
    }

    void Sessions::garbageCollect(int oldAge)
    {
        std::vector<std::string> deleteList;
        std::map<std::string, Session*>::iterator it;
        std::vector<std::string>::iterator vit;

        mutex.lock();
        for (it=sessions.begin(); it!=sessions.end(); it++) {
            std::string name = (*it).first;
            Session *session = (*it).second;

            if (session->getAge() > oldAge) {
				delete session;
                deleteList.push_back(name);
            }
        }

        for (vit=deleteList.begin(); vit!=deleteList.end(); vit++) {
            sessions.erase(*vit);
        }
        mutex.unlock();
    }
}
