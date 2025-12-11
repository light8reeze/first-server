#pragma once
#include <memory>
#include <type_traits>
#include <cassert>
#include <util/BraidPCH.h>
#include <service/ServiceObject.h>
#include <arpa/inet.h>

namespace braid {
    class Service;

    template<typename ServiceType = Service>
    requires std::is_base_of_v<Service, ServiceType>
    class ServiceBuilder {
    public:
        ServiceBuilder() : service_instance_(std::unique_ptr<ServiceType>(new ServiceType())) {
        }

        virtual ~ServiceBuilder() = default;


    public:
		static ServiceBuilder<ServiceType> create_builder() {
            return ServiceBuilder<ServiceType>{};
		}


    public:
        virtual bool validate() {
            return true;
        }

		std::shared_ptr<ServiceType> build() {
			assert(nullptr != service_instance_);

            if (!validate())
                return nullptr;

            std::shared_ptr<ServiceType> build_service_instance = std::shared_ptr<ServiceType>(std::move(service_instance_));

            std::shared_ptr<ServiceObject> acceptor = build_service_instance->acceptor_object_;
            if (nullptr == acceptor) {
                build_service_instance->acceptor_object_ = std::make_shared<ServiceObject>(build_service_instance);
                acceptor = build_service_instance->acceptor_object_;
            }

            acceptor->set_socket_fd(listen_fd_);
            acceptor->set_address(acceptor_addr_);

            return build_service_instance;
        }

        ServiceBuilder<ServiceType>& set_address(std::string address, int port) {

            listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
            int enable = 1;
            setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
            
            acceptor_addr_.sin_family = AF_INET;
            acceptor_addr_.sin_port = htons(port);

            if (address.empty())
                acceptor_addr_.sin_addr.s_addr = INADDR_ANY;
            else 
                ::inet_pton(AF_INET, address.c_str(), &(acceptor_addr_.sin_addr));

            bind(listen_fd_, (struct sockaddr*)&acceptor_addr_, sizeof(acceptor_addr_));
            listen(listen_fd_, 128);

            return (*this);
        }

        ServiceBuilder<ServiceType>& set_thread_count(int thread_count) {
            service_instance_->thread_count_ = thread_count;
			return (*this);
        }

		ServiceBuilder<ServiceType>& set_session_count(int session_count) {
            service_instance_->session_count_ = session_count;
			return (*this);
		}

		ServiceBuilder<ServiceType>& set_queue_depth_per_thread(int queue_depth) {
            service_instance_->queue_depth_per_thread_ = queue_depth;
			return (*this);
		}


    protected:
        std::unique_ptr<ServiceType> service_instance_ = nullptr;

        int                 listen_fd_ = -1;
        int                 port_ = 0;
        struct sockaddr_in  acceptor_addr_;
    };
}