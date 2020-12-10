/*
 * Copyright © 2018-2019 Thunder Software Technology Co., Ltd.
 * All rights reserved.
 */

#include <mutex>
#include <atomic>
#include <memory>
#include <list>
#include <iostream>
#include "BufManager.h"

#ifdef TT

template<typename T> 
BufManager<T>::BufManager() /*noexcept */ {
	//swap_ready(false);
	swap_ready = false;
}

template<typename T>
BufManager<T>::~BufManager() /*noexcept*/ {
	if (!debug_info.empty() ) {
		printf("BufManager %s destroyed.", debug_info.c_str());
	}
}
template<typename T> 
void BufManager<T>::feed(std::shared_ptr<T> pending) {
	if (nullptr == pending.get()) {
		throw "ERROR: feed an empty buffer to BufManager";
	}
	swap_mtx.lock();
	front_sp = pending;
	swap_ready = true;
		swap_mtx.unlock();
        return;
    }

template<typename T>
std::shared_ptr<T> BufManager<T>::fetch()  /*noexcept */{
        if (swap_ready) {
            swap_mtx.lock();
            std::swap(back_sp, front_sp);
			swap_ready = false;
            swap_mtx.unlock();
        }
        return back_sp;
    }



#endif
