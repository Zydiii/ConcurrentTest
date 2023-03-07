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
	thread_guard g(my_thread); // 使用 RAII 能保证本线程结束时调用 my_thread.join()

	char buffer[1024] = "abcdefghijkl";
	std::thread my_thread2(f, i, std::string(buffer)); // 提前转化为所需的参数 std::string，仿真隐式转换还没有结束该线程就退出了
	my_thread2.detach();

	std::thread my_thread3(f_object_ref, std::ref(my_func)); // 直接复制 my_func 会将其作为 move-only 的右值形式传递，但需要接受非 const 引用，使用 std::ref 生成引用
	my_thread3.detach();

	std::thread my_thread4(&func::do_func, &my_func); // 指定成员函数需要传入成员函数指针和类对象
	my_thread4.detach();

	std::unique_ptr<func> f = std::make_unique<func>(my_func);
	std::thread my_thread5(f_unique_ptr, std::move(f)); // unique_ptr 只能移动不能复制
	my_thread5.detach();
}

int main() {
	oops();

	return 0;
}