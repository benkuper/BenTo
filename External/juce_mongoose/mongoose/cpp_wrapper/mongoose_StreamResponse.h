
/**
 * A stream response to a request
 */
namespace Mongoose
{
    class StreamResponse : public std::ostringstream, public Response
    {
        public:
            /**
             * Gets the response body
             *
             * @return string the response body
             */
            virtual std::string getBody();
    };
}

