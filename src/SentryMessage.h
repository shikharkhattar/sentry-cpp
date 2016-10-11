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
        SentryMessage(std::string _title, std::string _level) :
            title(_title),
            message(_title),
            level(_level) {}

        SentryMessage(std::string _title, std::string _message,
                std::string _level) :
            title(_title),
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
            return this->title;
        }

        nlohmann::json getExtra()
        {
            return this->extra;
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
