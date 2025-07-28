//
// Created by guita on 25.07.2025.
//

#ifndef ZMQ_H
#define ZMQ_H

class ZmQBackendBase
{
public:
    // inline static socket
    // inline static context; ....
    static void init()
    {
        // create Socket instance

        //register callback to Handle

        //
    }
};

template <typename T>
class ZmQBackend :public ZmQBackendBase
{
public:
    static void init()
    {
        //register callback to Handle like
        ZmQBackendBase::socket.subscribe([](){},filter = T::event_name);
        //
    }
    static void emit(std::shared_ptr<T> event)
    {
        //prepare message
        auto eventname = T::event_name;
        socket_send(event_name+event);
    }
};
#endif //ZMQ_H
