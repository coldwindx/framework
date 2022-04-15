#pragma once

#include <memory>
/**
 * @brief 单例模式智能指针封装类
 * @details T 类型
 *          X 为了创造多个实例对应的Tag
 *          N 同一个Tag创造多个实例索引
 */
template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
	/**
	 * @brief 返回单例智能指针
	 */
	static std::shared_ptr<T> instance() {
		static std::shared_ptr<T> v(new T);
		return v;
	}
};
