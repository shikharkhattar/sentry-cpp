#ifndef  __SENTRY_MESSAGE_H_
#define  __SENTRY_MESSAGE_H_

#include <string>

#include "json/json.hpp"


class SentryMessage
{
    private:
        std::string title;
        std::string message;
        std::string level;
        nlohmann::json extra;

    public:
        /* Constructors */
        SentryMessage(std::string _message, std::string _level) :
            message(_message),
            level(_level) {}

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
#endif
