#ifndef  __SENTRY_MESSAGE_H_
#define  __SENTRY_MESSAGE_H_

#include <string>

#include "json/json.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace sentry {
    class SentryMessage
    {
        private:
            std::string title;
            std::string message;
            std::string level;
            std::string id;
            nlohmann::json extra;

            boost::uuids::random_generator generator;

        public:
            /* Constructors */

            /* Level can be one of the following (as per Sentry API)
             * "fatal"
             * "info"
             * "debug"
             * "warning"
             * "error"
             */
            SentryMessage(std::string _message, std::string _level)
                : message(_message), level(_level)
            {
                this->id = this->uuid4();
            }

            /* To generate unique message id */
            std::string uuid4()
            {
                boost::uuids::uuid uuid = generator();
                std::string str_uuid = to_string(uuid);

                /* Remove '-' from the uuid. Sentry complains about max length */
                boost::erase_all(str_uuid, "-");
                return str_uuid;
            }

            /* Getters */
            std::string getMessage()
            {
                return this->message;
            }

            std::string getLevel()
            {
                return this->level;
            }

            std::string getTitle()
            {
                if (!this->title.length())
                    return this->message;
                return this->title;
            }

            nlohmann::json getExtra()
            {
                return this->extra;
            }

            std::string getID()
            {
                return this->id;
            }

            /* Setters */
            void setTitle(std::string _title)
            {
                this->title = _title;
            }

            /* to add extra data */
            template <class Value>
                void add_extra(const std::string& key, const Value& value)
                {
                    this->extra[key] = value;
                }

            /* Destructor */
            ~SentryMessage() {}
    };
}
#endif
