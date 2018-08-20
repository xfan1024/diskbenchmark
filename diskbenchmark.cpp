/* 
 * MIT License
 * 
 * Copyright (c) 2018 xiaofan <xfan1024@live.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <stdint.h>
#include <assert.h>

// #define CPP17


using namespace std;

#ifdef CPP17
#include <filesystem>
namespace fs = std::filesystem;
#endif

bool test_write = false;
bool test_read = false;
bool test_path_created = false;
string test_path;
uint64_t test_size = 512*1024*1024;

uint8_t dummy_data[16*1024] = 
{
    0x42, 0xc8, 0x65, 0x82, 0xc6, 0x67, 0x33, 0x07, 0x5e, 0xcc, 0x70, 0x8c, 0x4a, 0x45, 0x58, 0x19,
    0x14, 0xbc, 0x78, 0x73, 0x71, 0xa1, 0x73, 0x94, 0x4e, 0x86, 0x4f, 0xf3, 0x53, 0xf7, 0x27, 0x59,
    0x17, 0xe5, 0x2f, 0x56, 0x6f, 0xaf, 0xda, 0x99, 0xb0, 0xdf, 0xa8, 0xe1, 0x6c, 0x9a, 0x99, 0xae,
    0xc7, 0x82, 0x5b, 0xa9, 0xa3, 0xc8, 0xa0, 0xc8, 0xba, 0x35, 0x5a, 0x50, 0x46, 0x47, 0xa1, 0x45,
    0x86, 0x95, 0xd3, 0x69, 0x62, 0x12, 0x3f, 0x6b, 0x9f, 0x18, 0xb3, 0x10, 0x06, 0x19, 0xad, 0xb7,
    0x72, 0xa0, 0xba, 0x19, 0xc6, 0xbc, 0xb7, 0xc7, 0xd9, 0x77, 0xd8, 0x3f, 0x9d, 0x51, 0xea, 0x8a,
    0x4c, 0x5f, 0x91, 0x00, 0xc1, 0xc9, 0x39, 0xda, 0x5a, 0xb9, 0x92, 0x3d, 0x4c, 0x7a, 0xe2, 0x09,
    0x54, 0x61, 0xed, 0xa1, 0xe3, 0x11, 0xba, 0x81, 0x13, 0x80, 0xfa, 0x2b, 0x7b, 0xa1, 0x27, 0x24,
    0xea, 0x79, 0x83, 0xfa, 0xd6, 0xcd, 0xe2, 0xae, 0x19, 0xa6, 0x2e, 0xfc, 0xca, 0x05, 0x71, 0xe4,
    0x70, 0xb5, 0xe1, 0xab, 0xa6, 0x10, 0x72, 0xb9, 0xcc, 0xbd, 0x55, 0x03, 0x1f, 0x21, 0xac, 0x0a,
    0x3a, 0x37, 0xbb, 0xd2, 0xa4, 0xda, 0x47, 0x8c, 0x09, 0x0d, 0xed, 0x18, 0x0f, 0x28, 0xce, 0x74,
    0x73, 0x95, 0x88, 0xab, 0xbe, 0xc9, 0x0b, 0x4a, 0x30, 0xee, 0x32, 0x6d, 0xc5, 0x43, 0xa7, 0x1f,
    0x3f, 0x63, 0xcb, 0x3c, 0xe4, 0x2a, 0x5b, 0xa5, 0xa0, 0x7e, 0x76, 0x60, 0xce, 0xd5, 0x96, 0x53,
    0x8d, 0x71, 0xb7, 0xb0, 0x8b, 0xa8, 0xf6, 0x49, 0xe8, 0x2a, 0x09, 0xd8, 0xf7, 0x75, 0x7e, 0xb4,
    0x6d, 0x27, 0xb0, 0xba, 0x84, 0x2c, 0x45, 0x4a, 0xdf, 0x51, 0xf7, 0x6c, 0x84, 0x85, 0x0e, 0xcb,
    0xe5, 0xd2, 0x71, 0xf7, 0x2a, 0x22, 0x67, 0x42, 0x5f, 0x33, 0x93, 0x2b, 0x76, 0x0c, 0x9e, 0x83,
};

void dummy_init(void)
{
    uint8_t * dummy_cur = dummy_data + 256;
    uint8_t * dummy_stop = end(dummy_data);
    for (unsigned i = 0; dummy_cur < dummy_stop; i++) {
        for (unsigned int j = 0; j < 256; j++) {
            dummy_cur[j] = dummy_data[j] + i;
        }
        dummy_cur += 256;
    }
}

void usage(const char* prog)
{
    cout << "Usage: << endl;" << endl;
    cout << prog << " <path> <command> [size]" << endl ;
    cout << endl;

    cout << "command: " << endl;
    cout << "    w, write           test write speed, path must not exists" << endl;
    cout << "    r, read            test read speed, path must exists" << endl;
    cout << "    wr, write_read     test write and read speed, path must not exists" << endl;
    cout << endl;

    cout << "size: " << endl;
    cout << "    default to 512M" << endl;
    cout << "    support unit: K, M, G" << endl;
    cout << endl;
}

string strtolower(const string& s)
{
    string r = s;
    for (char& c : r) c = tolower(c);
    return r;
}

string strtoupper(const string& s)
{
    string r = s;
    for (char& c : r) c = toupper(c);
    return r;
}

string dtos(double number, int precision)
{
    std::ostringstream ss;
    
    ss << fixed;
    ss << setprecision(precision);
    ss << number;

    return ss.str();
}

string strsize(uint64_t size)
{
    string res;
    if (size < 10000llu) {
        return to_string(size) + "B";
    } else if (size < 10000llu*1024) {
        res = dtos(size / double(1024), 2) + "KB";
    } else if (size < 10000llu*1024*1024) {
        res = dtos(size / double(1024*1024), 2) + "MB";
    } else {
        res = dtos(size / double(1024*1024*1024), 2) + "GB";
    }
    return res + "(" + to_string(size) + ")";
}

string strspeed(double speed)
{
    string res;
    if (speed < 10000llu) {
        return to_string(speed) + "B/s";
    } else if (speed < 10000llu*1024) {
        return dtos(speed / double(1024), 2) + "KB/s";
    } else if (speed < 10000llu*1024*1024) {
        return dtos(speed / double(1024*1024), 2) + "MB/s";
    } else {
        return dtos(speed / double(1024*1024*1024), 2) + "GB/s";
    }
}

bool parse_args(int argc, char* argv[])
{

    if (argc != 3 && argc != 4) {
        usage(argv[0]);
        return false;
    }
    test_path = argv[1];
    string command = argv[2];
    command = strtolower(command);
    if (command == "w" || command == "write") {
        test_write = true;
    } else if (command == "r" || command == "read") {
        test_read = true;
    } else if (command == "wr" || command == "write_read") {
        test_write = test_read = true;
    } else {
        cout << "error: unsupport command: " << command << endl;
        return false;
    }

    if (argc == 4) {
        string size = argv[3];
        if (size.empty()) {
            cout << "error: empty size sting" << endl;
            return false;
        }
        string number, unit;
        unsigned int unit_base;
        uint64_t file_size; 
        string::size_type i;
        for (i = 0; i < size.length(); i++) {
            if (!isdigit(size[i])) {
                if (i == 0) {
                    cout << "error: invalid size: " << size << endl;
                    return false;
                }
                number = size.substr(0, i);
                unit = strtoupper(size.substr(i));
                if (unit == "" || unit == "B") {
                    unit_base = 1;
                } else if (unit == "K" || unit == "kB") {
                    unit_base = 1024;
                } else if (unit == "M" || unit == "MB") {
                    unit_base = 1024*1024;
                } else if (unit == "G" || unit == "GB") {
                    unit_base = 1024*1024*1024;
                } else {
                    cout << "error: unsupport unit: " << unit << endl;
                    return false;
                }
                file_size = stoull(number);
                file_size *= unit_base;
                test_size = file_size;
                break;
            }
        }
        if (i == size.length()) {
            test_size = stoull(size);
        }
    }
    return true;
}

#ifdef CPP17
bool file_exists(const string& s) {
    return true;
}

bool file_delete(const string& s) {
    return false;
}

#else
bool file_exists(const string& s)
{
    // the result could be wrong in some condition
    ifstream f(s);
    return f.is_open();
}

bool file_delete(const string& s)
{
    if (file_exists(s)) {
        ofstream f(s); // trunc the file
        f << "create by diskbenchmark, please delete me" << endl;
    }
    cout << "warning: sorry, this version don't support delete temperory file, please delete " << s << " manually" << endl;
    return false;
}
#endif

istream::pos_type tellg(istream &s)
{
    bool eof = s.eof();
    if (eof) {
        s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
    }
    auto r = s.tellg();
    if (eof)
        s.clear(s.rdstate() | (ios::eofbit | ios::failbit));
    return r;
}

ostream::pos_type tellp(ostream &s)
{
    bool eof = s.eof();
    if (eof) {
        s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
    }
    auto r = s.tellp();
    if (eof) {
        s.clear(s.rdstate() | (ios::eofbit | ios::failbit));
    }
    return r;
}


bool test_write_process()
{
    bool res = true;

    if (file_exists(test_path)) {
        cout << "error: " << test_path << " is exists" << endl;
        return false;
    }
    ofstream file(test_path, fstream::binary);
    if (!file) {
        cout << "error: " << "open " << test_path << " failed" << endl;
        return false;
    }
    test_path_created = true;
    auto start = chrono::steady_clock::now();
    uint64_t remain = test_size;
    while (remain) {
        streamsize bs = (streamsize)min(remain, (uint64_t)sizeof(dummy_data));
        file.write((char*)dummy_data, bs);
        if (file.fail()) {
            cout << "error: write failed" << endl;
            res = false;
            break;
        }
        remain -= bs;
    }
    if (!remain) {
        file.flush();
        if (file.fail()) {
            cout << "error: flush failed" << endl;
            return false;
        }
    }

    auto elapse_ms = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
    auto endpos = tellp(file);
    cout << "write " << strsize(endpos) << " use " << elapse_ms << "ms" << endl;
    if (elapse_ms) {
        cout << "write speed: " << strspeed(endpos / (elapse_ms / 1000.0)) << endl;
    } else {
        cout << "write too fast to calculate speed, please try a lager size" << endl;
    }
    return res;
}

bool test_read_process()
{
    bool res = true;

    char readbuffer[16*1024];
    ifstream file(test_path, fstream::binary);
    if (!file) {
        cout << "error: open " << test_path << " failed" << endl;
        return false;
    }
    
    auto start = chrono::steady_clock::now();
    uint64_t remain = test_size;
    while (remain) {
        streamsize bs = (streamsize)min(remain, (uint64_t)sizeof(readbuffer));
        file.read(readbuffer, sizeof(readbuffer));
        if (file.eof()) {
            break;
        } 
        if (file.fail()) {
            res = false;
            cout << "error: read failed" << endl;
            break;
        }
        remain -= bs;
    }

    auto elapse_ms = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
    auto endpos = tellg(file);
    if (file.eof()) {
        assert(endpos >= 0 && (uint64_t)endpos < test_size);
        cout << "warning: file size is " << strsize(endpos) << ", less than " << strsize(test_size) << endl;
    }
    
    cout << "read " << strsize(endpos) << " use " << elapse_ms << "ms" << endl;
    if (elapse_ms) {
        cout << "read speed: " << strspeed(endpos / (elapse_ms / 1000.0)) << endl;
    } else {
        cout << "read too fast to calculate speed, please try a lager size" << endl;
    }
    return res;
}

void work()
{
    dummy_init();
    if (test_write && !test_write_process()) {
        cout << "error: write test failed" << endl;
        return;
    }
    if (test_read && !test_read_process()) {
        cout << "error: read test failed" << endl;
        return;
    }
}

int main(int argc, char *argv[])
{
    if (parse_args(argc, argv)) {
        work();
        if (test_write && test_path_created) {
            if (!file_delete(test_path)) {
                cout << "error: delete " << test_path << " failed" << endl;
            }
        }
    }
    return 0;
}
