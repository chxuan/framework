#ifndef _PEOPLEINFOMESSAGE_H
#define _PEOPLEINFOMESSAGE_H

#include "Message.h"

class PeopleInfoMessage
{
public:
    TCP_MESSAGE_HEADER

    std::string m_name;
    unsigned int m_age;
    unsigned int m_sex;
};

namespace boost
{
    namespace serialization
    {
        template<class Archive>
        void serialize(Archive& ar, PeopleInfoMessage& message, const unsigned int version)
        {
            (void)version;
            ar & message.m_name;
            ar & message.m_age;
            ar & message.m_sex;
        }
    }
}

#endif
