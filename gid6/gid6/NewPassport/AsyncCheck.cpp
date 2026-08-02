#include "stdafx.h"
#include "MainFrm.h"
#include <thread>
#include <future>
#include <iostream>
//#include "async-await/async_task.hpp"
#include "sstream"
//using namespace zb;
using namespace std;

void CGidrView::OnAsyncCheck()
{
#if 0
    int count = 10;
    CString s = "";
    std::async(std::launch::async, [&count, &s] {
        for (int i = 0; i < count; ++i) {
            std::cout << 1;
            s += "1";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        });
    std::async(std::launch::async, [&count, &s] {
        for (int i = 0; i < count; ++i) {
            std::cout << 2;
            s += "2";
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        });
    

    auto tk = task::async([] {
        this_thread::sleep_for(10000ms);
        std::stringstream ss;
        ss << std::this_thread::get_id();
        uint64_t id = std::stoull(ss.str());
        CString s1;
        s1.Format("I'll go second on thread %d", id);
        })
        .await([&]()->void {
            std::stringstream ss;
            ss << std::this_thread::get_id();
            uint64_t id = std::stoull(ss.str());
            CString s1;
            s1.Format("I'll go second on thread %d", id);
            AfxMessageBox("Result is ");
            });
   
    std::stringstream ss;
    ss << std::this_thread::get_id();
    uint64_t id = std::stoull(ss.str());
    CString s1;
    s1.Format("I'll go first on thread %d", id);
#endif
}