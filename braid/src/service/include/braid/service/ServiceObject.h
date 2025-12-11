#pragma once
#include <net/IOUringObject.h>
#include <memory>

namespace braid {

	class Service;
	// 서비스 레이어에서 사용하는 기본 오브젝트
	class ServiceObject : public IOUringObject {

	public:
		ServiceObject(std::shared_ptr<Service>& service_instance);
		virtual ~ServiceObject() = default;


	public:
		void request_receive();
		void request_send();
		void request_accept(int accept_fd_);


	public:
		virtual void on_accepted() override;
		virtual void on_received(int bytes_received) override;


	protected:
		const std::weak_ptr<Service> service_instance_;
	};
}