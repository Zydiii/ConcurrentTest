#include <iostream>
#include <thread>

class thread_guard {
	std::thread& t;
public:
	explicit thread_guard(std::thread &t_) : t(t_) {}
	~thread_guard() {
		if (t.joinable()) {
			t.join();
		}
	}

	thread_guard(thread_guard const&) = delete;
	thread_guard& operator=(thread_guard const&) = delete;
};

struct func {
	int& i;
	func(int &i_) : i (i_) {}
	void operator() () {
		for (unsigned j{ 0 }; j < 10; j++) {
			std::cout << j << " " << i << std::endl;
		}
	}

	void do_func() {
		for (unsigned j{ 0 }; j < 10; j++) {
			std::cout << j << " " << i << std::endl;
		}
	}
};

void f(int i, std::string const& s) {
	std::cout << i << " " << s << std::endl;
}

void f_object_ref(func& f) {
	f();
}

void f_unique_ptr(std::unique_ptr<func> f) {
	f->do_func();
}

void oops() {
	int i{ 1 };
	func my_func(i);
	std::thread my_thread(my_func);
	thread_guard g(my_thread); // ʹ�� RAII �ܱ�֤���߳̽���ʱ���� my_thread.join()

	char buffer[1024] = "abcdefghijkl";
	std::thread my_thread2(f, i, std::string(buffer)); // ��ǰת��Ϊ����Ĳ��� std::string��������ʽת����û�н������߳̾��˳���
	my_thread2.detach();

	std::thread my_thread3(f_object_ref, std::ref(my_func)); // ֱ�Ӹ��� my_func �Ὣ����Ϊ move-only ����ֵ��ʽ���ݣ�����Ҫ���ܷ� const ���ã�ʹ�� std::ref ��������
	my_thread3.detach();

	std::thread my_thread4(&func::do_func, &my_func); // ָ����Ա������Ҫ�����Ա����ָ��������
	my_thread4.detach();

	std::unique_ptr<func> f = std::make_unique<func>(my_func);
	std::thread my_thread5(f_unique_ptr, std::move(f)); // unique_ptr ֻ���ƶ����ܸ���
	my_thread5.detach();
}

int main() {
	oops();

	return 0;
}