#include "model.h"
#include <opencv.hpp>
#include <iostream>
#include <string>
#include <windows.h>��
#include <cstdlib>
#include<thread>
#include <regex>
#include <cstdlib>
using namespace std;

#define NUM_THREADS     3

//struct thread_data {
//    cv::VideoCapture cap;
//    int fpscount;
//    int interval;
//    int prob;
//};

void ce(int a)

{
    a = 1;
}
void getppt(vector<cv::Mat> fpsdanchou, vector<double> times,double prob,string imgpath)
//void getppt()
{

    cv::Mat lastimg;
    cv::Mat nowimg;
    lastimg = fpsdanchou[0];
    int fpscount = fpsdanchou.size();
    DWORD t1, t2;
    for (int i = 0; i < fpscount; i++)
    {
        t1 = GetTickCount();
        nowimg = fpsdanchou[i];
        double time = times[i];
        if (nowimg.empty())
        {
            std::cout << "nowimg��֡����ͼ��հ�" << std::endl;
            continue;
        }

        double same = delsame(lastimg, nowimg);
        //double same = ssim(lastimg, nowimg);

        if (same < prob)
        {
            int min = time / 60;
            int sen = (int)time % 60;
            //string imgpath = "D:\\ApowerREC\\20220828_104110\\";
            //imgpath.append()
            cv::imwrite(imgpath + to_string(min) + "_" + to_string(sen) + ".jpg", nowimg);
            lastimg = nowimg.clone();
            std::cout << "�����µ�֡��ʱ��" + to_string(min) + ":" + to_string(sen) << std::endl;

            //cv::imshow("showpic", nowimg);

        }

        t2 = GetTickCount();
        double dantimems = (t2 - t1) * 1.0;
        t1 = t2;
        
        std::cout << this_thread::get_id() <<"����"+to_string(same) + "ѭ��ʱ��" + to_string(dantimems) + "ms" << std::endl;
        
    }
    //return 0;

}


int main()
{
    //����ѡ����
    double invfps = 1;//��֡���,��λ�룬��ֵԽ����ʱ��Խ��
    double prob = 0.9;//ͼ�����ƶȣ���ֵԽ��ɸѡ����Խ��
    string video_path = "D:\\ApowerREC\\20220828_104110.mp4";//��Ƶ·��
    
    string imgpath = "D:\\ApowerREC\\20220828_104110\\";//�ļ���·��
    int numth = NUM_THREADS;//�߳���
    int eachxianread = 100;

    cv::VideoCapture cap = cv::VideoCapture(video_path);
    bool suc = cap.isOpened();
    cv::Mat lastimg;
    cv::Mat nowimg;
    int fps = cap.get(cv::CAP_PROP_FPS);
    int interval = fps * invfps;
    int fpscount = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int fpsreadcount = fpscount / interval;
    //thread* th = new thread(numth);
    thread th[NUM_THREADS];
    vector<vector<cv::Mat>> fpsdanchou_duoxian;
    vector<vector<double>> times_duoxian;
    int xianxhengre = fpscount / (numth * eachxianread * interval);
    int nowfps = 0;
    for (int i = 0; i < xianxhengre; i++)
    {
        if (i > fpscount) break;
        for (int j = 0; j < numth; j++)
        {
            vector<cv::Mat> fpsdanchou;
            vector<double> times;
            cv::Mat linimg;
            cv::Mat linimg2;
            //cap.set(cv::CAP_PROP_POS_FRAMES, 13*interval);
            for (int k = 0; k < eachxianread; k++)
            {
                cap.set(cv::CAP_PROP_POS_FRAMES, nowfps);
                cap.read(linimg);
                linimg2 = linimg.clone();
                //cv::imwrite(to_string(j) + ".jpg", linimg);
                //int nowfps = cap.get(cv::CAP_PROP_POS_FRAMES);
                
                fpsdanchou.push_back(linimg2);
                
                double time = cap.get(cv::CAP_PROP_POS_MSEC) / 1000;
                times.push_back(time);
                
                cout <<"��" + to_string((int)nowfps/interval) +"��ͼ" << endl;
                linimg2.release();
                nowfps += interval;
            }
            fpsdanchou_duoxian.push_back(fpsdanchou);
            times_duoxian.push_back(times);
            fpsdanchou.clear();
            times.clear();
            linimg.release();

        }

        for (int i = 0; i < numth; i++)
        {

            th[i] = thread(getppt, fpsdanchou_duoxian[i], times_duoxian[i], prob, imgpath);
        }

        for (auto& thread : th)
            thread.join();
        
        fpsdanchou_duoxian.clear();
        times_duoxian.clear();
    }
   
    delete th;
    return 0;
}

