#pragma once

#include <memory>
/**
 * @brief ����ģʽ����ָ���װ��
 * @details T ����
 *          X Ϊ�˴�����ʵ����Ӧ��Tag
 *          N ͬһ��Tag������ʵ������
 */
template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
	/**
	 * @brief ���ص�������ָ��
	 */
	static std::shared_ptr<T> instance() {
		static std::shared_ptr<T> v(new T);
		return v;
	}
};
