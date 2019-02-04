
/**
 * A stream response to a request
 */
namespace Mongoose
{
    class JsonResponse : public Response
    {
        public:
            JsonResponse();

            /**
             * Gets the response body
             *
             * @return string the response body
             */
            virtual std::string getBody() override;

            /**
             * Sets the human readability of the response
             *
             * @param bool true for human readable
             */
            void setHuman(bool human);

        protected:
            bool humanReadable;
    };
}
