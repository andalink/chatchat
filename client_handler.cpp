#include "client_handler.h"

client_handler::client_handler(int client_fd, unsigned int timeout, unsigned int client_id, receive_callback on_message_receive)
 : client_fd_(client_fd), timeout_(timeout), data_buff_(DATA_BUFF_LEN), client_id_(client_id), on_receive_(on_message_receive),
   terminate_(false), echoing_msg_(false), processing_msg_(true)
{

}

void client_handler::run()
{
    while(!terminate_) {
        unsigned int count = 0;
        try {
            count = read_data();
        } catch (const std::exception &e) {
            terminate_ = true;
            std::cerr << "error while receiving file: " << e.what() << std::endl;
            break;
        }

        if(count > 0) {
            try {
                Message msg = Message::from_raw_data(data_buff_, client_id_);
                // TODO process message here
            } catch (const std::exception &e) {
                std::cerr << "error while processing message: " << e.what() << std::endl;
            }
        } else {
            if(terminate_)
                break;
        }
    }
    close(client_fd_);
}

void client_handler::exit()
{
    terminate_ = true;
}

void send(const Message& message)
{
    // TODO actions to send message
}

unsigned int client_handler::id() const
{
    return client_id_;
}

bool client_handler::terminated() const
{
    return terminate_;
}

std::vector<std::string> client_handler::groups() const
{
    std::lock_guard<std::mutex> lock(groups_lock_);
    return std::vector<std::string>(groups_);
}

bool client_handler::in_group(const std::string &group) const
{
    std::lock_guard<std::mutex> lock(groups_lock_);
    return std::find(groups_.cbegin(), groups_.cend(), group) != groups_.end();
}

bool client_handler::echoing_required() const
{
    return echoing_msg_;
}

bool client_handler::processing_required() const
{
    return processing_msg_;
}

unsigned int client_handler::read_data()
{
    struct pollfd fds;
    fds.fd = client_fd_;
    fds.events = POLLIN;

    int poll_res = poll(&fds, 1, timeout_);

    switch (poll_res) {
    case -1:
        throw std::system_error(std::error_code(errno, std::generic_category()),
                                "error while poll socket");
    case 0:
        break;
    default:
        unsigned count = 0;
        count = read(client_fd_, &data_buff_[0], data_buff_.size());
        if(count > 0)
            return count;
        else {
            std::cout << "socket closed by other side" << std::endl;
            terminate_ = true;
        }
    }
    return 0;
}
