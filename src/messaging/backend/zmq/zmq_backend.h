//
// Created by guita on 25.07.2025.
//

#ifndef ZMQ_H
#define ZMQ_H

class ZmQBackend
{
public:
    template <typename T>
    void init()
    {
        printf("Initializing ZmQ backend for event type: %s\n", T::event_name.c_str());
    }

    template <typename T>
    void emit(std::shared_ptr<T> event)
    {
        printf("Emitting ZmQ backend for event type: %s\n", T::event_name.c_str());

    }
};
#endif //ZMQ_H
