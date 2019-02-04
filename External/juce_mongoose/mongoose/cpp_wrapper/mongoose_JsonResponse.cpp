

namespace Mongoose
{
    JsonResponse::JsonResponse()
        : humanReadable(false)
    {
    }

    std::string JsonResponse::getBody()
    {
        return "";
//        if (humanReadable) {
//            Json::StyledWriter writer;
//            return writer.write(*this);
//        } else {
//            Json::FastWriter writer;
//            return writer.write(*this);
//        }
    }

    void JsonResponse::setHuman(bool human)
    {
        humanReadable = human;
    }
}
