#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#define NUM 6
#define NUM_FOR_THREAD 4

using namespace std;

timed_mutex m;

class book{
protected:
    int _date;
    int _num_of_page;
public:
    book(int num_of_page, int date): _date(date), _num_of_page(num_of_page){
        cout << "Created book!" << endl;
    };

    int get_date() const {
        return _date;
    }

    int get_num_of_page() const {
        return _num_of_page;
    }

    virtual void about_me() {
        cout << "------------------------------------" << endl;
        cout << "I have _date: " + to_string(get_date()) << endl;
        cout << "       _num_of_page: " + to_string(get_num_of_page()) << endl;
        cout << "------------------------------------" << endl;
    }
    virtual ~book() {
        cout << "book: I was deleted!"<< endl;
    }
};

struct Student_book : book {
protected:
    string _subject;
public:
    Student_book(int num_of_page, int date, string subject="ALGEBRA"): book(num_of_page, date), _subject(move(subject)){
        cout << "--->Created ST_book!" << endl;
    };
    string get_subject() {
        return _subject;
    }
    void about_me() {
        cout << "------------------------------------" << endl;
        cout << "I have _date: " + to_string(get_date()) << endl;
        cout << "       _num_of_page: " + to_string(get_num_of_page()) << endl;
        //book::about_me();
        cout << "       _subject: " + get_subject() << endl;
        cout << "------------------------------------" << endl;
    }
    ~Student_book() {
        cout << "Student_book: I was deleted! And my parent:" << endl;
        cout << "--->";
    }
};

struct str{
    atomic <bool> ulala;
    void count_to_dec(vector <book*> &shell, string ss) {
        int i = 0, len = 0;
        bool check = true;
        ulala = true;
        while (ulala) {
//            std::lock_guard<std::mutex> lk(m);
//            this_thread::sleep_for(10ms);
            if (true) {
                std::lock_guard<std::timed_mutex> lk(m);
                for (int j = len; j < shell.size(); j++) {
                    if ((string) typeid(*shell[j]).name() == ss) {
                        i++;
                        cout << to_string(j) + " --- Success!!! ---" << typeid(*shell[j]).name() << " = " << ss << endl;
                    } else {
                        cout << to_string(j) + " --- Fault!!! ---" << (string) typeid(*shell[j]).name() << " != " << ss
                             << endl;
                    }
                }
                len = (int) shell.size();
                //m.unlock();
            }

            this_thread::sleep_for(1000ms);
            if (check and (i == NUM_FOR_THREAD)) {
                check = !check;
                cout << "                                                                     *congratulations*: " << ss
                     << endl;
            }
        }
    }
};

int main() {
    vector <book*> shell;
    str search_book;
    str search_st_book;
    thread thr_search_book(&str::count_to_dec, &search_st_book, ref(shell), typeid(book).name());
    thread thr_search_st_book(&str::count_to_dec, &search_st_book, ref(shell), typeid(Student_book).name());
//    this_thread::sleep_for(20ms);

    for (int i=1; i<=NUM; ++i) {
        std::lock_guard<std::timed_mutex> lk(m);
        cout <<"######################################################################"<<endl;
        cout << "Iteration: " << i << endl;
        cout <<"######################################################################"<<endl;
        shell.push_back(new book(12*i, 2018));
        shell.back()->about_me();
        shell.push_back(new Student_book(13*i, 2020));
        shell.back()->about_me();
        //this_thread::sleep_for(2500ms);             //?????? ????????????????

    }

    //TODO !TODO
    // ?? ?????? ???? ?????????????????? lock_guard ?????? ???????????????? ??????????????, ??.??. ?????????????????? ?????? ?????? ???????????????? ???? thread
    // (???? ?????? ???????????? ???? ????????????.)
    // (?????? ???? ?? ??????????????, ?????? ??????, ???????????????????????????????? ???????? ???? ???????????????????? ???????????????? ?????????? ???????????? 50/50)
    // ???????? ?????? ?????????? ???????????? ?? deadlock https://habr.com/ru/post/182610/

    cout <<"######################################################################"<<endl;
    cout << "END! REMOVING..." << endl;
    cout <<"######################################################################"<<endl;

    for (int i=0; i<NUM*2; ++i) {
        std::lock_guard<std::timed_mutex> lk(m);
        delete shell[0];
        shell.erase(shell.begin(), shell.begin()+1);
    }

    cout << shell.size() <<endl;

    cout << "STOPPING..." << endl;
    search_book.ulala.store(false);
    search_st_book.ulala.store(false);

    thr_search_book.join();
    thr_search_st_book.join();
    cout <<"SUCCESS!";
    ///CAUTION: NEXT THE DEATH ZONE
}
