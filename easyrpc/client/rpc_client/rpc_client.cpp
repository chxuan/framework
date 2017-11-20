#include "rpc_client.h"
#include "crpc/utility/logger.h"
#include "crpc/core/codec/client_codec.h"
#include "crpc/client/rpc_client/task_dispatcher.h"

rpc_client::rpc_client()
{
    codec_ = std::make_shared<client_codec>();
    codec_->set_decode_data_callback(std::bind(&rpc_client::decode_data_callback, 
                                               this, std::placeholders::_1));
}

rpc_client::~rpc_client()
{
    stop();
}

bool rpc_client::run()
{
    if (tcp_client::run())
    {
        dispatcher_ = std::make_shared<task_dispatcher>(request_timeout_);
        return true;
    }

    return false;
}

void rpc_client::stop()
{
    tcp_client::stop();
    if (dispatcher_)
    {
        dispatcher_->stop();
    }
}

auto rpc_client::make_recv_handler(const std::function<void(const std::shared_ptr<result>&)>& func)
{
    auto recv_handler = [func](const response_content& body)
    {
        try
        {
            error_code ec(body.code);
            if (!body.message_name.empty())
            {
                if (ec)
                {
                    auto ret = std::make_shared<result>(ec, body.call_id);
                    func(ret);
                }
                else
                {
                    auto ret = std::make_shared<result>(ec, body.call_id,
                                                        protobuf_serialize::unserialize(body.message_name, 
                                                                                        body.body));
                    func(ret);
                }
            }
            else
            {
                if (ec)
                {
                    auto ret = std::make_shared<result>(ec, body.call_id, "");
                    func(ret);
                }
                else
                {
                    auto ret = std::make_shared<result>(ec, body.call_id, body.body);
                    func(ret);
                }
            }
        }
        catch (std::exception& e)
        {
            log_warn() << e.what();
        }
    };

    return recv_handler;
}

unsigned int rpc_client::call(const std::string& func_name, 
                              const std::shared_ptr<google::protobuf::Message>& message, 
                              const std::function<void(const std::shared_ptr<result>&)>& func)
{
    unsigned int serial_num = make_serial_num();
    auto recv_handler = make_recv_handler(func);
    dispatcher_->add_recv_handler(serial_num, recv_handler);

    auto network_data = codec_->encode(serial_num, func_name, message);
    tcp_client::async_write(network_data);

    return serial_num;
}

unsigned int rpc_client::make_serial_num()
{
    static std::atomic<unsigned int> serial_num;
    return ++serial_num;
}

void rpc_client::decode_data_callback(const response_content& body)
{
    dispatcher_->dispatch(body);
}