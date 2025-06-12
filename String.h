#pragma once
#ifndef STRING_H
#define STRING_H
#include <iostream>
using namespace std;
class String {
private:
    char* str;
    int len;
    int cap;
    int lenght(const char* s);
public:
    String();
    String(const char* ptr);
    String(const char c, int s);
    String(int n);
    void print() const;
    String(const String& b);
    String replace_first(char c);
    String& operator=(const String& s);
    int stoi(const String& s);
    String& itos(int n);
    String trim();
    char operator[](int i) const;
    char& operator[](int i);
    bool is_equal(const String s);
    bool is_less(const String s);
    bool is_greater(const String s);
    String* split(char delim, int& count)const;
    String* tokenize(const char* delim, int& count) const;
    String operator+(const String& s) const;
    int* search_string(const char* delim, int& count) const;
    int find_first(char ch);
    int find_first(const String& s) const;
    int find_last(char ch);
    int find_last(const String& s) const;
    int* find_all(char ch, int& c);
    int* find_all(const String& s, int& c) const;
    void remove_at(int i);
    void insert_at(int i, char ch);
    void insert_at(int i, const String sub);
    void remove_first(char ch);
    void remove_last(char ch);
    void remove_all(char ch);
    void clear();
    ~String();
    void upper();
    void lower();
    //friend String& operator+(const String& a, const String& b);
};
#endif !String;
