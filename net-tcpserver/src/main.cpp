/*************************************************************************
	> File Name: main.cpp
	> Author: 
	> Mail: 
	> Created Time: 2016年02月03日 星期三 15时06分27秒
 ************************************************************************/

#include <iostream>
#include "TcpServerImpl.h"
#include "PeopleInfoMessage.h"

int main()
{
    std::cout << "Server starting..." << std::endl;
    TcpServerImpl server(8888);
    server.start();

    std::cout << "###############################" << std::endl;

    //std::cin.get();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));

    for (auto& iter : server.allRemoteAddress())
    {
        boost::shared_ptr<PeopleInfoMessage> peopleInfoMessage(new PeopleInfoMessage);
        peopleInfoMessage->m_messageType = 1000;
        peopleInfoMessage->m_name = "Jack";
        peopleInfoMessage->m_age = 20;
        server.asyncWrite(peopleInfoMessage, iter);

        boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

        boost::shared_ptr<PeopleInfoMessage> peopleInfoMessage2(new PeopleInfoMessage);
        peopleInfoMessage2->m_messageType = 2000;
        peopleInfoMessage2->m_name = "Tom";
        peopleInfoMessage2->m_age = 22;
        server.asyncWrite(peopleInfoMessage2, iter);
    }

    boost::this_thread::sleep_for(boost::chrono::milliseconds(5000));

    std::cout << "Server stoped..." << std::endl;

    return 0;
}
