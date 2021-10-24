#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

#define NUM 12
#define NUM_FOR_THREAD 10
#define WITH_LOG false

using namespace std;

mutex m;

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
        cout << "I have _date: " + to_string(get_date()) << endl;
        cout << "       _num_of_page: " + to_string(get_num_of_page()) << endl;
        cout << "------------------------------------" << endl;
    }
    virtual ~book() {
        cout << "book: I was deleted!"<< endl;
    }
};

struct st_book : book {
protected:
    string _subject;
public:
    st_book(int num_of_page, int date, string subject="ALGEBRA"): book(num_of_page, date), _subject(move(subject)){
        cout << "--->Created ST_book!" << endl;
    };
    string get_subject() {
        return _subject;
    }

    void about_me() override {
        cout << "I have _date: " + to_string(get_date()) << endl;
        cout << "       _num_of_page: " + to_string(get_num_of_page()) << endl;
        //book::about_me();
        cout << "       _subject: " + get_subject() << endl;
        cout << "------------------------------------" << endl;
    }

    ~st_book() override {
        cout << "st_book: I was deleted! And my parent:" << endl;
        cout << "--->";
    }
};

void count_to_dec(vector<book *> &shell, atomic <uint8_t> &my_thread_atom_sync, const char* ss) {
    int i = 0, len = 0;
    bool check = true;

    my_thread_atom_sync = my_thread_atom_sync + 1;              //atomic имеет защиту от чтения/записи из другого потока

    if (WITH_LOG) {
        cout << "                                                     I am here: " << (int) my_thread_atom_sync << endl;
    }

    while (my_thread_atom_sync > 0) {
        lock_guard<mutex> lk(m);
        for (int j = len; j < shell.size(); j++) {
            if ((string) typeid(*shell[j]).name() == ss) {
                i++;
                if (WITH_LOG) {
                    cout << to_string(j) + " --- Success!!! ---" << typeid(*shell[j]).name() << " = "
                         << ss << endl;
                }
            } else {
                if (WITH_LOG) {
                    cout << to_string(j) + " --- Fault!!! ---" << (string) typeid(*shell[j]).name() << " != "
                         << ss << endl;
                }
            }
        }
        len = (int)shell.size();

        if (check and (i >= NUM_FOR_THREAD)) {
            check = false;
            cout << "                                                       *congratulations*: " << ss << endl;
        }
    }
}

int main() {
    vector <book *> shell;
    atomic <uint8_t> atom_sync;
    atom_sync = 1;

    thread thr_count_book   (&count_to_dec, ref(shell), ref(atom_sync), typeid(book).name());
    thread thr_count_st_book(&count_to_dec, ref(shell), ref(atom_sync), typeid(st_book).name());

    for (int i = 1; i <= NUM; i++) {
        lock_guard<mutex> lk(m);
        cout << "######################################################################" << endl;
        cout << "Iteration: " << i << endl;
        cout << "######################################################################" << endl;
        shell.push_back(new book(12 * i, 2018));
        shell.back()->about_me();
        shell.push_back(new st_book(13 * i, 2020));
        shell.back()->about_me();
    }

    cout << "STOPPING..." << endl;

    while (atom_sync < 3) {           //ждем чтобы thread поток начал выполняться(если еще не начался)
        cout << "wait" << endl;
        this_thread::sleep_for(10ms);
    }

    this_thread::sleep_for(1000ms); //ждем чтобы потоки thread выполнились
    //main может проскочить настолько быстро, что thread будет только создан и удален

    cout << "END OF WAIT!" << endl;
    atom_sync = 0;// atom_sync.store(0);
    thr_count_book.join();
    thr_count_st_book.join();

    cout << "######################################################################" << endl;
    cout << "END! REMOVING..." << endl;
    cout << "######################################################################" << endl;

    for (int i = 0; i < NUM * 2; i++) {
        delete shell[0];
        shell.erase(shell.begin(), shell.begin() + 1);
        cout << "------------------------------------" << endl;

    }

    cout << "SUCCESS!";
    return 0;
}
