
namespace Mongoose
{
    Session::Session()
    {
        ping();
    }

    void Session::ping()
    {
        mutex.lock();
        date = (int) time(NULL);
        mutex.unlock();
    }

    void Session::setValue(std::string key, std::string value)
    {
        mutex.lock();
        values[key] = value;
        mutex.unlock();
    }

    void Session::unsetValue(std::string key)
    {
        mutex.lock();
        values.erase(key);
        mutex.unlock();
    }

    bool Session::hasValue(std::string key)
    {
        return values.find(key) != values.end();
    }

    std::string Session::get(std::string key, std::string fallback)
    {
        mutex.lock();
        if (hasValue(key)) {
            std::string value = values[key];
            mutex.unlock();

            return value;
        } else {
            mutex.unlock();
            return fallback;
        }
    }

    int Session::getAge()
    {
        return (int) time(NULL) - date;
    }
}
